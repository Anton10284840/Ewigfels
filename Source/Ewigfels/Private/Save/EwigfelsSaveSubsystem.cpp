// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Save/EwigfelsSaveSubsystem.h"
#include "Save/EwigfelsSaveableComponent.h"
#include "WorldState/EwigfelsWorldStateSubsystem.h"
#include "Quest/EwigfelsQuestSubsystem.h"
#include "Ewigfels.h"
#include "SUDSSubsystem.h"
#include "Appearance/EwigfelsAppearanceComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "UObject/UObjectGlobals.h"

const TCHAR* UEwigfelsSaveSubsystem::SlotIndexName = TEXT("EwigfelsSlotIndex");

void UEwigfelsSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	WorldState = Collection.InitializeDependency<UEwigfelsWorldStateSubsystem>();
	Quests = Collection.InitializeDependency<UEwigfelsQuestSubsystem>();
	Collection.InitializeDependency<USUDSSubsystem>();

	SessionStartSeconds = FPlatformTime::Seconds();
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UEwigfelsSaveSubsystem::HandlePostLoadMap);
	SetAutosaveInterval(AutosaveInterval);
}

void UEwigfelsSaveSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	FTSTicker::GetCoreTicker().RemoveTicker(AutosaveHandle);
	Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// Speichern

UEwigfelsSaveGame* UEwigfelsSaveSubsystem::CreateSnapshot(const FString& SlotName, const FString& DisplayName, bool bAutosave)
{
	UEwigfelsSaveGame* SaveGame = Cast<UEwigfelsSaveGame>(UGameplayStatics::CreateSaveGameObject(UEwigfelsSaveGame::StaticClass()));
	if (!SaveGame)
	{
		return nullptr;
	}
	UWorld* World = GetGameInstance()->GetWorld();

	SaveGame->SaveVersion = EwigfelsSave::CurrentVersion;
	if (WorldState)
	{
		SaveGame->WorldState = WorldState->GetData();
	}
	if (Quests)
	{
		SaveGame->Quests = Quests->GetSaveData();
	}
	if (USUDSSubsystem* Suds = GetGameInstance()->GetSubsystem<USUDSSubsystem>())
	{
		// Nur die Variablen-Map serialisieren: SUDS deklariert operator<<(FArchive&, FSUDSGlobalState&),
		// definiert ihn aber nirgends (LNK2019 beim ersten Build, 26.11.2026). FSUDSValue hat einen echten Serializer.
		TMap<FName, FSUDSValue> Vars = Suds->GetSavedGlobalState().GetGlobalVariables();
		FMemoryWriter Writer(SaveGame->DialogueGlobals, true);
		Writer << Vars;
	}

	SaveGame->LevelName = GetLevelPackageName(World);
	if (APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr)
	{
		if (const APawn* Pawn = PC->GetPawn())
		{
			SaveGame->PlayerTransform = Pawn->GetActorTransform();
			SaveGame->bHasPlayerTransform = true;
			if (const UEwigfelsAppearanceComponent* Look = Pawn->FindComponentByClass<UEwigfelsAppearanceComponent>())
			{
				if (Look->IsCreated())
				{
					SaveGame->PlayerAppearance = Look->GetAppearance();
					SaveGame->bHasPlayerAppearance = true;
				}
			}
		}
	}

	// Geladene Actors frisch erfassen, entladene stehen schon in Records.
	for (const TPair<FName, TWeakObjectPtr<UEwigfelsSaveableComponent>>& Pair : Registered)
	{
		if (const UEwigfelsSaveableComponent* Component = Pair.Value.Get())
		{
			Component->WriteRecord(Records.FindOrAdd(Pair.Key));
		}
	}
	SaveGame->Actors = Records;

	FEwigfelsSlotInfo& Info = SaveGame->Info;
	Info.SlotName = SlotName;
	Info.DisplayName = DisplayName.IsEmpty() ? MakeDefaultDisplayName() : DisplayName;
	Info.SavedAt = FDateTime::Now();
	Info.PlayTimeSeconds = GetPlayTimeSeconds();
	Info.LevelName = SaveGame->LevelName;
	Info.TrackedQuest = SaveGame->Quests.TrackedQuest;
	Info.bAutosave = bAutosave;
	Info.SaveVersion = EwigfelsSave::CurrentVersion;
	return SaveGame;
}

