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
 * Gamepad: Weisen 1–4 vorerst auf dem Steuerkreuz (RB + Taste kommt mit dem Optionsmenü).
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
	UPROPERTY() TObjectPtr<UInputAction> LockOn;
	UPROPERTY() TObjectPtr<UInputAction> SwitchTarget;
	UPROPERTY() TObjectPtr<UInputAction> Interact;
	UPROPERTY() TObjectPtr<UInputAction> ToggleHelp;
};
