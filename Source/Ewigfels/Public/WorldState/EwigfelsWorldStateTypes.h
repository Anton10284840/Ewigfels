// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "EwigfelsWorldStateTypes.generated.h"

/**
 * Weltstatus = alles, was sich das Spiel über Entscheidungen merkt.
 * Drei Bereiche, alle als Ganzzahl unter einem Schlüssel (FName):
 *  - Flag:         globale Schalter und Zähler (0 = nicht gesetzt), z. B. E1_TrustEdda
 *  - Relationship: Beziehung zu einer Figur, −100 … +100, z. B. Lio
 *  - Reputation:   Ruf bei einer Fraktion, −100 … +100, z. B. Kollegium
 * Erlaubte Schlüssel stehen in Content/Data/World/WorldState.json (das Prüfskript vergleicht dagegen).
 */
UENUM(BlueprintType)
enum class EEwigfelsStateDomain : uint8
{
	Flag,
	Relationship,
	Reputation
};

UENUM(BlueprintType)
enum class EEwigfelsCompareOp : uint8
{
	Equal,
	NotEqual,
	Less,
	LessEqual,
	Greater,
	GreaterEqual
};

/** Ruf-Stufen laut GDD §7.1. Grenzen: siehe EwigfelsWorldState::TierFromValue. */
UENUM(BlueprintType)
enum class EEwigfelsReputationTier : uint8
{
	Feind,
	Misstrauisch,
	Neutral,
	Geschaetzt,
	Verbuendet
};

UENUM(BlueprintType)
enum class EEwigfelsEffectOp : uint8
{
	Set,
	Add
};

/**
 * Eine Bedingung, z. B. „Ruf Kollegium ≥ Geschätzt“.
 * Textform (Quest-JSON, Konsole): "flag:E1_TrustEdda", "!flag:E1_TrustEdda", "rel:Lio>=30", "rep:Kollegium>=Geschaetzt",
 * "quest:M101" (abgeschlossen), "!quest:M101", "quest:M101=active".
 */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	EEwigfelsStateDomain Domain = EEwigfelsStateDomain::Flag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	EEwigfelsCompareOp Op = EEwigfelsCompareOp::NotEqual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	int32 Value = 0;

	/** Für Logs und Debug-Anzeige, gleiche Syntax wie beim Parsen. */
	FString ToString() const;
};

/**
 * Eine Änderung am Weltstatus, z. B. „Beziehung Lio +10“.
 * Textform: "flag:E1_TrustEdda=1", "flag:Kerzen+1", "rel:Lio+10", "rep:Kollegium-15".
 */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	EEwigfelsStateDomain Domain = EEwigfelsStateDomain::Flag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	EEwigfelsEffectOp Op = EEwigfelsEffectOp::Set;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|WorldState")
	int32 Value = 1;

	FString ToString() const;
};

/** Der komplette Weltstatus. Wird 1:1 im Spielstand gespeichert. Reine Daten + Logik, ohne Engine-Welt (testbar). */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsWorldStateData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|WorldState")
	TMap<FName, int32> Flags;

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|WorldState")
	TMap<FName, int32> Relationships;

	UPROPERTY(SaveGame, VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|WorldState")
	TMap<FName, int32> Reputation;

	int32 GetValue(EEwigfelsStateDomain Domain, FName Key) const;

	/** Setzt den Wert (Beziehung/Ruf werden auf −100 … +100 begrenzt). Gibt den alten Wert zurück. */
	int32 SetValue(EEwigfelsStateDomain Domain, FName Key, int32 NewValue);

	bool Check(const FEwigfelsCondition& Condition) const;
	bool CheckAll(const TArray<FEwigfelsCondition>& Conditions) const;

	/** Wendet den Effekt an. Gibt über OutOld/OutNew die Werte zurück (für Events). */
	void Apply(const FEwigfelsEffect& Effect, int32& OutOld, int32& OutNew);

	void Reset();

private:
	TMap<FName, int32>& MapFor(EEwigfelsStateDomain Domain);
	const TMap<FName, int32>& MapFor(EEwigfelsStateDomain Domain) const;
};

/** Hilfsfunktionen ohne Zustand: Parsen, Grenzen, Ruf-Stufen. */
namespace EwigfelsWorldState
{
	constexpr int32 MinValue = -100;
	constexpr int32 MaxValue = 100;

	/** Ruf → Stufe: ≤ −60 Feind, ≤ −20 Misstrauisch, < 20 Neutral, < 60 Geschätzt, sonst Verbündet. */
	EWIGFELS_API EEwigfelsReputationTier TierFromValue(int32 Value);

	/** Untergrenze einer Stufe (für "rep:Kollegium>=Geschaetzt"). */
	EWIGFELS_API int32 TierMinValue(EEwigfelsReputationTier Tier);

	EWIGFELS_API bool ParseCondition(const FString& Text, FEwigfelsCondition& Out, FString& OutError);
	EWIGFELS_API bool ParseEffect(const FString& Text, FEwigfelsEffect& Out, FString& OutError);

	EWIGFELS_API bool Compare(int32 Lhs, EEwigfelsCompareOp Op, int32 Rhs);
}
