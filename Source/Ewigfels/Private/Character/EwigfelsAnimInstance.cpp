// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Character/EwigfelsAnimInstance.h"
#include "Character/EwigfelsCharacter.h"
#include "Character/EwigfelsCharacterMovementComponent.h"
#include "Character/EwigfelsLockOnComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "KismetAnimationLibrary.h"

void UEwigfelsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<AEwigfelsCharacter>(TryGetPawnOwner());
}

void UEwigfelsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<AEwigfelsCharacter>(TryGetPawnOwner());
		if (!OwnerCharacter)
		{
			return;
		}
	}
	// Alles, was Actor/Komponenten liest, hier im Game-Thread
	const UEwigfelsCharacterMovementComponent* Move = OwnerCharacter->GetEwigfelsMovement();
	Velocity = OwnerCharacter->GetVelocity();
	Acceleration = Move ? Move->GetCurrentAcceleration() : FVector::ZeroVector;
	bIsFalling = Move && Move->IsFalling();
	bIsSprinting = Move && Move->IsSprinting();
	bIsStrafing = OwnerCharacter->IsStrafing();
	bIsDodging = OwnerCharacter->IsDodging();
	ActorRotation = OwnerCharacter->GetActorRotation();
	AimRotation = OwnerCharacter->GetBaseAimRotation();

	if (const UEwigfelsSpellCasterComponent* Caster = OwnerCharacter->GetCaster())
	{
		bIsCasting = Caster->IsCasting();
		bShieldUp = Caster->IsShieldUp();
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetCastState().SpellId);
		bTwoHandedCast = (Spell && Spell->bTwoHanded) || bShieldUp;
		CastSchool = Spell ? Spell->School : NAME_None;
		CastNote = Spell && Spell->Chord.Num() > 0 ? Spell->Chord[0] : NAME_None;
	}
	if (const UEwigfelsCombatComponent* Combat = OwnerCharacter->GetCombat())
	{
		bIsDead = Combat->IsDead();
		bIsStaggered = Combat->IsStaggered();
		Exhaustion = Combat->GetMaxHealth() > 0.f ? 1.f - Combat->GetHealth() / Combat->GetMaxHealth() : 0.f;
		bHandReadyTarget = bIsCasting || bShieldUp || (OwnerCharacter->GetLockOn() && OwnerCharacter->GetLockOn()->GetTarget());
	}
}

void UEwigfelsAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	GroundSpeed = Velocity.Size2D();
	bIsMoving = GroundSpeed > 5.f;
	bHasMoveInput = Acceleration.SizeSquared2D() > 1.f;
	Direction = bIsMoving ? UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation) : 0.f;

	// Kurvenneigung aus Drehgeschwindigkeit
	const float YawDelta = FRotator::NormalizeAxis(ActorRotation.Yaw - PreviousYaw);
	PreviousYaw = ActorRotation.Yaw;
	const float TargetLean = DeltaSeconds > 0.f ? FMath::Clamp(YawDelta / DeltaSeconds / 360.f, -1.f, 1.f) : 0.f;
	Lean = FMath::FInterpTo(Lean, bIsMoving ? TargetLean : 0.f, DeltaSeconds, 6.f);

	const FRotator Delta = (AimRotation - ActorRotation).GetNormalized();
	AimPitch = FMath::Clamp(Delta.Pitch, -90.f, 90.f);
	AimYaw = FMath::Clamp(Delta.Yaw, -120.f, 120.f);

	HandReadyAlpha = FMath::FInterpTo(HandReadyAlpha, bHandReadyTarget ? 1.f : 0.f, DeltaSeconds, 10.f);
}
