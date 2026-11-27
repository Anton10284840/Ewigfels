// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Magic/EwigfelsSpellTypes.h"

class UEwigfelsCombatComponent;

/** Ein Treffer einer Weise auf ein Ziel. */
struct EWIGFELS_API FEwigfelsSpellHit
{
	AActor* Caster = nullptr;
	AActor* Target = nullptr;
	FVector Location = FVector::ZeroVector;
	/** Flugrichtung bzw. Richtung Wirker → Ziel (für Stoß und Zug). */
	FVector Direction = FVector::ForwardVector;
	/** Letztes Glied einer Kette (Funkenstoß 3. Schuss). */
	bool bChainEnd = true;
};

/**
 * Wendet die Effekte einer Weise an. Alles hier läuft NUR auf dem Server (Koop-fähig);
 * die reinen Rechenfunktionen (ComputeDamage, IsInCone) sind frei von Welt-Zustand und werden getestet.
 */
namespace EwigfelsSpellEffects
{
	/** Wendet Effects der Reihe nach auf Hit.Target an. StaggerAttacker wirkt auf Hit.Caster's Gegenüber (Attacker). */
	EWIGFELS_API void ApplyEffects(const FEwigfelsSpellDef& Spell, const TArray<FEwigfelsSpellEffect>& Effects,
		const FEwigfelsSpellHit& Hit, AActor* Attacker = nullptr);

	/**
	 * Schaden eines Effekts gegen ein Ziel inkl. Bonus (z. B. ×2,5 gegen Gefroren).
	 * bOutConsume = Bonus-Status soll danach entfernt werden.
	 */
	EWIGFELS_API float ComputeDamage(const FEwigfelsSpellEffect& Effect, const UEwigfelsCombatComponent* Target, bool& bOutConsume);

	/** Liegt Point im Kegel (Origin, Forward, Range, voller Winkel in Grad)? Höhe zählt nur halb (flacher Kegel). */
	EWIGFELS_API bool IsInCone(const FVector& Origin, const FVector& Forward, float Range, float AngleDeg, const FVector& Point);

	/** Feindliche, lebende Ziele mit Kampfkomponente im Kegel bzw. Kreis, mit Sichtlinie. */
	EWIGFELS_API TArray<AActor*> GatherCone(AActor* Caster, const FVector& Origin, const FVector& Forward, float Range, float AngleDeg);
	EWIGFELS_API TArray<AActor*> GatherSphere(AActor* Caster, const FVector& Origin, float Radius, const AActor* Ignore = nullptr);
}
