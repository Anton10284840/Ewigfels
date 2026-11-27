// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Character/EwigfelsCharacter.h"
#include "Appearance/EwigfelsAppearanceComponent.h"
#include "Character/EwigfelsCharacterMovementComponent.h"
#include "Character/EwigfelsInputSetup.h"
#include "Character/EwigfelsLockOnComponent.h"
#include "Character/EwigfelsPlaceholderBody.h"
#include "Camera/EwigfelsDialogueCameraComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Quest/EwigfelsQuestSubsystem.h"
#include "UI/EwigfelsSpellbookComponent.h"
#include "World/EwigfelsInteractable.h"
#include "Ewigfels.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/OverlapResult.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/RootMotionSource.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "EwigfelsCharacter"

namespace
{
	const FName DodgeSourceName(TEXT("EwigfelsDodge"));
	const FName StepMove(TEXT("move"));
	const FName StepJump(TEXT("jump"));
	const FName StepDodge(TEXT("dodge"));
	const FName TutorialMovement(TEXT("Tutorial_Movement"));
}

AEwigfelsCharacter::AEwigfelsCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UEwigfelsCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(38.f, 90.f);
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Mannequin/MetaHuman-Standard: Füße am Kapselboden, Blick entlang +X
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones; // Fingerspitzen auch auf dem Server

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = DefaultArmLength;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 55.f); // über die rechte Schulter
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 14.f;
	CameraBoom->ProbeSize = 14.f;
	DefaultSocketOffset = CameraBoom->SocketOffset;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetFieldOfView(DefaultFov);

	Combat = CreateDefaultSubobject<UEwigfelsCombatComponent>(TEXT("Combat"));
	Combat->SetTeam(EEwigfelsTeam::Player);
	Combat->DefaultMaxHealth = 120.f;
	Combat->StaggerThreshold = 25.f;

	Caster = CreateDefaultSubobject<UEwigfelsSpellCasterComponent>(TEXT("Caster"));
	LockOn = CreateDefaultSubobject<UEwigfelsLockOnComponent>(TEXT("LockOn"));
	DialogueCamera = CreateDefaultSubobject<UEwigfelsDialogueCameraComponent>(TEXT("DialogueCamera"));
	Appearance = CreateDefaultSubobject<UEwigfelsAppearanceComponent>(TEXT("Appearance"));
	Spellbook = CreateDefaultSubobject<UEwigfelsSpellbookComponent>(TEXT("Spellbook"));

	PlaceholderBody = CreateDefaultSubobject<UEwigfelsPlaceholderBody>(TEXT("PlaceholderBody"));
	PlaceholderBody->SetupAttachment(RootComponent);
	PlaceholderBody->SetLook(FLinearColor(0.035f, 0.05f, 0.11f), FLinearColor(1.f, 0.78f, 0.42f), FLinearColor(0.75f, 0.9f, 1.f), 1.f, false);
}

void AEwigfelsCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AEwigfelsCharacter, bLockedOnReplicated, COND_SkipOwner);
}

UEwigfelsCharacterMovementComponent* AEwigfelsCharacter::GetEwigfelsMovement() const
{
	return Cast<UEwigfelsCharacterMovementComponent>(GetCharacterMovement());
}

void AEwigfelsCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ApplyDefaultLook();
}

void AEwigfelsCharacter::ApplyDefaultLook()
{
	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (!SkelMesh || SkelMesh->GetSkeletalMeshAsset() || DefaultMesh.IsNull())
	{
		return;
	}
	if (USkeletalMesh* Asset = Cast<USkeletalMesh>(EwigfelsSpellVisuals::LoadIfExists(DefaultMesh)))
	{
		SkelMesh->SetSkeletalMesh(Asset);
		if (UClass* AnimClass = DefaultAnimClass.TryLoadClass<UAnimInstance>())
		{
			SkelMesh->SetAnimInstanceClass(AnimClass);
		}
	}
	else
	{
		UE_LOG(LogEwigfels, Log, TEXT("Figur: %s nicht gefunden, nutze Platzhalter-Koerper"), *DefaultMesh.ToString());
	}
}

void AEwigfelsCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultArmLength = CameraBoom->TargetArmLength;
	DefaultFov = FollowCamera->FieldOfView;
	DetectExternalCamera();

	Caster->OnCastStarted.AddDynamic(this, &AEwigfelsCharacter::HandleCastStarted);
	Caster->OnSpellReleased.AddDynamic(this, &AEwigfelsCharacter::HandleSpellReleased);
	Caster->OnCastFailed.AddDynamic(this, &AEwigfelsCharacter::HandleCastFailed);
	Caster->OnShieldChanged.AddDynamic(this, &AEwigfelsCharacter::HandleShieldChanged);
	Caster->OnParried.AddDynamic(this, &AEwigfelsCharacter::HandleParried);
	Caster->OnBeamFired.AddDynamic(this, &AEwigfelsCharacter::HandleBeamFired);
	Caster->OnImpactFx.AddDynamic(this, &AEwigfelsCharacter::HandleImpactFx);
	Combat->OnHitFeedback.AddDynamic(this, &AEwigfelsCharacter::HandleHitFeedback);
	Combat->OnDied.AddDynamic(this, &AEwigfelsCharacter::HandleDied);
	Combat->OnStaggered.AddDynamic(this, &AEwigfelsCharacter::HandleStaggered);
	LockOn->OnTargetChanged.AddDynamic(this, &AEwigfelsCharacter::HandleLockOnChanged);

	TWeakObjectPtr<AEwigfelsCharacter> WeakThis(this);
	Caster->SetAimPointProvider([WeakThis]()
	{
		return WeakThis.IsValid() ? WeakThis->ComputeAimPoint() : FVector::ZeroVector;
	});
}

