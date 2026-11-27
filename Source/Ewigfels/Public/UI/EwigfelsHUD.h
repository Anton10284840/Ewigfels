// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Combat/EwigfelsCombatTypes.h"
#include "EwigfelsHUD.generated.h"

struct FEwigfelsHitEvent;
class AEwigfelsCharacter;
class UFont;

/**
 * Phase-3-HUD, direkt auf den Canvas gezeichnet (keine UMG-Assets nötig): Leben, Atem, Status, Weisen-Leiste mit
 * Akkord-Noten und Abklingzeit, Fadenkreuz, Lock-On-Marker, Gegner-Lebensbalken, Schadenszahlen, Hinweise,
 * Untertitel, Steuerungshilfe (F1). Das endgültige UI (CommonUI, GDD §9) kommt in Phase 5 und ersetzt diese Klasse.
 */
UCLASS()
class EWIGFELS_API AEwigfelsHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void DrawHUD() override;

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

	void DrawBars(AEwigfelsCharacter* Player);
	void DrawStatuses(AEwigfelsCharacter* Player, float X, float Y);
	void DrawSpellBar(AEwigfelsCharacter* Player);
	void DrawCrosshair(AEwigfelsCharacter* Player);
	void DrawEnemies(AEwigfelsCharacter* Player);
	void DrawFloatingNumbers();
	void DrawMessage(AEwigfelsCharacter* Player);
	void DrawInteractPrompt(AEwigfelsCharacter* Player);
	void DrawSubtitles(AEwigfelsCharacter* Player);
	void DrawHelp(AEwigfelsCharacter* Player);

	void Bar(float X, float Y, float W, float H, float Fraction, const FLinearColor& Fill, const FLinearColor& Back, float Ghost = -1.f);
	void Text(const FString& S, float X, float Y, const FLinearColor& Color, UFont* Font, float Scale = 1.f, bool bCenter = false);
	float S(float V) const { return V * UIScale; }

	TArray<FFloatingNumber> Numbers;
	FDelegateHandle HitHandle;
	float UIScale = 1.f;
	/** Nachlaufender Balken (zeigt kurz, wie viel gerade verloren ging). */
	float GhostHealth = 1.f;
};
