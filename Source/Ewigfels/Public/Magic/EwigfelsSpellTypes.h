// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "EwigfelsSpellTypes.generated.h"

/**
 * Datentypen der Weisen (Zauber). Eine Weise = Auslieferung (wie sie ins Ziel kommt) + Liste von Effekten (was sie dort tut).
 * Damit sind hunderte Weisen reine Daten (Content/Data/Spells/*.json, Format: docs/SPELL_FORMAT.md).
 * Neue Auslieferungs- oder Effekt-Typen: Enum erweitern, Parser (EwigfelsSpellTypes.cpp) und Ausführung
 * (UEwigfelsSpellCasterComponent bzw. EwigfelsSpellEffects) ergänzen, Scripts/Tools/validate_combat.py nachziehen.
 */

UENUM(BlueprintType)
enum class EEwigfelsSpellDelivery : uint8
{
	/** Geschoss aus den Fingern (Funkenstoß, Glutweise, Frostweise). */
	Projectile,
	/** Sofortiger Kegel vor dem Wirker (Hammerton, Splitterchor). */
	Cone,
	/** Halten zum Blocken, Parierfenster am Anfang (Glockenschild). */
	Shield,
	/** Sofortiger Kreis um den Wirker (Verstummen, Klangwand-Stoß). */
	Area,
	/** Wirkt nur auf den Wirker selbst (Atemwende). */
	Self,
	/** Sofortiger Strahl aus den Fingern bis zum ersten Hindernis (Funkenstoß als Blitz). */
	Beam
};

UENUM(BlueprintType)
enum class EEwigfelsSpellEffectType : uint8
{
	Damage,
	Status,
	Knockback,
	Pull,
	RestoreAtem,
	Heal,
	/** Unterbricht den Angreifer (nur in on_parry sinnvoll). */
	StaggerAttacker,
	/** Hebt das Ziel Amount cm hoch und hält es Duration s in der Luft (Haltelied). */
	Lift,
	/** Schmettert das Ziel mit Amount cm/s zu Boden; beim Aufprall wirken on_land (Haltelied, zweiter Druck). */
	Slam
};

