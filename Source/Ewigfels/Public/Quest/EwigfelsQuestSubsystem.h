// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Quest/EwigfelsQuestTypes.h"
#include "EwigfelsQuestSubsystem.generated.h"

class UEwigfelsWorldStateSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEwigfelsQuestEventSignature,
	FName, QuestId, EEwigfelsQuestEvent, Event, FName, StageOrObjective);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEwigfelsQuestRewardSignature,
	FName, QuestId, const FEwigfelsQuestRewards&, Rewards);

/**
 * Datengetriebenes Questsystem. Lädt beim Start alle JSON-Dateien aus Content/Data/Quests/.
 *
 * Wie das Spiel Fortschritt meldet (aus C++, Blueprint oder einem SUDS-Dialog-Event):
 *   ReportEvent("talk", "Edda")          → Ziel {kind: talk, target: Edda} zählt hoch
 *   ReportEvent("defeat", "Schattenwolf") → …
 * Ziele mit kind "condition" erledigen sich selbst, sobald der Weltstatus passt.
 *
 * Konsole (~):  Ewigfels.Quest.List · Ewigfels.Quest.Start M101 · Ewigfels.Quest.Report talk Edda · Ewigfels.Quest.Reload
 */
UCLASS()
class EWIGFELS_API UEwigfelsQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Started, StageChanged, ObjectiveUpdated, Completed, Failed. Für Questlog, HUD-Hinweise, Musik-Stinger. */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Quest")
	FEwigfelsQuestEventSignature OnQuestEvent;

	/** Belohnung beim Abschluss. XP/Gold/Items holt sich später das Inventar/Progressionssystem hier ab. */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Quest")
	FEwigfelsQuestRewardSignature OnQuestRewarded;

	// --- Aktionen ---

	/** Startet die Quest, wenn sie noch nicht lief und ihre Voraussetzungen erfüllt sind (oder bForce). */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Quest")
	bool StartQuest(FName QuestId, bool bForce = false);

	/** Meldet ein Spielereignis an alle aktiven Ziele. Rückgabe: Anzahl Ziele, die weitergezählt haben. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Quest")
	int32 ReportEvent(FName Kind, const FString& Target, int32 Amount = 1);

	/** Hakt ein Ziel direkt ab (z. B. aus einem Skript oder Dialog). */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Quest")
	bool CompleteObjective(FName QuestId, FName ObjectiveId);

	/** Springt zu einer Etappe (Debug oder Sonderfälle aus Dialogen). */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Quest")
	bool SetStage(FName QuestId, FName StageId);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Quest")
	void FailQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Quest")
	void SetTrackedQuest(FName QuestId);

	// --- Abfragen (Questlog, HUD, Kompass) ---

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	EEwigfelsQuestStatus GetQuestStatus(FName QuestId) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	bool CanStartQuest(FName QuestId) const;

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Quest")
	bool GetQuestDefinition(FName QuestId, FEwigfelsQuestDef& OutQuest) const;

	/** Quests mit dem Status, sortiert nach Startreihenfolge. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	TArray<FName> GetQuestsWithStatus(EEwigfelsQuestStatus Status) const;

	/** Ziele der aktuellen Etappe mit Fortschritt. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	TArray<FEwigfelsObjectiveView> GetCurrentObjectives(FName QuestId) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	FText GetCurrentStageText(FName QuestId) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	FName GetTrackedQuest() const { return SaveData.TrackedQuest; }

	/** Wegpunkte der verfolgten Quest (offene Ziele mit Marker). */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	TArray<FName> GetTrackedMarkers() const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Quest")
	int32 GetLoadedQuestCount() const { return Definitions.Num(); }

	// --- Laden der Definitionen ---

	/** Liest alle Quest-JSONs neu ein (Konsole: Ewigfels.Quest.Reload). Gibt die Anzahl der Fehler zurück. */
	int32 ReloadDefinitions();

	/** Für Tests: eine Quest direkt aus JSON-Text hinzufügen. */
	bool AddDefinitionFromJson(const FString& JsonText, const FString& SourceName, TArray<FString>& OutErrors);

	// --- Speichern/Laden ---
	const FEwigfelsQuestSaveData& GetSaveData() const { return SaveData; }
	void LoadSaveData(const FEwigfelsQuestSaveData& InData);
	void ResetProgress();

	FString DescribeState() const;

private:
	FEwigfelsQuestProgress* FindProgress(FName QuestId);
	const FEwigfelsQuestProgress* FindProgress(FName QuestId) const;

	void EnterStage(const FEwigfelsQuestDef& Quest, FName StageId);
	/** Prüft, ob die aktuelle Etappe fertig ist, und schaltet weiter. */
	void TryAdvance(FName QuestId);
	void FinishQuest(const FEwigfelsQuestDef& Quest, FEwigfelsQuestProgress& Progress, EEwigfelsQuestStatus EndStatus);
	bool IsStageDone(const FEwigfelsStageDef& Stage, const FEwigfelsQuestProgress& Progress) const;
	/** Ziele vom Typ condition gegen den Weltstatus prüfen. Rückgabe: ob sich etwas geändert hat. */
	bool UpdateConditionObjectives(const FEwigfelsQuestDef& Quest);
	void SetStatusFlag(FName QuestId, EEwigfelsQuestStatus Status);
	void CheckAutoStart();

	UFUNCTION()
	void HandleWorldStateChanged(EEwigfelsStateDomain Domain, FName Key, int32 OldValue, int32 NewValue);

	UPROPERTY()
	TMap<FName, FEwigfelsQuestDef> Definitions;

	UPROPERTY()
	FEwigfelsQuestSaveData SaveData;

	UPROPERTY()
	TObjectPtr<UEwigfelsWorldStateSubsystem> WorldState;

	/** Verhindert Endlosschleifen: Effekte ändern Weltstatus → Event → Quest schaltet weiter → … */
	int32 ReentryDepth = 0;
};
