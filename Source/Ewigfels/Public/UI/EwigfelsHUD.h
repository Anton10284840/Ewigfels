// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Combat/EwigfelsCombatTypes.h"
#include "EwigfelsHUD.generated.h"

struct FEwigfelsHitEvent;
struct FEwigfelsSpellDef;
class AEwigfelsCharacter;
class UFont;

/**
 * Kampf-Anzeige, direkt auf den Canvas gezeichnet (keine UMG-Assets nötig). Einfach, bunt, rund, magisch:
 *   - unten links: Lebens-Amulett (Leben- und Atem-Ring, Status-Pillen)
 *   - unten rechts: gewählte Weise groß (leuchtet, Funken), daneben Basis-Weise und Schild
 *   - unten Mitte: Schnellplätze (Tastatur 1–6, Controller Steuerkreuz)
 *   - Zauberrad (RB / Q halten): 6 Weisen im Kreis, Stick oder Maus wählt
 *   - Zauberbuch (B / Menü): alle gelernten Weisen als große Karten, 6 davon fürs Rad wählen
 * Tasten-Symbole wechseln automatisch zwischen Controller und Maus/Tastatur (zuletzt benutztes Gerät).
 * Das endgültige UI (CommonUI, GDD §9) kann diese Klasse später ersetzen; Aufbau und Farben bleiben die Vorlage.
 */
UCLASS()
class EWIGFELS_API AEwigfelsHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DrawHUD() override;

	/** Aktionen, für die ein Tasten-Symbol gezeichnet wird. */
	enum class EAction : uint8
	{
		Move, Sprint, Jump, Dodge, Cast, Shield, Wheel, CastSelected, QuickSlots, LockOn, Switch, Interact, Help,
		Book, Confirm, Back, Browse
	};

private:
	struct FFloatingNumber
	{
		FVector Location;
		FString Text;
		FLinearColor Color;
		float Start = 0.f;
		float Scale = 1.f;
		FVector2D Drift;
	};

	void HandleHit(const FEwigfelsHitEvent& Event);

	// Bereiche
	void DrawVitals(AEwigfelsCharacter* Player);
	void DrawStatuses(AEwigfelsCharacter* Player, float X, float Y);
	void DrawSpellCluster(AEwigfelsCharacter* Player);
	void DrawQuickSlots(AEwigfelsCharacter* Player);
	void DrawWheel(AEwigfelsCharacter* Player);
	void DrawSpellbook(AEwigfelsCharacter* Player);
	/** Text in Zeilen umbrechen (höchstens MaxLines, sonst „…“). */
	TArray<FString> Wrap(const FString& Str, UFont* Font, float Scale, float MaxWidth, int32 MaxLines);
	void DrawCrosshair(AEwigfelsCharacter* Player);
	void DrawEnemies(AEwigfelsCharacter* Player);
	void DrawFloatingNumbers();
	void DrawMessage(AEwigfelsCharacter* Player);
	void DrawInteractPrompt(AEwigfelsCharacter* Player);
	void DrawSubtitles(AEwigfelsCharacter* Player);
	void DrawHelp(AEwigfelsCharacter* Player);

	/** Runde Weisen-Kugel: Farbverlauf, Abklingzeit als Tortenstück, optional Leuchten und Funken. */
	void SpellOrb(AEwigfelsCharacter* Player, const FEwigfelsSpellDef* Spell, const FVector2D& C, float R, bool bShine);

	// Grundformen
	/** Ring(-stück) von R0 bis R1, From/To als Anteil 0–1 im Uhrzeigersinn ab oben. R0 = 0 ergibt eine Scheibe. */
	void Ring(const FVector2D& C, float R0, float R1, float From, float To, const FLinearColor& Inner, const FLinearColor& Outer, bool bAdditive = false);
	void Disc(const FVector2D& C, float R, const FLinearColor& Color) { Ring(C, 0.f, R, 0.f, 1.f, Color, Color); }
	/** Weicher Lichthof (additiv) von R nach außen. */
	void Glow(const FVector2D& C, float R, float Width, const FLinearColor& Color, float Strength);
	/** Funkelnde Pünktchen, die um C kreisen. */
	void Sparkles(const FVector2D& C, float R, const FLinearColor& Color, int32 Count, float Seed);
	void RoundRect(float X, float Y, float W, float H, float Radius, const FLinearColor& Color);
	/** Tasten-Symbol mit linkem Rand X, mittig auf CY. Gibt die Breite zurück. */
	float Glyph(EAction Action, bool bGamepad, float X, float CY, float H);
	float GlyphWidth(EAction Action, bool bGamepad, float H);

	void Text(const FString& Str, float X, float Y, const FLinearColor& Color, UFont* Font, float Scale = 1.f, bool bCenter = false);
	/** Text mittig auf (CX, CY). */
	void TextC(const FString& Str, float CX, float CY, const FLinearColor& Color, UFont* Font, float Scale = 1.f);
	float TextW(const FString& Str, UFont* Font, float Scale = 1.f);
	float S(float V) const { return V * UIScale; }
	float Now() const;

	TArray<FFloatingNumber> Numbers;
	FDelegateHandle HitHandle;
	float UIScale = 1.f;
	/** Nachlaufender Ring (zeigt kurz, wie viel gerade verloren ging). */
	float GhostHealth = 1.f;
	/** Rad blendet weich ein (0–1). */
	float WheelBlend = 0.f;
	/** Hover im Rad wächst weich (pro Platz 0–1). */
	float HoverGrow[9] = {};
	/** Karussell im Zauberbuch gleitet weich zur gewählten Karte. */
	float BookScroll = 0.f;
	float BookBlend = 0.f;
};