bool UEwigfelsSaveSubsystem::SaveToSlot(const FString& SlotName, const FString& DisplayName)
{
	if (SlotName.IsEmpty() || SlotName == SlotIndexName || bSaveInFlight || PendingLoad)
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Save: Speichern in '%s' gerade nicht moeglich"), *SlotName);
		return false;
	}
	return WriteAsync(CreateSnapshot(SlotName, DisplayName, false));
}

bool UEwigfelsSaveSubsystem::Autosave()
{
	const UWorld* World = GetGameInstance()->GetWorld();
	const APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	if (!PC || !PC->GetPawn() || AutosaveBlockers.Num() > 0 || bSaveInFlight || PendingLoad)
	{
		return false;
	}
	UEwigfelsSlotIndex* Index = LoadOrCreateIndex();
	const int32 Slot = FMath::Clamp(Index->NextAutosave, 0, NumAutosaveSlots - 1);
	Index->NextAutosave = (Slot + 1) % NumAutosaveSlots;
	UGameplayStatics::SaveGameToSlot(Index, SlotIndexName, 0);

	return WriteAsync(CreateSnapshot(FString::Printf(TEXT("Auto_%d"), Slot), FString(), true));
}

bool UEwigfelsSaveSubsystem::WriteAsync(UEwigfelsSaveGame* SaveGame)
{
	if (!SaveGame)
	{
		return false;
	}
	bSaveInFlight = true;
	UpdateSlotIndex(SaveGame->Info, false);
	UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SaveGame->Info.SlotName, 0,
		FAsyncSaveGameToSlotDelegate::CreateUObject(this, &UEwigfelsSaveSubsystem::HandleAsyncSaveDone));
	return true;
}

void UEwigfelsSaveSubsystem::HandleAsyncSaveDone(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	bSaveInFlight = false;
	if (!bSuccess)
	{
		UE_LOG(LogEwigfels, Error, TEXT("Save: Schreiben von '%s' fehlgeschlagen"), *SlotName);
		FEwigfelsSlotInfo Info;
		Info.SlotName = SlotName;
		UpdateSlotIndex(Info, true);
	}
	else
	{
		UE_LOG(LogEwigfels, Log, TEXT("Save: '%s' gespeichert"), *SlotName);
	}
	OnSaveFinished.Broadcast(SlotName, bSuccess);
}

// ---------------------------------------------------------------------------
// Laden

bool UEwigfelsSaveSubsystem::LoadFromSlot(const FString& SlotName)
{
	UEwigfelsSaveGame* SaveGame = Cast<UEwigfelsSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	if (!SaveGame || !Migrate(SaveGame))
	{
		UE_LOG(LogEwigfels, Error, TEXT("Save: '%s' nicht lesbar"), *SlotName);
		OnLoadFinished.Broadcast(SlotName, false);
		return false;
	}

	// Systemzustand sofort einspielen, Actors/Spieler erst, wenn das richtige Level da ist.
	ApplySnapshot(SaveGame);

	UWorld* World = GetGameInstance()->GetWorld();
	if (World && !SaveGame->LevelName.IsEmpty() && GetLevelPackageName(World) != SaveGame->LevelName)
	{
		PendingLoad = SaveGame;
		UGameplayStatics::OpenLevel(World, FName(*SaveGame->LevelName));
		return true; // weiter in HandlePostLoadMap
	}

	PendingLoad = SaveGame;
	ApplyToLoadedWorld();
	return true;
}