// ---------------------------------------------------------------------------
// Eingabe

void AEwigfelsCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->IsLocalController())
	{
		return;
	}
	if (!InputSetup)
	{
		InputSetup = UEwigfelsInputSetup::Create(this);
	}
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext ? DefaultMappingContext.Get() : InputSetup->Context.Get(), 0);
	}
}

void AEwigfelsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input)
	{
		UE_LOG(LogEwigfels, Error, TEXT("Figur: Enhanced Input fehlt (DefaultInput.ini)"));
		return;
	}
	if (!InputSetup)
	{
		InputSetup = UEwigfelsInputSetup::Create(this);
	}
	const UEwigfelsInputSetup* S = InputSetup;
	if (bUseBuiltInMovementInput)
	{
		Input->BindAction(S->Move, ETriggerEvent::Triggered, this, &AEwigfelsCharacter::InputMove);
		Input->BindAction(S->Look, ETriggerEvent::Triggered, this, &AEwigfelsCharacter::InputLook);
		Input->BindAction(S->LookStick, ETriggerEvent::Triggered, this, &AEwigfelsCharacter::InputLookStick);
		Input->BindAction(S->Jump, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputJump);
		Input->BindAction(S->Jump, ETriggerEvent::Completed, this, &AEwigfelsCharacter::InputJumpReleased);
		Input->BindAction(S->Sprint, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSprint);
		Input->BindAction(S->Sprint, ETriggerEvent::Completed, this, &AEwigfelsCharacter::InputSprint);
	}
	Input->BindAction(S->Dodge, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputDodge);
	Input->BindAction(S->CastBasic, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputCastBasic);
	Input->BindAction(S->Shield, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputShieldStart);
	Input->BindAction(S->Shield, ETriggerEvent::Completed, this, &AEwigfelsCharacter::InputShieldEnd);
	Input->BindAction(S->Spell1, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell1);
	Input->BindAction(S->Spell2, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell2);
	Input->BindAction(S->Spell3, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell3);
	Input->BindAction(S->Spell4, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell4);
	Input->BindAction(S->Spell5, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell5);
	Input->BindAction(S->Spell6, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell6);
	Input->BindAction(S->Spell7, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell7);
	Input->BindAction(S->Spell8, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpell8);
	Input->BindAction(S->LockOn, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputLockOn);
	Input->BindAction(S->SwitchTarget, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSwitchTarget);
	Input->BindAction(S->Interact, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputInteract);
	Input->BindAction(S->ToggleHelp, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputToggleHelp);
	Input->BindAction(S->SpellWheel, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputWheelOpen);
	Input->BindAction(S->SpellWheel, ETriggerEvent::Completed, this, &AEwigfelsCharacter::InputWheelClose);
	Input->BindAction(S->CastSelected, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputCastSelected);
	Input->BindAction(S->Spellbook, ETriggerEvent::Started, this, &AEwigfelsCharacter::InputSpellbook);
	Spellbook->BindInput(Input);
	// Rad-Zeiger und Geräte-Erkennung immer, auch wenn ein Blueprint Bewegung/Kamera selbst steuert
	Input->BindAction(S->Look, ETriggerEvent::Triggered, this, &AEwigfelsCharacter::InputWheelPointMouse);
	Input->BindAction(S->LookStick, ETriggerEvent::Triggered, this, &AEwigfelsCharacter::InputWheelPointStick);
	// Jede Taste meldet das Gerät (Controller oder Maus/Tastatur), ohne sie zu verschlucken
	// Enhanced Input sperrt BindKey (= delete), die Basisklasse UInputComponent kann es noch
	FInputKeyBinding& AnyKey = static_cast<UInputComponent*>(Input)->BindKey(EKeys::AnyKey, IE_Pressed, this, &AEwigfelsCharacter::InputAnyKey);
	AnyKey.bConsumeInput = false;
}

void AEwigfelsCharacter::SetGameplayInputBlocked(bool bBlocked)
{
	bInputBlocked = bBlocked;
	if (bBlocked)
	{
		if (UEwigfelsCharacterMovementComponent* Move = GetEwigfelsMovement())
		{
			Move->SetSprinting(false);
		}
		Caster->RequestShield(false);
		LockOn->Release();
	}
}

void AEwigfelsCharacter::InputMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (bInputBlocked || !Controller || Combat->IsDead())
	{
		return;
	}
	const FRotator Yaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::X), Axis.Y);
	AddMovementInput(FRotationMatrix(Yaw).GetUnitAxis(EAxis::Y), Axis.X);
	if (Axis.SizeSquared() > 0.25f && GetVelocity().SizeSquared2D() > FMath::Square(200.f))
	{
		NoteTutorialStep(StepMove);
	}
}

void AEwigfelsCharacter::InputLook(const FInputActionValue& Value)
{
	if (bInputBlocked || bWheelOpen || LockOn->GetTarget() || DialogueCamera->IsDialogueActive())
	{
		return;
	}
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X * MouseSensitivity);
	AddControllerPitchInput(-Axis.Y * MouseSensitivity);
}

void AEwigfelsCharacter::InputLookStick(const FInputActionValue& Value)
{
	if (bInputBlocked || bWheelOpen || LockOn->GetTarget() || DialogueCamera->IsDialogueActive())
	{
		return;
	}
	const FVector2D Axis = Value.Get<FVector2D>();
	const float Dt = GetWorld()->GetDeltaSeconds();
	AddControllerYawInput(Axis.X * GamepadLookRate * Dt);
	AddControllerPitchInput(-Axis.Y * GamepadLookRate * 0.7f * Dt);
}

