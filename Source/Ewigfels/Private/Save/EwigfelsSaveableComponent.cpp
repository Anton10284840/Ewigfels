// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Save/EwigfelsSaveableComponent.h"
#include "Save/EwigfelsSaveGame.h"
#include "Save/EwigfelsSaveSubsystem.h"
#include "Ewigfels.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

namespace
{
	UEwigfelsSaveSubsystem* GetSaveSubsystem(const UActorComponent* Component)
	{
		const UWorld* World = Component->GetWorld();
		UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		return GameInstance ? GameInstance->GetSubsystem<UEwigfelsSaveSubsystem>() : nullptr;
	}
}

UEwigfelsSaveableComponent::UEwigfelsSaveableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FName UEwigfelsSaveableComponent::GetEffectiveSaveId() const
{
	if (!SaveId.IsNone())
	{
		return SaveId;
	}
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return NAME_None;
	}
	// Platzierte Actors: Pfad ohne PIE-Präfix ist stabil (z. B. /Game/Maps/Velmark.Velmark:PersistentLevel.Truhe_12).
	return FName(*UWorld::RemovePIEPrefix(Owner->GetPathName()));
}

void UEwigfelsSaveableComponent::WriteRecord(FEwigfelsActorRecord& OutRecord) const
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	OutRecord.bHasTransform = bSaveTransform;
	if (bSaveTransform)
	{
		OutRecord.Transform = Owner->GetActorTransform();
	}
	OutRecord.Data.Reset();
	FMemoryWriter Writer(OutRecord.Data, true);
	FObjectAndNameAsStringProxyArchive Ar(Writer, true);
	Ar.ArIsSaveGame = true; // nur Felder mit SaveGame-Häkchen
	Owner->Serialize(Ar);
}

void UEwigfelsSaveableComponent::ReadRecord(const FEwigfelsActorRecord& Record)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}
	if (Record.bDestroyed)
	{
		Owner->Destroy();
		return;
	}
	if (Record.bHasTransform)
	{
		Owner->SetActorTransform(Record.Transform, false, nullptr, ETeleportType::TeleportPhysics);
	}
	if (Record.Data.Num() > 0)
	{
		FMemoryReader Reader(Record.Data, true);
		FObjectAndNameAsStringProxyArchive Ar(Reader, true);
		Ar.ArIsSaveGame = true;
		Owner->Serialize(Ar);
	}
	OnRestored.Broadcast();
}

void UEwigfelsSaveableComponent::DestroyPersistently()
{
	if (UEwigfelsSaveSubsystem* Save = GetSaveSubsystem(this))
	{
		Save->MarkDestroyed(GetEffectiveSaveId());
	}
	if (AActor* Owner = GetOwner())
	{
		Owner->Destroy();
	}
}

void UEwigfelsSaveableComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner() && !GetOwner()->HasAnyFlags(RF_WasLoaded) && SaveId.IsNone())
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Save: %s wurde zur Laufzeit gespawnt und hat keine SaveId - Zustand ist nach dem Laden evtl. weg."),
			*GetOwner()->GetName());
	}
	if (UEwigfelsSaveSubsystem* Save = GetSaveSubsystem(this))
	{
		Save->RegisterSaveable(this); // stellt gespeicherten Zustand sofort her
	}
}

void UEwigfelsSaveableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UEwigfelsSaveSubsystem* Save = GetSaveSubsystem(this))
	{
		// Entladen durch Streaming (World Partition) oder Levelwechsel: Zustand merken. Destroyed: DestroyPersistently regelt das.
		const bool bCapture = EndPlayReason == EEndPlayReason::RemovedFromWorld || EndPlayReason == EEndPlayReason::LevelTransition;
		Save->UnregisterSaveable(this, bCapture);
	}
	Super::EndPlay(EndPlayReason);
}
