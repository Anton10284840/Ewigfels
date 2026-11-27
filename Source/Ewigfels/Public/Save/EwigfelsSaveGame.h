// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WorldState/EwigfelsWorldStateTypes.h"
#include "Quest/EwigfelsQuestTypes.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "EwigfelsSaveGame.generated.h"

/** Version des Spielstand-Formats. Erhöhen, wenn sich Felder ändern, und in UEwigfelsSaveSubsystem::Migrate nachziehen. */
namespace EwigfelsSave
{
	/** v2 (27.11.2026): Aussehen der Spielfigur. */
	constexpr int32 CurrentVersion = 2;
}

/** Gespeicherter Zustand eines Actors mit UEwigfelsSaveableComponent (Truhe, Tür, Sammelobjekt, NPC …). */
USTRUCT()
struct EWIGFELS_API FEwigfelsActorRecord
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FTransform Transform;

	UPROPERTY(SaveGame)
	bool bHasTransform = false;

	/** Dauerhaft entfernt (z. B. eingesammelt). Wird beim Laden wieder zerstört. */
	UPROPERTY(SaveGame)
	bool bDestroyed = false;

	/** Alle UPROPERTY(SaveGame)-Felder des Actors, binär. */
	UPROPERTY(SaveGame)
	TArray<uint8> Data;
};

/** Kurzinfo für das Lade-Menü. Liegt zusätzlich im Slot-Index, damit das Menü nicht jeden Spielstand öffnen muss. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	FString SlotName;

	/** Vom Spiel erzeugt, z. B. „Akademie – M104 Die erste Stimme“. */
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	FString DisplayName;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	FDateTime SavedAt;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	double PlayTimeSeconds = 0.0;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	FString LevelName;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	FName TrackedQuest;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	bool bAutosave = false;

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Save")
	int32 SaveVersion = 0;
};

/** Ein kompletter Spielstand. */
UCLASS()
class EWIGFELS_API UEwigfelsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	int32 SaveVersion = EwigfelsSave::CurrentVersion;

	UPROPERTY(SaveGame)
	FEwigfelsSlotInfo Info;

	UPROPERTY(SaveGame)
	FEwigfelsWorldStateData WorldState;

	UPROPERTY(SaveGame)
	FEwigfelsQuestSaveData Quests;

	/** Globale SUDS-Variablen (global.* in Dialogskripten), binär über den Serializer von SUDS. */
	UPROPERTY(SaveGame)
	TArray<uint8> DialogueGlobals;

	UPROPERTY(SaveGame)
	FString LevelName;

	UPROPERTY(SaveGame)
	FTransform PlayerTransform;

	UPROPERTY(SaveGame)
	bool bHasPlayerTransform = false;

	/** Aussehen der Figur des speichernden Spielers (Koop-Gäste bringen ihr Aussehen aus ihrem eigenen Profil mit). */
	UPROPERTY(SaveGame)
	FEwigfelsAppearance PlayerAppearance;

	UPROPERTY(SaveGame)
	bool bHasPlayerAppearance = false;

	/** Schlüssel = SaveId des Actors (siehe UEwigfelsSaveableComponent). */
	UPROPERTY(SaveGame)
	TMap<FName, FEwigfelsActorRecord> Actors;
};

/** Liste aller Slots fürs Menü. Eigene kleine Datei "EwigfelsSlotIndex". */
UCLASS()
class EWIGFELS_API UEwigfelsSlotIndex : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TArray<FEwigfelsSlotInfo> Slots;

	/** Nächster Autosave-Platz (rotiert über Auto_0 … Auto_N). */
	UPROPERTY(SaveGame)
	int32 NextAutosave = 0;
};
