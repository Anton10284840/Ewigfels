// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "WorldState/EwigfelsWorldStateTypes.h"
#include "EwigfelsQuestTypes.generated.h"

/**
 * Quests sind reine Daten: eine JSON-Datei pro Quest in Content/Data/Quests/.
 * Format und Beispiel: docs/QUEST_FORMAT.md. Das Prüfskript Scripts/Tools/validate_data.py kennt dieselben Regeln.
 */

UENUM(BlueprintType)
enum class EEwigfelsQuestType : uint8
{
	Main,       // Hauptmission (M101 …)
	Series,     // Questreihe
	Faction,    // Fraktions-Questlinie
	Side        // Nebenquest
};

/** Status einer Quest. Steht zusätzlich als Flag "Quest_<ID>" im Weltstatus (für Bedingungen "quest:ID"). */
UENUM(BlueprintType)
enum class EEwigfelsQuestStatus : uint8
{
	NotStarted = 0,
	Active     = 1,
	Completed  = 2,
	Failed     = 3
};

UENUM(BlueprintType)
enum class EEwigfelsQuestEvent : uint8
{
	Started,
	StageChanged,
	ObjectiveUpdated,
	Completed,
	Failed
};

/** Ein Ziel innerhalb einer Etappe, z. B. „Sprich mit Edda“ oder „Besiege 3 Schattenwölfe“. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsObjectiveDef
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FText Text;

	/** talk, reach, collect, defeat, interact, cast, custom oder condition. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Kind;

	/** Wen/was: NPC-ID, Marker-ID, Item-ID … Bei Kind = condition: die Bedingung als Text. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FString Target;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 Count = 1;

	/** Wegpunkt für Kompass/Karte (ID eines Markers in der Welt). Leer = kein Wegpunkt. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Marker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	bool bOptional = false;

	/** Nur bei Kind = condition, vorab geparst. */
	UPROPERTY()
	FEwigfelsCondition Condition;
};

/** Verzweigung am Ende einer Etappe: erste passende Regel gewinnt. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsStageTransition
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FEwigfelsCondition> Conditions;

	/** Nächste Etappe. Leer, wenn die Quest hier endet. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName NextStage;

	/** Completed oder Failed, wenn die Quest hier endet. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	EEwigfelsQuestStatus EndStatus = EEwigfelsQuestStatus::NotStarted;
};

USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsStageDef
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Id;

	/** Text im Questlog für diese Etappe. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FText Text;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FEwigfelsObjectiveDef> Objectives;

	/** Weltstatus-Änderungen, wenn die Etappe abgeschlossen ist. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FEwigfelsEffect> OnComplete;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FEwigfelsStageTransition> Next;
};

USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsQuestRewards
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 Xp = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 Gold = 0;

	/** Item-IDs (Inventar kommt später, bis dahin nur als Event gemeldet). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FName> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FEwigfelsEffect> Effects;
};

/** Komplette Quest-Definition, geladen aus JSON. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsQuestDef
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	EEwigfelsQuestType Type = EEwigfelsQuestType::Side;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FText Title;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FText Summary;

	/** NPC-ID der Auftraggeberin / des Auftraggebers. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Giver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Region;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 Act = 1;

	/** Geschätzte Spielzeit in Minuten (für PROGRESS.md und das Prüfskript). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 DurationMinutes = 0;

	/** Alle müssen erfüllt sein, damit die Quest starten kann. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FEwigfelsCondition> Requires;

	/** Startet von selbst, sobald Requires erfüllt ist (z. B. Hauptmissionen nacheinander). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	bool bAutoStart = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName StartStage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TArray<FEwigfelsStageDef> Stages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FEwigfelsQuestRewards Rewards;

	const FEwigfelsStageDef* FindStage(FName StageId) const
	{
		return Stages.FindByPredicate([StageId](const FEwigfelsStageDef& S) { return S.Id == StageId; });
	}
};

/** Laufzeit-Stand einer Quest. Wird gespeichert. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsQuestProgress
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName QuestId;

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	EEwigfelsQuestStatus Status = EEwigfelsQuestStatus::NotStarted;

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName StageId;

	/** Fortschritt pro Ziel-ID der aktuellen Etappe. */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	TMap<FName, int32> ObjectiveCounts;

	/** Reihenfolge, in der Quests gestartet wurden (Questlog sortieren). */
	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 StartOrder = 0;
};

/** Alles, was das Questsystem im Spielstand ablegt. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsQuestSaveData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TArray<FEwigfelsQuestProgress> Quests;

	UPROPERTY(SaveGame)
	FName TrackedQuest;

	UPROPERTY(SaveGame)
	int32 NextStartOrder = 0;
};

/** Ein Ziel für die Questlog-/HUD-Anzeige. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsObjectiveView
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Quest")
	FText Text;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 Current = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Quest")
	int32 Required = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Quest")
	bool bDone = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Quest")
	bool bOptional = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Quest")
	FName Marker;
};

namespace EwigfelsQuest
{
	/** Flag-Name, unter dem der Status im Weltstatus steht: "Quest_<ID>". */
	EWIGFELS_API FName StatusFlag(FName QuestId);

	/** Liest eine Quest-JSON-Datei. Fehler landen in OutErrors (mit Dateiname), Rückgabe false bei schweren Fehlern. */
	EWIGFELS_API bool ParseQuestJson(const FString& JsonText, const FString& SourceName, FEwigfelsQuestDef& OutQuest, TArray<FString>& OutErrors);
}
