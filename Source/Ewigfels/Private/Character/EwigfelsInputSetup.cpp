// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Character/EwigfelsInputSetup.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputCoreTypes.h"

namespace
{
	UInputAction* MakeAction(UObject* Outer, const TCHAR* Name, EInputActionValueType Type)
	{
		UInputAction* Action = NewObject<UInputAction>(Outer, Name);
		Action->ValueType = Type;
		return Action;
	}

	/** W/S liegen auf der Y-Achse der 2D-Bewegung: Achsen tauschen (+ ggf. umdrehen). */
	void MapWasd(UInputMappingContext* Ctx, UInputAction* Move)
	{
		UObject* Outer = Ctx;
		FEnhancedActionKeyMapping& W = Ctx->MapKey(Move, EKeys::W);
		W.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(Outer));

		FEnhancedActionKeyMapping& S = Ctx->MapKey(Move, EKeys::S);
		S.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(Outer));
		S.Modifiers.Add(NewObject<UInputModifierNegate>(Outer));

		FEnhancedActionKeyMapping& A = Ctx->MapKey(Move, EKeys::A);
		A.Modifiers.Add(NewObject<UInputModifierNegate>(Outer));

		Ctx->MapKey(Move, EKeys::D);
	}

	void MapStick(UInputMappingContext* Ctx, UInputAction* Action, const FKey& Key, float DeadZone)
	{
		FEnhancedActionKeyMapping& M = Ctx->MapKey(Action, Key);
		UInputModifierDeadZone* Dz = NewObject<UInputModifierDeadZone>(Ctx);
		Dz->LowerThreshold = DeadZone;
		M.Modifiers.Add(Dz);
	}
}

UEwigfelsInputSetup* UEwigfelsInputSetup::Create(UObject* Outer)
{
	UEwigfelsInputSetup* S = NewObject<UEwigfelsInputSetup>(Outer);
	S->Context = NewObject<UInputMappingContext>(S, TEXT("IMC_Ewigfels_Erkundung"));
	UInputMappingContext* C = S->Context;

	S->Move         = MakeAction(S, TEXT("IA_Move"), EInputActionValueType::Axis2D);
	S->Look         = MakeAction(S, TEXT("IA_Look"), EInputActionValueType::Axis2D);
	S->LookStick    = MakeAction(S, TEXT("IA_LookStick"), EInputActionValueType::Axis2D);
	S->Jump         = MakeAction(S, TEXT("IA_Jump"), EInputActionValueType::Boolean);
	S->Sprint       = MakeAction(S, TEXT("IA_Sprint"), EInputActionValueType::Boolean);
	S->Dodge        = MakeAction(S, TEXT("IA_Dodge"), EInputActionValueType::Boolean);
	S->CastBasic    = MakeAction(S, TEXT("IA_CastBasic"), EInputActionValueType::Boolean);
	S->Shield       = MakeAction(S, TEXT("IA_Shield"), EInputActionValueType::Boolean);
	S->Spell1       = MakeAction(S, TEXT("IA_Spell1"), EInputActionValueType::Boolean);
	S->Spell2       = MakeAction(S, TEXT("IA_Spell2"), EInputActionValueType::Boolean);
	S->Spell3       = MakeAction(S, TEXT("IA_Spell3"), EInputActionValueType::Boolean);
	S->Spell4       = MakeAction(S, TEXT("IA_Spell4"), EInputActionValueType::Boolean);
	S->Spell5       = MakeAction(S, TEXT("IA_Spell5"), EInputActionValueType::Boolean);
	S->Spell6       = MakeAction(S, TEXT("IA_Spell6"), EInputActionValueType::Boolean);
	S->Spell7       = MakeAction(S, TEXT("IA_Spell7"), EInputActionValueType::Boolean);
	S->Spell8       = MakeAction(S, TEXT("IA_Spell8"), EInputActionValueType::Boolean);
	S->LockOn       = MakeAction(S, TEXT("IA_LockOn"), EInputActionValueType::Boolean);
	S->SwitchTarget = MakeAction(S, TEXT("IA_SwitchTarget"), EInputActionValueType::Axis1D);
	S->Interact     = MakeAction(S, TEXT("IA_Interact"), EInputActionValueType::Boolean);
	S->ToggleHelp   = MakeAction(S, TEXT("IA_ToggleHelp"), EInputActionValueType::Boolean);

	// Bewegen / Kamera
	MapWasd(C, S->Move);
	MapStick(C, S->Move, EKeys::Gamepad_Left2D, 0.2f);
	C->MapKey(S->Look, EKeys::Mouse2D);
	MapStick(C, S->LookStick, EKeys::Gamepad_Right2D, 0.15f);

	// Springen, Sprinten, Ausweichen
	C->MapKey(S->Jump, EKeys::SpaceBar);
	C->MapKey(S->Jump, EKeys::Gamepad_FaceButton_Bottom);
	C->MapKey(S->Sprint, EKeys::LeftShift);
	C->MapKey(S->Sprint, EKeys::Gamepad_LeftThumbstick);
	C->MapKey(S->Dodge, EKeys::LeftAlt);
	C->MapKey(S->Dodge, EKeys::C); // Alt kollidiert gern mit Windows/Editor, C als Zweitbelegung
	C->MapKey(S->Dodge, EKeys::Gamepad_FaceButton_Right);

	// Weisen
	C->MapKey(S->CastBasic, EKeys::LeftMouseButton);
	C->MapKey(S->CastBasic, EKeys::Gamepad_RightTrigger);
	C->MapKey(S->Shield, EKeys::RightMouseButton);
	C->MapKey(S->Shield, EKeys::Gamepad_LeftTrigger);
	C->MapKey(S->Spell1, EKeys::One);
	C->MapKey(S->Spell2, EKeys::Two);
	C->MapKey(S->Spell3, EKeys::Three);
	C->MapKey(S->Spell4, EKeys::Four);
	// 5–8 nur Tastatur; am Gamepad kommt später das Zauberrad
	C->MapKey(S->Spell5, EKeys::Five);
	C->MapKey(S->Spell6, EKeys::Six);
	C->MapKey(S->Spell7, EKeys::Seven);
	C->MapKey(S->Spell8, EKeys::Eight);
	C->MapKey(S->Spell1, EKeys::Gamepad_DPad_Up);
	C->MapKey(S->Spell2, EKeys::Gamepad_DPad_Right);
	C->MapKey(S->Spell3, EKeys::Gamepad_DPad_Down);
	C->MapKey(S->Spell4, EKeys::Gamepad_DPad_Left);

	// Ziel, Interaktion
	C->MapKey(S->LockOn, EKeys::MiddleMouseButton);
	C->MapKey(S->LockOn, EKeys::Tab);
	C->MapKey(S->LockOn, EKeys::Gamepad_RightThumbstick);
	C->MapKey(S->SwitchTarget, EKeys::MouseWheelAxis);
	FEnhancedActionKeyMapping& SwitchLeft = C->MapKey(S->SwitchTarget, EKeys::Gamepad_LeftShoulder);
	SwitchLeft.Modifiers.Add(NewObject<UInputModifierNegate>(C));
	C->MapKey(S->SwitchTarget, EKeys::Gamepad_RightShoulder);
	C->MapKey(S->Interact, EKeys::E);
	C->MapKey(S->Interact, EKeys::Gamepad_FaceButton_Left);
	C->MapKey(S->ToggleHelp, EKeys::F1);
	C->MapKey(S->ToggleHelp, EKeys::Gamepad_Special_Left);
	return S;
}