void AEwigfelsCharacter::InputJump()
{
	if (bInputBlocked || !Combat->CanAct())
	{
		return;
	}
	Jump();
	NoteTutorialStep(StepJump);
}

void AEwigfelsCharacter::InputJumpReleased()
{
	StopJumping();
}

void AEwigfelsCharacter::InputSprint(const FInputActionValue& Value)
{
	if (UEwigfelsCharacterMovementComponent* Move = GetEwigfelsMovement())
	{
		Move->SetSprinting(Value.Get<bool>() && !bInputBlocked);
	}
}

void AEwigfelsCharacter::InputDodge()
{
	if (bInputBlocked || !Combat->CanAct() || !GetCharacterMovement()->IsMovingOnGround() || IsDodging())
	{
		return;
	}
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastDodgeTime < DodgeCooldown)
	{
		return;
	}
	if (Caster->GetAtem() < DodgeAtemCost)
	{
		ShowMessage(LOCTEXT("NoAtemDodge", "Kein Atem zum Ausweichen"), FLinearColor(0.45f, 0.7f, 1.f));
		return;
	}
	FVector Dir = GetLastMovementInputVector().GetSafeNormal2D();
	if (Dir.IsNearlyZero())
	{
		Dir = -GetActorForwardVector(); // ohne Richtung: Schritt zurück
	}
	LastDodgeTime = Now;
	if (!HasAuthority())
	{
		StartDodge(Dir); // Vorhersage
	}
	Server_Dodge(Dir);
	NoteTutorialStep(StepDodge);
}

void AEwigfelsCharacter::Server_Dodge_Implementation(FVector_NetQuantizeNormal Direction)
{
	if (!Combat->CanAct() || IsDodging() || !Caster->TrySpendAtem(DodgeAtemCost))
	{
		return; // der Client korrigiert sich über die normale Bewegungskorrektur
	}
	Caster->InterruptCast();
	Caster->RequestShield(false);
	Combat->SetInvulnerableFor(DodgeInvulnerability);
	StartDodge(FVector(Direction).GetSafeNormal2D());
}

void AEwigfelsCharacter::StartDodge(const FVector& Direction)
{
	TSharedPtr<FRootMotionSource_ConstantForce> Force = MakeShared<FRootMotionSource_ConstantForce>();
	Force->InstanceName = DodgeSourceName;
	Force->AccumulateMode = ERootMotionAccumulateMode::Override;
	Force->Priority = 5;
	Force->Force = Direction * (DodgeDistance / DodgeDuration);
	Force->Duration = DodgeDuration;
	Force->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::ClampVelocity;
	Force->FinishVelocityParams.ClampVelocity = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->ApplyRootMotionSource(Force);

	// Kosmetik: kurzer Lichtschweif am Boden
	if (PlaceholderBody->IsActiveBody() || GetNetMode() != NM_DedicatedServer)
	{
		EwigfelsSpellVisuals::SpawnFlash(GetWorld(), TEXT("sphere"), GetActorLocation() - FVector(0.f, 0.f, 70.f), Direction.Rotation(),
			FLinearColor(0.6f, 0.75f, 1.f), FVector(0.3f, 0.3f, 0.05f), FVector(2.5f, 0.6f, 0.05f), 0.3f, 1500.f, 300.f);
	}
}

bool AEwigfelsCharacter::IsDodging() const
{
	const UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!Move)
	{
		return false;
	}
	// GetRootMotionSource ist nicht const, darum selbst suchen
	for (const TSharedPtr<FRootMotionSource>& Source : Move->CurrentRootMotion.RootMotionSources)
	{
		if (Source.IsValid() && Source->InstanceName == DodgeSourceName)
		{
			return true;
		}
	}
	return false;
}

void AEwigfelsCharacter::InputCastBasic()
{
	CastSlot(0);
}

void AEwigfelsCharacter::CastSlot(int32 Slot)
{
	if (bInputBlocked || Combat->IsDead())
	{
		return;
	}
	// Ziel: Lock-On, sonst weiche Zielhilfe (kleiner Kegel um die Bildmitte)
	AActor* Target = LockOn->GetTarget();
	if (!Target)
	{
		Target = LockOn->FindSoftTarget(7.f);
	}
	Caster->SetAimTarget(Target);
	Caster->RequestCastSlot(Slot);
	if (Slot >= 1 && Slot <= UEwigfelsSpellCasterComponent::WheelSlotCount && !Caster->GetSlotSpell(Slot).IsNone())
	{
		SelectedSlot = Slot; // zuletzt direkt gewirkte Weise wird die gewählte (Y / F)
	}
}

// --- Zauberrad -----------------------------------------------------------

void AEwigfelsCharacter::InputWheelOpen()
{
	if (bInputBlocked || bWheelOpen || Combat->IsDead())
	{
		return;
	}
	bWheelOpen = true;
	WheelHover = 0;
	WheelMouse = FVector2D::ZeroVector;
	WheelPointer = FVector2D::ZeroVector;
	// Kamera steht, solange das Rad offen ist (wirkt auch auf die Kamera des Blueprints)
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->SetIgnoreLookInput(true);
	}
}