void UEwigfelsSaveSubsystem::ApplySnapshot(const UEwigfelsSaveGame* SaveGame)
{
	if (!SaveGame)
	{
		return;
	}
	if (WorldState)
	{
		WorldState->ReplaceData(SaveGame->WorldState);
	}
	if (Quests)
	{
		Quests->LoadSaveData(SaveGame->Quests);
		Quests->StartAvailableAutoQuests(); // neue Quests aus einem Update, deren Voraussetzung schon erfüllt ist
	}
	if (USUDSSubsystem* Suds = GetGameInstance()->GetSubsystem<USUDSSubsystem>())
	{
		if (SaveGame->DialogueGlobals.Num() > 0)
		{
			// Gegenstück zu CreateSnapshot (nur die Map, siehe LNK2019-Hinweis dort).
			TMap<FName, FSUDSValue> Vars;
			FMemoryReader Reader(SaveGame->DialogueGlobals, true);
			Reader << Vars;
			Suds->RestoreSavedGlobalState(FSUDSGlobalState(Vars));
		}
		else
		{
			Suds->ResetGlobalState(true);
		}
	}
	Records = SaveGame->Actors;
	PlayTimeBeforeSession = SaveGame->Info.PlayTimeSeconds;
	SessionStartSeconds = FPlatformTime::Seconds();
}

void UEwigfelsSaveSubsystem::HandlePostLoadMap(UWorld* World)
{
	if (PendingLoad && World && World->GetGameInstance() == GetGameInstance())
	{
		ApplyToLoadedWorld();
	}
}

void UEwigfelsSaveSubsystem::ApplyToLoadedWorld()
{
	UEwigfelsSaveGame* SaveGame = PendingLoad;
	PendingLoad = nullptr;
	if (!SaveGame)
	{
		return;
	}

	// Bereits geladene Saveables auf den Spielstand setzen (später gestreamte erledigen das selbst im BeginPlay).
	TArray<TWeakObjectPtr<UEwigfelsSaveableComponent>> Components;
	Registered.GenerateValueArray(Components);
	for (const TWeakObjectPtr<UEwigfelsSaveableComponent>& Weak : Components)
	{
		UEwigfelsSaveableComponent* Component = Weak.Get();
		if (!Component)
		{
			continue;
		}
		if (const FEwigfelsActorRecord* Record = Records.Find(Component->GetEffectiveSaveId()))
		{
			Component->ReadRecord(*Record);
		}
	}

	bHasLoadedAppearance = SaveGame->bHasPlayerAppearance;
	LoadedAppearance = SaveGame->PlayerAppearance;

	// Spielerposition: Pawn existiert evtl. erst nach BeginPlay des GameModes, daher hier ohne Garantie.
	const UWorld* World = GetGameInstance()->GetWorld();
	APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
	if (APawn* Pawn = PC ? PC->GetPawn() : nullptr)
	{
		if (SaveGame->bHasPlayerTransform)
		{
			Pawn->SetActorTransform(SaveGame->PlayerTransform, false, nullptr, ETeleportType::TeleportPhysics);
			PC->SetControlRotation(SaveGame->PlayerTransform.Rotator());
		}
		if (SaveGame->bHasPlayerAppearance)
		{
			if (UEwigfelsAppearanceComponent* Look = Pawn->FindComponentByClass<UEwigfelsAppearanceComponent>())
			{
				Look->SetAppearanceAuthority(SaveGame->PlayerAppearance);
			}
		}
	}
	else if (SaveGame->bHasPlayerTransform)
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Save: Kein Spieler-Pawn beim Laden - Position nicht gesetzt (GameMode muss das spaeter nachholen)."));
	}

	UE_LOG(LogEwigfels, Log, TEXT("Save: '%s' geladen"), *SaveGame->Info.SlotName);
	OnLoadFinished.Broadcast(SaveGame->Info.SlotName, true);
}

bool UEwigfelsSaveSubsystem::Migrate(UEwigfelsSaveGame* SaveGame)
{
	if (SaveGame->SaveVersion > EwigfelsSave::CurrentVersion)
	{
		UE_LOG(LogEwigfels, Error, TEXT("Save: Spielstand ist neuer (v%d) als das Spiel (v%d)"), SaveGame->SaveVersion, EwigfelsSave::CurrentVersion);
		return false;
	}
	if (SaveGame->SaveVersion < 2)
	{
		// v1 kannte kein Aussehen: bHasPlayerAppearance bleibt false, die Figur behält ihr aktuelles Aussehen
		// (oder der Charakter-Editor öffnet sich, falls sie noch keins hat).
		SaveGame->bHasPlayerAppearance = false;
	}
	// Hier künftig: if (SaveGame->SaveVersion < 3) { … } usw.
	SaveGame->SaveVersion = EwigfelsSave::CurrentVersion;
	return true;
}