/** Bonus, wenn das Ziel einen Status trägt: "bonus": { "if_status": "frozen", "mult": 2.5, "consume": true } */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSpellBonus
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName IfStatus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Multiplier = 1.f;

	/** Status nach dem Bonus entfernen (z. B. Gefroren zerbricht). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bConsume = false;
};

USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSpellEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	EEwigfelsSpellEffectType Type = EEwigfelsSpellEffectType::Damage;

	/** Schaden, Kraft, Heilung oder Atem – je nach Typ. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Amount = 0.f;

	/** Grundton des Schadens (Glut, Firn, …) – für Resistenzen und Farb-Schilde. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Element;

	/** Nur für Type = Status. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Status;

	/** Status-Dauer in s. 0 = Standarddauer aus Status.json. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Duration = 0.f;

	/** Zusätzlicher Schwung nach oben (Knockback). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Up = 0.f;

	/** Nur beim letzten Glied einer Kette (Funkenstoß 3. Schuss). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bChainEndOnly = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FEwigfelsSpellBonus Bonus;
};

USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSpellDeliveryDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	EEwigfelsSpellDelivery Type = EEwigfelsSpellDelivery::Projectile;

	/** Geschoss: cm/s. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Speed = 3000.f;

	/** Geschoss: Kollisionsradius. Area/Shield: Wirkradius. In cm. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Radius = 15.f;

	/** Reichweite in cm (Geschoss-Flugweite, Kegellänge). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Range = 3000.f;

	/** Kegel: voller Öffnungswinkel in Grad. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Angle = 60.f;

	/** 0 = gerade, 1 = starke Zielsuche auf das Lock-On-Ziel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Homing = 0.f;

	/** Schwerkraft-Faktor des Geschosses (0 = keine). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Gravity = 0.f;

	/** Kettenglieder (Funkenstoß = 3). 1 = keine Kette. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	int32 Chain = 1;

	/** Zeit in s, in der der nächste Druck die Kette fortsetzt. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float ChainWindow = 0.6f;

	/** Wie viele Ziele das Geschoss durchschlägt, bevor es stoppt. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	int32 Pierce = 0;

	/** > 0: Geschoss explodiert beim Aufprall und trifft alle im Radius. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float ExplodeRadius = 0.f;

	/** Schild: Zeitfenster für Reinklang (Parade) ab dem Hochnehmen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float ParryWindow = 0.2f;

	/** Schild: Atem pro abgewehrtem Schadenspunkt. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float BlockAtemPerDamage = 0.5f;

	/** Schild: Anteil des Schadens, der trotz Block durchkommt. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float BlockDamageMultiplier = 0.2f;

	/** Strahl: so oft springt er vom Ziel zum nächsten Gegner weiter (Sturmweise = 2 → 3 Ziele). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	int32 Bounces = 0;

	/** Strahl: maximale Sprungweite in cm. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float BounceRange = 900.f;
};

USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSpellVisual
{
	GENERATED_BODY()

	/** Kernfarbe (Licht, Platzhalter-Kugel, Niagara-Parameter "User.Color"). Ohne vfx.color = 1. Grundton. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FLinearColor Color = FLinearColor(1.f, 0.9f, 0.6f);

	/** Saum/Glühen (Niagara "User.GlowColor"). Ohne vfx.glow_color = 2. Grundton. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FLinearColor GlowColor = FLinearColor(1.f, 0.9f, 0.6f);

	/** Funken/Splitter (Niagara "User.SparkColor"). Ohne vfx.spark_color = 3. Grundton. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FLinearColor SparkColor = FLinearColor(1.f, 0.9f, 0.6f);

	/** Strahl-Look: "bolt" (Zickzack-Blitz) oder "tether" (ruhiges, pulsierendes Klangband, z. B. Ziehklang). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName BeamStyle = TEXT("bolt");

	/** Fingergeste (flick, punch, stiff, clap, lift, slam, grab, rub, twitch). Montage-Rückfall: GestureMontagePath(). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Gesture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Scale = 1.f;

	/** "orb" (Kugel) oder "lance" (gestreckte Lanze) – nur für den Platzhalter-Look. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Shape = TEXT("orb");

	/** Niagara-System für Flug/Schild/Welle. Fehlt es, gibt es den Platzhalter-Look. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FSoftObjectPath Niagara;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FSoftObjectPath ImpactNiagara;

	/** Upper-Body-Montage (ANIMATION_BIBLE §3.2). Optional. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FSoftObjectPath Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FSoftObjectPath CastSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FSoftObjectPath ImpactSound;
};

/** Eine Weise. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSpellDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FText Description;

	/** Zauberwort (lateinisch klingend, selbst erfunden), z. B. "Scintivox". Wird beim Wirken eingeblendet. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FText Incantation;

	/** kern, alltag, koop (Zauber-Katalog, Content/Data/Spells/Katalog). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Kind;

	/** Id im Design-Katalog (z. B. K01), damit Kampfwerte und Katalog-Eintrag zusammenfinden. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName CatalogId;

	/** Schlag, Schutz, Elementar, Wandel, Ur. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName School;

	/** Drei Grundtöne (STORY_BIBLE §3.2). Gleicher Grundton wie die vorige Weise = längeres Combo-Fenster. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	TArray<FName> Chord;

	/** start, akt1, …, hidden (nur intern, z. B. Konter). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Unlock;

	/** Atem-Kosten. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Cost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Cooldown = 0.f;

	/** Zeit vom Druck bis zum Auslösen (Ausholen). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float CastTime = 0.f;

	/** Zeit nach dem Auslösen, bis die nächste Weise geht. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Recovery = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bTwoHanded = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FEwigfelsSpellDeliveryDef Delivery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	TArray<FEwigfelsSpellEffect> Effects;

	/** Nur Schild: Effekte bei gelungener Parade (Ziel = Wirker bzw. Angreifer). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	TArray<FEwigfelsSpellEffect> OnParry;

	/** Zweiter Druck innerhalb RecastWindow s: RecastEffects wirken auf die noch gehaltenen Ziele (Haltelied → Schmettern). 0 = aus. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float RecastWindow = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	TArray<FEwigfelsSpellEffect> RecastEffects;

	/** Wirkt, wenn ein von dieser Weise geschleudertes/geschmettertes Ziel aufschlägt (am Ziel und im LandRadius). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	TArray<FEwigfelsSpellEffect> OnLand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float LandRadius = 0.f;

	/** Nur Schild: Weise, die nach einer Parade automatisch auf den Angreifer geht. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName CounterSpell;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FEwigfelsSpellVisual Visual;

	/** Felder aus dem JSON, die der Code (noch) nicht kennt – z. B. learn, combo, koop, wirkart aus dem Zauber-Katalog. Wert als JSON-Text. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	TMap<FName, FString> Extra;

	bool IsValid() const { return !Id.IsNone(); }
};

/** Status-Effekt (Brennen, Gefroren, …), definiert in Status.json. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsStatusDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Id;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float Duration = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float DamagePerSecond = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float MoveSpeedMultiplier = 1.f;

	/** Wirkt auf Ausholen/Erholung von Weisen und Gegnerangriffen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float ActionSpeedMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float AtemRegenMultiplier = 1.f;

	/** Kein Zaubern, kein Angriff (Gefroren, Betäubt). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bStopsActions = false;

	/** Diese Status werden beim Auftragen entfernt (Feuer taut Eis). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	TArray<FName> Cancels;

	/** Ab so vielen Stapeln wird daraus EscalateTo (3× Unterkühlt = Gefroren). 0 = nie. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	int32 EscalateAt = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName EscalateTo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FLinearColor Color = FLinearColor::White;

	/** Look am Ziel: "ice" (Eisblock, Animation steht), "notes" (kreisende Noten), "flames", "rings" (Schweberinge), "tint". */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName Look;
};