void AEwigfelsCharacter::InputWheelClose()
{
	if (!bWheelOpen)
	{
		return;
	}
	bWheelOpen = false;
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->SetIgnoreLookInput(false);
	}
	if (WheelHover >= 1 && WheelHover <= UEwigfelsSpellCasterComponent::WheelSlotCount && !Caster->GetSlotSpell(WheelHover).IsNone())
	{
		SelectedSlot = WheelHover;
		if (const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetSlotSpell(SelectedSlot)))
		{
			ShowMessage(Spell->Name, Spell->Visual.GlowColor, 0.8f);
		}
	}
	WheelHover = 0;
}

void AEwigfelsCharacter::InputSpellbook()
{
	if (bWheelOpen)
	{
		InputWheelClose();
	}
	Spellbook->Toggle();
}

void AEwigfelsCharacter::InputCastSelected()
{
	if (!bWheelOpen)
	{
		CastSlot(SelectedSlot);
	}
}

void AEwigfelsCharacter::InputWheelPointMouse(const FInputActionValue& Value)
{
	const FVector2D Delta = Value.Get<FVector2D>();
	if (Delta.SizeSquared() > 4.f)
	{
		SetUsingGamepad(false);
	}
	if (bWheelOpen && !bUsingGamepad)
	{
		WheelMouse += Delta;
		const float Range = FMath::Max(WheelMouseRange, 1.f);
		if (WheelMouse.Size() > Range)
		{
			WheelMouse = WheelMouse.GetSafeNormal() * Range;
		}
		WheelPointer = WheelMouse / Range;
	}
}

void AEwigfelsCharacter::InputWheelPointStick(const FInputActionValue& Value)
{
	const FVector2D Stick = Value.Get<FVector2D>();
	if (Stick.SizeSquared() > 0.09f)
	{
		SetUsingGamepad(true);
	}
	if (bWheelOpen && bUsingGamepad)
	{
		// Stick zurück in die Mitte behält die letzte Wahl (wie bei Waffenrädern üblich)
		if (Stick.SizeSquared() > 0.25f)
		{
			WheelPointer = Stick.GetSafeNormal() * FMath::Min(Stick.Size(), 1.f);
		}
	}
}

void AEwigfelsCharacter::InputAnyKey(FKey Key)
{
	if (Key.IsGamepadKey())
	{
		SetUsingGamepad(true);
	}
	else if (Key.IsMouseButton() || !Key.IsAnalog())
	{
		SetUsingGamepad(false);
	}
}

void AEwigfelsCharacter::SetUsingGamepad(bool bGamepad)
{
	if (bUsingGamepad == bGamepad)
	{
		return;
	}
	bUsingGamepad = bGamepad;
	if (bWheelOpen)
	{
		WheelMouse = FVector2D::ZeroVector; // Zeiger nicht vom anderen Gerät erben
	}
}

void AEwigfelsCharacter::UpdateSpellWheel()
{
	if (!bWheelOpen)
	{
		return;
	}
	if (bInputBlocked || Combat->IsDead())
	{
		InputWheelClose();
		return;
	}
	// 6 Felder im Uhrzeigersinn, Platz 1 oben; Y zeigt bei Stick und Maus nach oben
	if (WheelPointer.Size() < 0.35f)
	{
		if (!bUsingGamepad)
		{
			WheelHover = 0; // Maus in der Mitte = nichts ändern
		}
		return;
	}
	float Angle = FMath::RadiansToDegrees(FMath::Atan2(WheelPointer.X, WheelPointer.Y));
	if (Angle < 0.f)
	{
		Angle += 360.f;
	}
	const int32 Count = UEwigfelsSpellCasterComponent::WheelSlotCount;
	const float Step = 360.f / Count;
	const int32 Sector = FMath::RoundToInt(Angle / Step) % Count;
	const int32 Slot = Sector + 1;
	WheelHover = Caster->GetSlotSpell(Slot).IsNone() ? 0 : Slot;
}

void AEwigfelsCharacter::UpdateInputDevice()
{
	// Linker Stick zählt auch (Laufen allein drückt keine Taste)
	if (const APlayerController* PC = Cast<APlayerController>(Controller))
	{
		const FVector2D Left(PC->GetInputAnalogKeyState(EKeys::Gamepad_LeftX), PC->GetInputAnalogKeyState(EKeys::Gamepad_LeftY));
		if (Left.SizeSquared() > 0.16f)
		{
			SetUsingGamepad(true);
		}
	}
}

void AEwigfelsCharacter::InputShieldStart()
{
	if (!bInputBlocked)
	{
		Caster->RequestShield(true);
	}
}

void AEwigfelsCharacter::InputShieldEnd()
{
	Caster->RequestShield(false);
}

void AEwigfelsCharacter::InputLockOn()
{
	if (!bInputBlocked)
	{
		LockOn->Toggle();
	}
}

void AEwigfelsCharacter::InputSwitchTarget(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	if (!FMath::IsNearlyZero(Axis))
	{
		LockOn->Switch(Axis > 0.f ? 1 : -1);
	}
}

void AEwigfelsCharacter::InputInteract()
{
	if (DialogueCamera->IsDialogueActive())
	{
		DialogueCamera->Advance();
		return;
	}
	AActor* Focus = FocusedInteractable.Get();
	if (!bInputBlocked && Focus && Focus->Implements<UEwigfelsInteractable>())
	{
		IEwigfelsInteractable::Execute_Interact(Focus, this);
	}
}

void AEwigfelsCharacter::InputToggleHelp()
{
	bShowHelp = !bShowHelp;
}

void AEwigfelsCharacter::HandleLockOnChanged(AActor* NewTarget)
{
	Caster->SetAimTarget(NewTarget);
	if (IsLocallyControlled())
	{
		bLockedOnReplicated = NewTarget != nullptr;
		Server_SetLockedOn(NewTarget != nullptr);
	}
}

