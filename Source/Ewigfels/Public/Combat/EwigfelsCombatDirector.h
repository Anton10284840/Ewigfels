// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Combat/EwigfelsCombatTypes.h"
#include "EwigfelsCombatDirector.generated.h"

/** Treffer-Ereignis für HUD (Schadenszahlen). Lokal auf jeder Maschine. */
struct FEwigfelsHitEvent
{
	TWeakObjectPtr<AActor> Victim;
	TWeakObjectPtr<AActor> Instigator;
	float Amount = 0.f;
	FVector Location = FVector::ZeroVector;
	EEwigfelsHitResult Result = EEwigfelsHitResult::Hit;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEwigfelsHitEventSignature, const FEwigfelsHitEvent&);

/**
 * Kampf-Koordinator pro Welt (GDD §5): verteilt Angriffs-Plätze, damit nie mehr als MaxMeleeAttackers gleichzeitig
 * auf denselben Spieler einschlagen (fair, lesbar; im Koop pro Spieler getrennt). Nur Server.
 * Außerdem Sammelstelle für Treffer-Ereignisse (HUD, lokal).
 */
UCLASS()
class EWIGFELS_API UEwigfelsCombatDirector : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Fragt einen Nahkampf-Platz gegen Target an. true = darf angreifen. */
	bool RequestMeleeToken(AActor* Attacker, AActor* Target);

	void ReleaseMeleeToken(AActor* Attacker);

	bool HasMeleeToken(const AActor* Attacker) const;

	/** Anzahl Nahkämpfer, die gerade Target angreifen (für Tests). */
	int32 CountAttackers(const AActor* Target) const;

	int32 MaxMeleeAttackers = 2;

	/** Treffer melden/abonnieren (HUD). */
	void BroadcastHit(const FEwigfelsHitEvent& Event) { OnHit.Broadcast(Event); }
	FEwigfelsHitEventSignature OnHit;

private:
	void Prune();

	/** Angreifer → Ziel */
	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<AActor>> Tokens;
};