namespace EwigfelsSpells
{
	/**
	 * Liest eine Zauberdatei ({"spells": [...], "statuses": [...], "loadout": [...]}, alle Teile optional).
	 * Fehler werden mit Dateiname und JSON-Pfad gesammelt. Gibt false zurück, wenn die Datei kein JSON ist.
	 */
	EWIGFELS_API bool ParseSpellFile(const FString& JsonText, const FString& SourceName,
		TArray<FEwigfelsSpellDef>& OutSpells, TArray<FEwigfelsStatusDef>& OutStatuses, TArray<FName>& OutLoadout,
		TArray<FString>& OutErrors);

	/** "#RRGGBB" → linear. Gibt false bei falschem Format. */
	EWIGFELS_API bool ParseHexColor(const FString& Hex, FLinearColor& Out);

	/** Grundton-Farbe (STORY_BIBLE §3.2), z. B. für HUD-Noten. Unbekannt = Weiß. */
	EWIGFELS_API FLinearColor ChordNoteColor(FName Note);

	/** Farbregel für alle Weisen: 1. Ton = Kern, 2. Ton = Glühen, 3. Ton = Funken. Fehlende Töne = vorheriger. */
	EWIGFELS_API void ChordPalette(const TArray<FName>& Chord, FLinearColor& OutCore, FLinearColor& OutGlow, FLinearColor& OutSpark);

	/** Rückfall-Montage einer Geste: /Game/Animations/Magic/Gesten/AM_Geste_<Gesture>. Leer bei None. */
	EWIGFELS_API FSoftObjectPath GestureMontagePath(FName Gesture);
}