void AEwigfelsCharacter::Server_SetLockedOn_Implementation(bool bLocked)
{
	bLockedOnReplicated = bLocked;
}

FVector AEwigfelsCharacter::ComputeAimPoint() const
{
	if (const AActor* Target = LockOn->GetTarget())
	{
		return Target->GetActorLocation();
	}
	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return GetActorLocation() + GetActorForwardVector() * 3000.f;
	}
	FVector ViewLoc;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewLoc, ViewRot);
	const FVector Dir = ViewRot.Vector();
	// Ab der Figur messen, damit nichts zwischen Kamera und Figur getroffen wird
	const float Skip = FVector::DotProduct(GetActorLocation() - ViewLoc, Dir);
	const FVector Start = ViewLoc + Dir * FMath::Max(0.f, Skip);
	const FVector End = ViewLoc + Dir * 12000.f;
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsAim), false, this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		return Hit.ImpactPoint;
	}
	return End;
}

// ---------------------------------------------------------------------------
// Tick

void AEwigfelsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateRotationMode();
	if (IsLocallyControlled() && IsPlayerControlled())
	{
		UpdateCamera(DeltaSeconds);
		UpdateCameraPunch(DeltaSeconds);
		UpdateInteractFocus();
		UpdateInputDevice();
		UpdateSpellWheel();
	}
	// Magische Landung: Beginn erkennen (Server + eigener Client rechnen sie in der Bewegung aus)
	if (const UEwigfelsCharacterMovementComponent* Move = GetEwigfelsMovement())
	{
		const bool bCatching = Move->IsFallCatching();
		if (bCatching && !bWasFallCatching)
		{
			PlayFallCatchFx(false);
			if (HasAuthority())
			{
				Multicast_FallCatchFx();
			}
		}
		bWasFallCatching = bCatching;
	}
	// Platzhalter: Hände zeigen, solange gezaubert wird
	if (PlaceholderBody->IsActiveBody())
	{
		const bool bCasting = Caster->IsCasting() || GetWorld()->GetTimeSeconds() < LocalCastPoseUntil;
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetCastState().SpellId);
		PlaceholderBody->SetCastPose(bCasting || Caster->IsShieldUp() ? 1.f : 0.f,
			Spell ? Spell->Visual.Color : FLinearColor(1.f, 0.85f, 0.5f), Caster->IsShieldUp() || (Spell && Spell->bTwoHanded));
		if (PlaceholderBody->IsPlayingDeath() && !Combat->IsDead())
		{
			PlaceholderBody->Revive();
		}
	}
}

void AEwigfelsCharacter::UpdateRotationMode()
{
	const bool bLocked = IsLocallyControlled() ? LockOn->GetTarget() != nullptr : bLockedOnReplicated;
	const bool bCasting = Caster->IsCasting() || GetWorld()->GetTimeSeconds() < LocalCastPoseUntil;
	const bool bNewStrafe = (bLocked || bCasting || Caster->IsShieldUp()) && !IsDodging();
	if (bNewStrafe != bStrafing)
	{
		bStrafing = bNewStrafe;
		GetCharacterMovement()->bOrientRotationToMovement = !bStrafing;
		GetCharacterMovement()->bUseControllerDesiredRotation = bStrafing;
	}
}

void AEwigfelsCharacter::DetectExternalCamera()
{
	// Nach Klassenname suchen, damit das Modul nicht vom GameplayCameras-Plugin abhängt
	TInlineComponentArray<UActorComponent*> Components(this);
	for (const UActorComponent* Comp : Components)
	{
		if (Comp && Comp->GetClass()->GetName().Contains(TEXT("GameplayCameraComponent")))
		{
			bExternalCamera = true;
			break;
		}
	}
	if (bExternalCamera)
	{
		// FollowCamera bleibt bestehen (Probestart lief so), wird aber nicht mehr nachgeführt
		UE_LOG(LogEwigfels, Log, TEXT("Figur: Kamera kommt aus dem Blueprint (%s)"), *GetClass()->GetName());
	}
}

void AEwigfelsCharacter::UpdateCamera(float DeltaSeconds)
{
	if (bExternalCamera || DialogueCamera->IsDialogueActive())
	{
		return;
	}
	const UEwigfelsCharacterMovementComponent* Move = GetEwigfelsMovement();
	float TargetArm = DefaultArmLength;
	float TargetFov = DefaultFov;
	FVector TargetOffset = DefaultSocketOffset;
	if (Move && Move->IsSprinting())
	{
		TargetFov = DefaultFov + 8.f;
		TargetArm = DefaultArmLength + 30.f;
	}
	if (Caster->IsShieldUp())
	{
		TargetArm = DefaultArmLength - 50.f;
	}
	if (LockOn->GetTarget())
	{
		TargetArm = DefaultArmLength + 60.f;
		TargetOffset = DefaultSocketOffset + FVector(0.f, 15.f, 20.f);
	}
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArm, DeltaSeconds, 5.f);
	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetOffset, DeltaSeconds, 5.f);
	FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, TargetFov, DeltaSeconds, 6.f));
}

void AEwigfelsCharacter::UpdateInteractFocus()
{
	AActor* Best = nullptr;
	float BestScore = -FLT_MAX;
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsInteract), false, this);
	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_Pawn);
	Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
	Objects.AddObjectTypesToQuery(ECC_WorldStatic);
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, Objects, FCollisionShape::MakeSphere(280.f), Params);
	for (const FOverlapResult& O : Overlaps)
	{
		AActor* A = O.GetActor();
		if (!A || A == this || !A->Implements<UEwigfelsInteractable>())
		{
			continue;
		}
		const FVector To = (A->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		const float Score = FVector::DotProduct(GetActorForwardVector(), To) - FVector::Dist(A->GetActorLocation(), GetActorLocation()) / 280.f;
		if (Score > BestScore)
		{
			BestScore = Score;
			Best = A;
		}
	}
	FocusedInteractable = Best;
}

void AEwigfelsCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (const UEwigfelsCharacterMovementComponent* Move = GetEwigfelsMovement())
	{
		if (Move->IsFallCatching()) // wird erst nach Landed zurückgesetzt
		{
			PlayFallCatchFx(true);
		}
	}
	if (PlaceholderBody->IsActiveBody())
	{
		PlaceholderBody->PlayHit(); // kleiner Federeffekt beim Landen
	}
}

// ---------------------------------------------------------------------------
// Rückmeldungen

void AEwigfelsCharacter::PlaySpellMontage(FName SpellId, uint8 ChainIndex)
{
	const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(SpellId);
	if (!Spell || !GetMesh() || !GetMesh()->GetAnimInstance())
	{
		return;
	}
	// Reihenfolge: eigene Montage der Weise, sonst Montage ihrer Fingergeste (Zweitwirkung = Geste "slam")
	const bool bRecast = Spell->RecastWindow > 0.f && ChainIndex == 1;
	UAnimMontage* Montage = bRecast ? nullptr : Cast<UAnimMontage>(EwigfelsSpellVisuals::LoadIfExists(Spell->Visual.Montage));
	if (!Montage)
	{
		const FName Gesture = bRecast ? FName(TEXT("slam")) : Spell->Visual.Gesture;
		Montage = Cast<UAnimMontage>(EwigfelsSpellVisuals::LoadIfExists(EwigfelsSpells::GestureMontagePath(Gesture)));
	}
	if (Montage)
	{
		const float Speed = Combat->GetActionSpeedMultiplier();
		PlayAnimMontage(Montage, Speed);
	}
}

void AEwigfelsCharacter::HandleCastStarted(FName SpellId, uint8 ChainIndex, float CastTime)
{
	LocalCastPoseUntil = GetWorld()->GetTimeSeconds() + CastTime + 0.3f;
	PlaySpellMontage(SpellId, ChainIndex);
	if (IsLocallyControlled())
	{
		// Zauberwort kurz einblenden (nur beim ersten Kettenglied, sonst flackert es beim Funkenstoß)
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(SpellId);
		if (Spell && ChainIndex == 0 && !Spell->Incantation.IsEmpty())
		{
			ShowMessage(Spell->Incantation, Spell->Visual.GlowColor, 0.9f);
		}

		// Beim Zaubern sofort zur Kamera drehen (sonst fliegt der erste Schuss seitlich weg)
		if (const AController* C = GetController())
		{
			const FRotator Yaw(0.f, C->GetControlRotation().Yaw, 0.f);
			if (LockOn->GetTarget() == nullptr && FMath::Abs(FRotator::NormalizeAxis(Yaw.Yaw - GetActorRotation().Yaw)) > 60.f)
			{
				SetActorRotation(Yaw);
			}
		}
		NoteTutorialStep(TEXT("cast"));
	}
}

void AEwigfelsCharacter::HandleImpactFx(FName SpellId, FVector Location, FVector Normal, bool bBig)
{
	// Schwerer Aufprall in der Nähe: Kamera zittert (nur lokal)
	if (!IsLocallyControlled())
	{
		return;
	}
	if (FVector::DistSquared(Location, GetActorLocation()) < FMath::Square(2500.f))
	{
		AddCameraShake(bBig ? 1.1f : 0.5f, bBig ? 0.4f : 0.2f);
	}
}

void AEwigfelsCharacter::HandleSpellReleased(FName SpellId, FVector Origin, FVector Direction)
{
	if (const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(SpellId))
	{
		EwigfelsSpellVisuals::PlayRelease(GetWorld(), *Spell, Origin, Direction, this);
	}
}

void AEwigfelsCharacter::HandleCastFailed(FName SpellId, EEwigfelsCastFailReason Reason)
{
	LocalCastPoseUntil = 0.f;
	StopAnimMontage();
	if (!IsLocallyControlled())
	{
		return;
	}
	switch (Reason)
	{
	case EEwigfelsCastFailReason::NoAtem:
		ShowMessage(LOCTEXT("NoAtem", "Kein Atem"), FLinearColor(0.45f, 0.7f, 1.f));
		break;
	case EEwigfelsCastFailReason::Cooldown:
		ShowMessage(LOCTEXT("Cooldown", "Weise klingt noch nach"), FLinearColor(0.8f, 0.8f, 0.85f), 0.8f);
		break;
	case EEwigfelsCastFailReason::CannotAct:
		ShowMessage(LOCTEXT("CannotAct", "Gerade nicht möglich"), FLinearColor(0.8f, 0.8f, 0.85f), 0.8f);
		break;
	default:
		break;
	}
}

void AEwigfelsCharacter::HandleShieldChanged(bool bUp)
{
	const FEwigfelsSpellDef* Shield = Caster->FindSpellDef(Caster->ShieldSpell);
	const FLinearColor Color = Shield ? Shield->Visual.Color : FLinearColor::White;
	PlaceholderBody->SetShieldVisible(bUp, Color);
	if (bUp && Shield)
	{
		EwigfelsSpellVisuals::SpawnFlash(GetWorld(), TEXT("sphere"), Caster->GetCastOrigin(true), GetActorRotation(), Color,
			FVector(0.1f), FVector(0.9f), 0.18f, 3000.f, 400.f);
	}
}

