// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "World/EwigfelsMirror.h"
#include "Appearance/EwigfelsAppearanceComponent.h"
#include "Appearance/EwigfelsAppearanceSubsystem.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "UI/EwigfelsCharacterCreator.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#define LOCTEXT_NAMESPACE "EwigfelsMirror"

AEwigfelsMirror::AEwigfelsMirror()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // im Koop sehen alle den Spiegel (der Server prüft die Nähe)

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));

	// Spiegel zeigt nach +X (vorne). Würfel = 100 cm → Rahmen 12 × 100 × 210 cm
	Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	Frame->SetupAttachment(RootComponent);
	Frame->SetCollisionProfileName(TEXT("BlockAll"));
	Frame->SetRelativeLocation(FVector(0.f, 0.f, 105.f));
	Frame->SetRelativeScale3D(FVector(0.12f, 1.0f, 2.1f));

	Glass = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Glass"));
	Glass->SetupAttachment(RootComponent);
	Glass->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Glass->SetRelativeLocation(FVector(7.f, 0.f, 105.f));
	Glass->SetRelativeScale3D(FVector(0.02f, 0.85f, 1.9f));
}

void AEwigfelsMirror::BeginPlay()
{
	Super::BeginPlay();
	if (UStaticMesh* Custom = MirrorMesh.LoadSynchronous())
	{
		Frame->SetStaticMesh(Custom);
		Frame->SetRelativeLocation(FVector::ZeroVector);
		Frame->SetRelativeScale3D(FVector::OneVector);
		Glass->SetVisibility(false);
		return;
	}
	// Platzhalter: dunkler Holzrahmen, Glas leuchtet kühl-silbern
	Frame->SetStaticMesh(EwigfelsSpellVisuals::GetShape(TEXT("Cube")));
	Glass->SetStaticMesh(EwigfelsSpellVisuals::GetShape(TEXT("Cube")));
	EwigfelsSpellVisuals::ApplySolid(Frame, FLinearColor(0.09f, 0.05f, 0.03f), 0.6f);
	EwigfelsSpellVisuals::ApplyGlow(Glass, FLinearColor(0.65f, 0.75f, 0.9f), 0.6f);
}

FText AEwigfelsMirror::GetInteractPrompt_Implementation() const
{
	const UGameInstance* GI = GetGameInstance();
	const UEwigfelsAppearanceSubsystem* Sub = GI ? GI->GetSubsystem<UEwigfelsAppearanceSubsystem>() : nullptr;
	const FEwigfelsLookShop* Shop = Sub ? Sub->GetCatalog().Shops.Find(ShopId) : nullptr;
	return FText::Format(LOCTEXT("Prompt", "In den Spiegel schauen ({0})"), Shop ? Shop->DisplayName : FText::FromName(ShopId));
}

void AEwigfelsMirror::Interact_Implementation(APawn* InstigatorPawn)
{
	const APlayerController* PC = InstigatorPawn ? Cast<APlayerController>(InstigatorPawn->GetController()) : nullptr;
	ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
	UEwigfelsAppearanceComponent* Look = InstigatorPawn ? InstigatorPawn->FindComponentByClass<UEwigfelsAppearanceComponent>() : nullptr;
	if (UEwigfelsCreatorSubsystem* Creator = LocalPlayer ? LocalPlayer->GetSubsystem<UEwigfelsCreatorSubsystem>() : nullptr)
	{
		if (Look && Look->IsCreated())
		{
			Creator->Open(Look, ShopId);
		}
	}
}

#undef LOCTEXT_NAMESPACE
