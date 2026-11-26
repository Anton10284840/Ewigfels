// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WorldState/EwigfelsWorldStateTypes.h"
#include "EwigfelsWorldStateSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEwigfelsWorldStateChanged,
	EEwigfelsStateDomain, Domain, FName, Key, int32, OldValue, int32, NewValue);

/**
 * Zentrale Stelle für Entscheidungen und Konsequenzen: globale Flags, Beziehungen zu Figuren, Ruf bei Fraktionen.
 * Lebt so lange wie das Spiel läuft (GameInstance), überlebt Levelwechsel. Gespeichert wird über UEwigfelsSaveSubsystem.
 *
 * Debug in der Konsole (~):
 *   Ewigfels.State.Dump
 *   Ewigfels.State.Apply rel:Lio+10
 *   Ewigfels.State.Check rep:Kollegium>=Geschaetzt
 */
UCLASS()
class EWIGFELS_API UEwigfelsWorldStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Feuert bei jeder echten Änderung (alter Wert != neuer Wert). UI, Quests und Dialoge hören hier zu. */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|WorldState")
	FEwigfelsWorldStateChanged OnStateChanged;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|WorldState")
	int32 GetValue(EEwigfelsStateDomain Domain, FName Key) const { return Data.GetValue(Domain, Key); }

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|WorldState")
	void SetValue(EEwigfelsStateDomain Domain, FName Key, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|WorldState")
	void AddValue(EEwigfelsStateDomain Domain, FName Key, int32 Delta);

	UFUNCTION(BlueprintPure, Category = "Ewigfels|WorldState")
	bool HasFlag(FName Flag) const { return Data.GetValue(EEwigfelsStateDomain::Flag, Flag) != 0; }

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|WorldState")
	void SetFlag(FName Flag, bool bSet = true) { SetValue(EEwigfelsStateDomain::Flag, Flag, bSet ? 1 : 0); }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|WorldState")
	EEwigfelsReputationTier GetReputationTier(FName Faction) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|WorldState")
	bool CheckCondition(const FEwigfelsCondition& Condition) const { return Data.Check(Condition); }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|WorldState")
	bool CheckAllConditions(const TArray<FEwigfelsCondition>& Conditions) const { return Data.CheckAll(Conditions); }

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|WorldState")
	void ApplyEffect(const FEwigfelsEffect& Effect);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|WorldState")
	void ApplyEffects(const TArray<FEwigfelsEffect>& Effects);

	/** Textform, z. B. "rel:Lio>=30". Ungültiger Text → false + Warnung im Log. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|WorldState")
	bool CheckConditionText(const FString& ConditionText) const;

	/** Textform, z. B. "rep:Kollegium+5". Gibt false zurück, wenn der Text ungültig war. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|WorldState")
	bool ApplyEffectText(const FString& EffectText);

	// --- Speichern/Laden (nur für UEwigfelsSaveSubsystem und Tests) ---
	const FEwigfelsWorldStateData& GetData() const { return Data; }

	/** Ersetzt den kompletten Zustand (Spielstand laden). Feuert OnStateReplaced, keine Einzel-Events. */
	void ReplaceData(const FEwigfelsWorldStateData& NewData);

	DECLARE_MULTICAST_DELEGATE(FOnStateReplaced);
	FOnStateReplaced OnStateReplaced;

	/** Mehrzeiliger Text aller Werte (Konsole, Debug-HUD). */
	FString DescribeState() const;

private:
	void Broadcast(EEwigfelsStateDomain Domain, FName Key, int32 Old, int32 New);

	UPROPERTY()
	FEwigfelsWorldStateData Data;
};