// ---------------------------------------------------------------------------
// Slots

bool UEwigfelsSaveSubsystem::DeleteSlot(const FString& SlotName)
{
	FEwigfelsSlotInfo Info;
	Info.SlotName = SlotName;
	UpdateSlotIndex(Info, true);
	return UGameplayStatics::DeleteGameInSlot(SlotName, 0);
}

TArray<FEwigfelsSlotInfo> UEwigfelsSaveSubsystem::GetSlots() const
{
	TArray<FEwigfelsSlotInfo> Slots = LoadOrCreateIndex()->Slots;
	Slots.RemoveAll([](const FEwigfelsSlotInfo& Info) { return !UGameplayStatics::DoesSaveGameExist(Info.SlotName, 0); });
	Slots.Sort([](const FEwigfelsSlotInfo& A, const FEwigfelsSlotInfo& B) { return A.SavedAt > B.SavedAt; });
	return Slots;
}

UEwigfelsSlotIndex* UEwigfelsSaveSubsystem::LoadOrCreateIndex() const
{
	if (UEwigfelsSlotIndex* Index = Cast<UEwigfelsSlotIndex>(UGameplayStatics::LoadGameFromSlot(SlotIndexName, 0)))
	{
		return Index;
	}
	return Cast<UEwigfelsSlotIndex>(UGameplayStatics::CreateSaveGameObject(UEwigfelsSlotIndex::StaticClass()));
}

void UEwigfelsSaveSubsystem::UpdateSlotIndex(const FEwigfelsSlotInfo& Info, bool bRemove)
{
	UEwigfelsSlotIndex* Index = LoadOrCreateIndex();
	Index->Slots.RemoveAll([&Info](const FEwigfelsSlotInfo& Existing) { return Existing.SlotName == Info.SlotName; });
	if (!bRemove)
	{
		Index->Slots.Add(Info);
	}
	UGameplayStatics::SaveGameToSlot(Index, SlotIndexName, 0);
}

// ---------------------------------------------------------------------------
// Neues Spiel, Autosave, Spielzeit

void UEwigfelsSaveSubsystem::StartNewGame()
{
	bHasLoadedAppearance = false;
	LoadedAppearance = FEwigfelsAppearance();
	if (WorldState)
	{
		WorldState->ReplaceData(FEwigfelsWorldStateData());
	}
	if (USUDSSubsystem* Suds = GetGameInstance()->GetSubsystem<USUDSSubsystem>())
	{
		Suds->ResetGlobalState(true);
	}
	Records.Reset();
	PendingLoad = nullptr;
	PlayTimeBeforeSession = 0.0;
	SessionStartSeconds = FPlatformTime::Seconds();
	if (Quests)
	{
		Quests->ResetProgress();
		Quests->StartAvailableAutoQuests(); // z. B. M101
	}
}

void UEwigfelsSaveSubsystem::SetAutosaveInterval(float Seconds)
{
	FTSTicker::GetCoreTicker().RemoveTicker(AutosaveHandle);
	AutosaveHandle.Reset();
	AutosaveInterval = Seconds;
	if (Seconds > 0.f)
	{
		AutosaveHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateUObject(this, &UEwigfelsSaveSubsystem::HandleAutosaveTick), Seconds);
	}
}

bool UEwigfelsSaveSubsystem::HandleAutosaveTick(float DeltaTime)
{
	const UWorld* World = GetGameInstance() ? GetGameInstance()->GetWorld() : nullptr;
	if (World && World->IsGameWorld() && !World->IsPaused())
	{
		Autosave();
	}
	return true; // Ticker weiterlaufen lassen
}

double UEwigfelsSaveSubsystem::GetPlayTimeSeconds() const
{
	return PlayTimeBeforeSession + (FPlatformTime::Seconds() - SessionStartSeconds);
}

FString UEwigfelsSaveSubsystem::MakeDefaultDisplayName() const
{
	const UWorld* World = GetGameInstance()->GetWorld();
	FString Name = World ? UGameplayStatics::GetCurrentLevelName(World, true) : FString(TEXT("Velmark"));
	if (Quests && !Quests->GetTrackedQuest().IsNone())
	{
		Name += TEXT(" - ") + Quests->GetTrackedQuest().ToString();
	}
	return Name;
}

