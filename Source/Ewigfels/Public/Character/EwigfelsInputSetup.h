// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EwigfelsInputSetup.generated.h"

class UInputAction;
class UInputMappingContext;

/**
 * Steuerung nach GDD §8, in C++ gebaut (keine Input-Assets nötig, sofort spielbar).
 * Später durch Assets ersetzbar: In der Figur DefaultMappingContext setzen, dann wird dieses hier nicht benutzt.
 * Controller und Maus/Tastatur sind gleichwertig:
 *   Zauberrad halten (RB / Q), mit rechtem Stick bzw. Maus wählen, loslassen = Weise gewählt.
 *   Gewählte Weise wirken (Y / F). Direkt: Steuerkreuz = Plätze 1–4, Tasten 1–6 = Plätze 1–6.
 *   Zauberbuch (B / Menü): aus allen gelernten Weisen die 6 fürs Rad wählen.
 */
UCLASS()
class EWIGFELS_API UEwigfelsInputSetup : public UObject
{
	GENERATED_BODY()

public:
	/** Baut alle Aktionen und den Erkundungs-/Kampf-Kontext. */
	static UEwigfelsInputSetup* Create(UObject* Outer);

	UPROPERTY() TObjectPtr<UInputMappingContext> Context;

	UPROPERTY() TObjectPtr<UInputAction> Move;
	UPROPERTY() TObjectPtr<UInputAction> Look;
	/** Rechter Stick getrennt von der Maus: Stick = Rate pro Sekunde, Maus = Pixel-Delta. */
	UPROPERTY() TObjectPtr<UInputAction> LookStick;
	UPROPERTY() TObjectPtr<UInputAction> Jump;
	UPROPERTY() TObjectPtr<UInputAction> Sprint;
	UPROPERTY() TObjectPtr<UInputAction> Dodge;
	UPROPERTY() TObjectPtr<UInputAction> CastBasic;
	UPROPERTY() TObjectPtr<UInputAction> Shield;
	UPROPERTY() TObjectPtr<UInputAction> Spell1;
	UPROPERTY() TObjectPtr<UInputAction> Spell2;
	UPROPERTY() TObjectPtr<UInputAction> Spell3;
	UPROPERTY() TObjectPtr<UInputAction> Spell4;
	UPROPERTY() TObjectPtr<UInputAction> Spell5;
	UPROPERTY() TObjectPtr<UInputAction> Spell6;
	UPROPERTY() TObjectPtr<UInputAction> Spell7;
	UPROPERTY() TObjectPtr<UInputAction> Spell8;
	/** Halten = Zauberrad offen. */
	UPROPERTY() TObjectPtr<UInputAction> SpellWheel;
	/** Im Rad gewählte Weise wirken. */
	UPROPERTY() TObjectPtr<UInputAction> CastSelected;
	/** Zauberbuch auf/zu (B / Menü-Taste): 6 Weisen fürs Zauberrad wählen. */
	UPROPERTY() TObjectPtr<UInputAction> Spellbook;
	UPROPERTY() TObjectPtr<UInputAction> LockOn;
	UPROPERTY() TObjectPtr<UInputAction> SwitchTarget;
	UPROPERTY() TObjectPtr<UInputAction> Interact;
	UPROPERTY() TObjectPtr<UInputAction> ToggleHelp;
	/** Ohne Bindung: schluckt Tasten, die im Game Animation Sample sonst Unsinn auslösen. */
	UPROPERTY() TObjectPtr<UInputAction> Swallow;

	/** Priorität unseres Kontexts: über IMC_Sandbox (0), unter Menüs wie dem Zauberbuch (100). */
	static constexpr int32 ContextPriority = 10;
};