void AEwigfelsCharacter::HandleParried(AActor* Attacker, FVector Location)
{
	const FEwigfelsSpellDef* Shield = Caster->FindSpellDef(Caster->ShieldSpell);
	const FLinearColor Color = Shield ? Shield->Visual.Color : FLinearColor::White;
	if (!Shield || !EwigfelsSpellVisuals::SpawnNiagaraAt(GetWorld(), Shield->Visual.ImpactNiagara, Location, GetActorRotation(), Color, 1.f))
	{
		EwigfelsSpellVisuals::SpawnFlash(GetWorld(), TEXT("sphere"), GetActorLocation(), FRotator::ZeroRotator, Color,
			FVector(0.5f), FVector(5.f), 0.45f, 30000.f, 1200.f);
	}
	if (IsLocallyControlled())
	{
		ShowMessage(LOCTEXT("Parry", "Reinklang!"), FLinearColor(1.f, 0.95f, 0.7f), 1.f);
	}
}

void AEwigfelsCharacter::HandleHitFeedback(float Amount, FVector Location, EEwigfelsHitResult Result, AActor* InstigatorActor)
{
	if (IsLocallyControlled() && (Result == EEwigfelsHitResult::Hit || Result == EEwigfelsHitResult::Blocked) && Amount > 0.f)
	{
		AddCameraPunch(-FMath::Clamp(Amount * 0.08f, 0.4f, 2.2f), FMath::FRandRange(-0.6f, 0.6f));
	}
	if (Result == EEwigfelsHitResult::Hit || Result == EEwigfelsHitResult::Blocked || Result == EEwigfelsHitResult::Killed)
	{
		PlaceholderBody->PlayHit();
	}
	if (IsLocallyControlled() && Result == EEwigfelsHitResult::Ignored && Amount <= 0.f && IsDodging())
	{
		ShowMessage(LOCTEXT("Dodged", "Ausgewichen"), FLinearColor(0.7f, 0.85f, 1.f), 0.6f);
	}
}

void AEwigfelsCharacter::HandleBeamFired(FName SpellId, FVector Start, FVector End, bool bHitTarget, uint8 ChainIndex)
{
	if (!IsLocallyControlled())
	{
		return;
	}
	const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(SpellId);
	const bool bStrong = Spell && Spell->Delivery.Chain > 1 && ChainIndex + 1 >= Spell->Delivery.Chain;
	// Rückstoß nach oben, abwechselnd leicht links/rechts; der dritte Schlag und Treffer knallen mehr
	const float Side = (ChainIndex % 2 == 0) ? 1.f : -1.f;
	AddCameraPunch((bStrong ? 1.6f : 0.7f) * (bHitTarget ? 1.25f : 1.f), Side * (bStrong ? 0.5f : 0.25f));
	AddCameraShake(bStrong ? 0.9f : 0.25f, bStrong ? 0.35f : 0.16f);
	EwigfelsSpellVisuals::FlashScreen(GetWorld(), bStrong ? 0.2f : 0.07f, bStrong ? 0.07f : 0.04f);
}

void AEwigfelsCharacter::AddCameraShake(float AmplitudeDeg, float Seconds)
{
	if (CameraPunchStrength <= 0.f)
	{
		return;
	}
	const float Remaining = ShakeAge >= 0.f && ShakeDuration > 0.f ? ShakeAmp * FMath::Square(1.f - ShakeAge / ShakeDuration) : 0.f;
	ShakeAmp = FMath::Max(Remaining, AmplitudeDeg * CameraPunchStrength);
	ShakeDuration = FMath::Clamp(Seconds, 0.15f, 0.4f);
	ShakeAge = 0.f;
}

void AEwigfelsCharacter::AddCameraPunch(float PitchDeg, float YawDeg)
{
	if (CameraPunchStrength <= 0.f)
	{
		return;
	}
	PunchPeak = FRotator(PitchDeg * CameraPunchStrength, YawDeg * CameraPunchStrength, 0.f);
	PunchAge = 0.f;
}

void AEwigfelsCharacter::UpdateCameraPunch(float DeltaSeconds)
{
	AController* C = GetController();
	if ((PunchAge < 0.f && ShakeAge < 0.f && PunchApplied.IsZero()) || !C)
	{
		return;
	}
	// 40 ms hoch, 180 ms zurück: wirkt wie ein Schlag, die Blickrichtung bleibt am Ende gleich
	constexpr float Attack = 0.04f;
	constexpr float Release = 0.18f;
	float Weight = 0.f;
	if (PunchAge >= 0.f)
	{
		PunchAge += DeltaSeconds;
	}
	if (PunchAge < 0.f)
	{
		Weight = 0.f;
	}
	else if (PunchAge < Attack)
	{
		Weight = PunchAge / Attack;
	}
	else if (PunchAge < Attack + Release)
	{
		const float T = (PunchAge - Attack) / Release;
		Weight = 1.f - T * T * (3.f - 2.f * T);
	}
	else
	{
		PunchAge = -1.f;
	}
	FRotator Wanted = PunchPeak * Weight;
	if (ShakeAge >= 0.f)
	{
		ShakeAge += DeltaSeconds;
		if (ShakeAge >= ShakeDuration)
		{
			ShakeAge = -1.f;
		}
		else
		{
			const float K = FMath::Square(1.f - ShakeAge / ShakeDuration) * ShakeAmp;
			Wanted += FRotator(FMath::FRandRange(-1.f, 1.f) * K, FMath::FRandRange(-1.f, 1.f) * K, 0.f);
		}
	}
	const FRotator Delta = Wanted - PunchApplied;
	PunchApplied = Wanted;
	C->SetControlRotation(C->GetControlRotation() + Delta);
}