FString UEwigfelsSaveSubsystem::GetLevelPackageName(const UWorld* World)
{
	return World ? UWorld::RemovePIEPrefix(World->GetPackage()->GetName()) : FString();
}

// ---------------------------------------------------------------------------
// Saveable-Actors

void UEwigfelsSaveSubsystem::RegisterSaveable(UEwigfelsSaveableComponent* Component)
{
	const FName Id = Component ? Component->GetEffectiveSaveId() : NAME_None;
	if (Id.IsNone())
	{
		return;
	}
	if (const TWeakObjectPtr<UEwigfelsSaveableComponent>* Existing = Registered.Find(Id))
	{
		if (Existing->IsValid() && Existing->Get() != Component)
		{
			UE_LOG(LogEwigfels, Warning, TEXT("Save: SaveId '%s' ist doppelt vergeben"), *Id.ToString());
		}
	}
	Registered.Add(Id, Component);
	if (const FEwigfelsActorRecord* Record = Records.Find(Id))
	{
		Component->ReadRecord(*Record);
	}
}

void UEwigfelsSaveSubsystem::UnregisterSaveable(UEwigfelsSaveableComponent* Component, bool bCaptureState)
{
	const FName Id = Component ? Component->GetEffectiveSaveId() : NAME_None;
	if (Id.IsNone())
	{
		return;
	}
	// Während eines Ladevorgangs würde das alte Level sonst die frisch geladenen Zustände überschreiben.
	if (bCaptureState && !PendingLoad)
	{
		FEwigfelsActorRecord& Record = Records.FindOrAdd(Id);
		if (!Record.bDestroyed)
		{
			Component->WriteRecord(Record);
		}
	}
	const TWeakObjectPtr<UEwigfelsSaveableComponent>* Existing = Registered.Find(Id);
	if (Existing && Existing->Get() == Component)
	{
		Registered.Remove(Id);
	}
}

void UEwigfelsSaveSubsystem::MarkDestroyed(FName SaveId)
{
	if (!SaveId.IsNone())
	{
		FEwigfelsActorRecord& Record = Records.FindOrAdd(SaveId);
		Record.bDestroyed = true;
		Record.Data.Reset();
	}
}

// ---------------------------------------------------------------------------
// Konsolenbefehle

namespace
{
	UEwigfelsSaveSubsystem* FindSave(UWorld* World)
	{
		UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		return GameInstance ? GameInstance->GetSubsystem<UEwigfelsSaveSubsystem>() : nullptr;
	}

	FAutoConsoleCommandWithWorldAndArgs GSaveCmd(TEXT("Ewigfels.Save"), TEXT("Speichern: Ewigfels.Save Slot_1"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (UEwigfelsSaveSubsystem* Save = FindSave(World))
			{
				Save->SaveToSlot(Args.Num() > 0 ? Args[0] : FString(TEXT("Quick")), FString());
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GLoadCmd(TEXT("Ewigfels.Load"), TEXT("Laden: Ewigfels.Load Slot_1"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (UEwigfelsSaveSubsystem* Save = FindSave(World))
			{
				Save->LoadFromSlot(Args.Num() > 0 ? Args[0] : FString(TEXT("Quick")));
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GAutosaveCmd(TEXT("Ewigfels.Autosave"), TEXT("Sofort automatisch speichern"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (UEwigfelsSaveSubsystem* Save = FindSave(World))
			{
				UE_LOG(LogEwigfels, Display, TEXT("Autosave %s"), Save->Autosave() ? TEXT("gestartet") : TEXT("gerade gesperrt"));
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GListCmd(TEXT("Ewigfels.Save.List"), TEXT("Alle Spielstaende auflisten"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (UEwigfelsSaveSubsystem* Save = FindSave(World))
			{
				for (const FEwigfelsSlotInfo& Info : Save->GetSlots())
				{
					UE_LOG(LogEwigfels, Display, TEXT("%-8s %s  %s  %.0f min"), *Info.SlotName, *Info.SavedAt.ToString(),
						*Info.DisplayName, Info.PlayTimeSeconds / 60.0);
				}
			}
		}));
}
