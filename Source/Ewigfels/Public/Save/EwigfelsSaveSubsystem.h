// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Containers/Ticker.h"
#include "Save/EwigfelsSaveGame.h"
#include "EwigfelsSaveSubsystem.generated.h"

class UEwigfelsSaveableComponent;
class UEwigfelsWorldStateSubsystem;
class UEwigfelsQuestSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEwigfelsSaveFinished, const FString&, SlotName, bool, bSuccess);

/**
 * Speichern/Laden: mehrere Slots, rotierende Autosaves, kompletter Weltstatus.
 * Inhalt eines Spielstands: Weltstatus (Flags, Beziehungen, Ruf), Quests, globale Dialog-Variablen,
 * Level + Spielerposition und alle Actors mit UEwigfelsSaveableComponent.
 *
 * Slots: "Slot_1" … "Slot_N" (manuell), "Auto_0" … "Auto_2" (Autosave, rotierend), "Quick" (Schnellspeichern).
 * Dateien landen unter Saved/SaveGames/ (beim Spieler: %LOCALAPPDATA%/Ewigfels/Saved/SaveGames/).
 *
 * Konsole (~): Ewigfels.Save Slot_1 · Ewigfels.Load Slot_1 · Ewigfels.Save.List · Ewigfels.Autosave
 */
UCLASS()
class EWIGFELS_API UEwigfelsSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Save")
	FEwigfelsSaveFinished OnSaveFinished;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Save")
	FEwigfelsSaveFinished OnLoadFinished;

	/** Speichert asynchron (kein Ruckler). Ergebnis kommt über OnSaveFinished. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	bool SaveToSlot(const FString& SlotName, const FString& DisplayName);

	/** Speichert in den nächsten Autosave-Platz, falls gerade erlaubt (kein Kampf, kein Dialog …). */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	bool Autosave();

	/** Lädt einen Spielstand. Wechselt bei Bedarf das Level; OnLoadFinished kommt, wenn alles wiederhergestellt ist. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	bool LoadFromSlot(const FString& SlotName);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	bool DeleteSlot(const FString& SlotName);

	/** Alle Spielstände fürs Menü, neueste zuerst. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Save")
	TArray<FEwigfelsSlotInfo> GetSlots() const;

	/** Neues Spiel: Weltstatus, Quests, Dialog-Variablen und Actor-Zustände leeren. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	void StartNewGame();

	/** Autosave alle X Sekunden (0 = aus). Standard: 300 s. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	void SetAutosaveInterval(float Seconds);

	/** Autosave sperren, z. B. Block("Kampf") … Unblock("Kampf"). Mehrere Gründe gleichzeitig möglich. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	void BlockAutosave(FName Reason) { AutosaveBlockers.Add(Reason); }

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	void UnblockAutosave(FName Reason) { AutosaveBlockers.Remove(Reason); }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Save")
	double GetPlayTimeSeconds() const;

	/** Name für den nächsten Spielstand, z. B. „Akademie · M104“. Kann ein Blueprint/GameMode überschreiben. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Save")
	FString MakeDefaultDisplayName() const;

	/** Aussehen aus dem zuletzt geladenen Spielstand – für eine Figur, die beim Laden noch nicht existierte. */
	bool GetLoadedPlayerAppearance(FEwigfelsAppearance& OutLook) const
	{
		OutLook = LoadedAppearance;
		return bHasLoadedAppearance;
	}

	// --- Für UEwigfelsSaveableComponent ---
	void RegisterSaveable(UEwigfelsSaveableComponent* Component);
	void UnregisterSaveable(UEwigfelsSaveableComponent* Component, bool bCaptureState);
	void MarkDestroyed(FName SaveId);

	// --- Für Tests ---
	/** Baut einen Spielstand aus dem aktuellen Zustand (ohne Datei). */
	UEwigfelsSaveGame* CreateSnapshot(const FString& SlotName, const FString& DisplayName, bool bAutosave);
	/** Spielt einen Spielstand ein (ohne Levelwechsel). */
	void ApplySnapshot(const UEwigfelsSaveGame* SaveGame);

	static constexpr int32 NumAutosaveSlots = 3;
	static const TCHAR* SlotIndexName;

private:
	bool WriteAsync(UEwigfelsSaveGame* SaveGame);
	void HandleAsyncSaveDone(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	void HandlePostLoadMap(UWorld* World);
	bool HandleAutosaveTick(float DeltaTime);
	void ApplyToLoadedWorld();
	void UpdateSlotIndex(const FEwigfelsSlotInfo& Info, bool bRemove);
	UEwigfelsSlotIndex* LoadOrCreateIndex() const;
	static bool Migrate(UEwigfelsSaveGame* SaveGame);
	static FString GetLevelPackageName(const UWorld* World);

	UPROPERTY()
	TObjectPtr<UEwigfelsWorldStateSubsystem> WorldState;

	UPROPERTY()
	TObjectPtr<UEwigfelsQuestSubsystem> Quests;

	/** Spielstand, der nach einem Levelwechsel noch eingespielt werden muss. */
	UPROPERTY()
	TObjectPtr<UEwigfelsSaveGame> PendingLoad;

	/** Zustände aller Saveable-Actors, auch der gerade entladenen. */
	TMap<FName, FEwigfelsActorRecord> Records;
	TMap<FName, TWeakObjectPtr<UEwigfelsSaveableComponent>> Registered;

	TSet<FName> AutosaveBlockers;
	FTSTicker::FDelegateHandle AutosaveHandle;
	float AutosaveInterval = 300.f;
	bool bSaveInFlight = false;

	FEwigfelsAppearance LoadedAppearance;
	bool bHasLoadedAppearance = false;

	double PlayTimeBeforeSession = 0.0;
	double SessionStartSeconds = 0.0;
};