void AEwigfelsCharacter::Multicast_FallCatchFx_Implementation()
{
	if (!HasAuthority() && !IsLocallyControlled())
	{
		PlayFallCatchFx(false);
	}
}

void AEwigfelsCharacter::PlayFallCatchFx(bool bLanding)
{
	UWorld* World = GetWorld();
	if (!World || World->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	if (!bLanding && World->GetTimeSeconds() - LastFallCatchFxTime < 0.5f)
	{
		return;
	}
	LastFallCatchFxTime = World->GetTimeSeconds();
	const FLinearColor Color(0.62f, 0.86f, 1.f);
	const FVector Feet = GetActorLocation() - FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	if (bLanding)
	{
		// Aufsetzen: weicher Klangring am Boden
		EwigfelsSpellVisuals::SpawnFlash(World, TEXT("cylinder"), Feet + FVector(0.f, 0.f, 3.f), FRotator::ZeroRotator, Color,
			FVector(0.4f, 0.4f, 0.01f), FVector(3.2f, 3.2f, 0.015f), 0.45f, 9000.f, 700.f);
		return;
	}
	// Abfangen: Ring unter den Füßen, Funken an beiden Händen
	EwigfelsSpellVisuals::SpawnFlash(World, TEXT("cylinder"), Feet - FVector(0.f, 0.f, 20.f), FRotator::ZeroRotator, Color,
		FVector(0.3f, 0.3f, 0.01f), FVector(2.2f, 2.2f, 0.02f), 0.6f, 12000.f, 800.f);
	if (USkeletalMeshComponent* SkelMesh = GetMesh())
	{
		for (const TCHAR* Bone : { TEXT("hand_r"), TEXT("hand_l") })
		{
			if (SkelMesh->GetSkeletalMeshAsset() && SkelMesh->DoesSocketExist(Bone))
			{
				EwigfelsSpellVisuals::SpawnFlash(World, TEXT("sphere"), SkelMesh->GetSocketLocation(Bone), FRotator::ZeroRotator, Color,
					FVector(0.05f), FVector(0.4f), 0.35f, 3000.f, 300.f);
			}
		}
		if (UAnimMontage* Montage = Cast<UAnimMontage>(EwigfelsSpellVisuals::LoadIfExists(FallCatchMontage)))
		{
			PlayAnimMontage(Montage);
		}
	}
}

void AEwigfelsCharacter::HandleStaggered(float Seconds)
{
	// Schwerer Treffer unterbricht Ausholen und senkt den Schild
	LocalCastPoseUntil = 0.f;
	StopAnimMontage();
	if (HasAuthority())
	{
		Caster->InterruptCast();
	}
}

void AEwigfelsCharacter::HandleDied(AActor* Killer)
{
	PlaceholderBody->PlayDeath();
	StopAnimMontage();
	if (IsLocallyControlled())
	{
		LockOn->Release();
		ShowMessage(LOCTEXT("Died", "Du sinkst zu Boden …"), FLinearColor(1.f, 0.5f, 0.45f), RespawnDelay);
	}
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(RespawnTimer, this, &AEwigfelsCharacter::Respawn, RespawnDelay, false);
	}
}

void AEwigfelsCharacter::Respawn()
{
	AGameModeBase* GM = GetWorld()->GetAuthGameMode();
	if (GM && GetController())
	{
		if (AActor* Start = GM->FindPlayerStart(GetController()))
		{
			TeleportTo(Start->GetActorLocation(), Start->GetActorRotation());
		}
	}
	Combat->ResetCombatState();
	Caster->RestoreAtem(Caster->GetMaxAtem());
}

void AEwigfelsCharacter::NoteTutorialStep(FName Step)
{
	if (bTutorialReported)
	{
		return;
	}
	TutorialSteps.Add(Step);
	if (TutorialSteps.Contains(StepMove) && TutorialSteps.Contains(StepJump) && TutorialSteps.Contains(StepDodge))
	{
		bTutorialReported = true;
		Server_ReportTutorial(TutorialMovement);
	}
}

void AEwigfelsCharacter::Server_ReportTutorial_Implementation(FName Step)
{
	// Nur bekannte Schritte annehmen (Schutz gegen beliebige Quest-Ereignisse vom Client)
	if (Step != TutorialMovement)
	{
		return;
	}
	if (UEwigfelsQuestSubsystem* Quests = GetGameInstance() ? GetGameInstance()->GetSubsystem<UEwigfelsQuestSubsystem>() : nullptr)
	{
		Quests->ReportEvent(TEXT("custom"), Step.ToString());
	}
}

void AEwigfelsCharacter::ShowMessage(const FText& Text, const FLinearColor& Color, float Seconds)
{
	MessageText = Text;
	MessageColor = Color;
	MessageStart = GetWorld()->GetRealTimeSeconds();
	MessageLength = FMath::Max(0.2f, Seconds);
}

bool AEwigfelsCharacter::GetMessage(FText& OutText, FLinearColor& OutColor, float& OutAlpha) const
{
	const float Age = GetWorld()->GetRealTimeSeconds() - MessageStart;
	if (Age > MessageLength || MessageText.IsEmpty())
	{
		return false;
	}
	OutText = MessageText;
	OutColor = MessageColor;
	OutAlpha = FMath::Clamp((MessageLength - Age) / 0.3f, 0.f, 1.f);
	return true;
}

#undef LOCTEXT_NAMESPACE
