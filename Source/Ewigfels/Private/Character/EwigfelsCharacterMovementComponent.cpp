// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Character/EwigfelsCharacterMovementComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

namespace
{
	/** Saved Move mit Sprint-Bit (FLAG_Custom_0). */
	class FSavedMove_Ewigfels : public FSavedMove_Character
	{
	public:
		using Super = FSavedMove_Character;

		uint8 bSavedWantsToSprint = 0;

		virtual void Clear() override
		{
			Super::Clear();
			bSavedWantsToSprint = 0;
		}

		virtual uint8 GetCompressedFlags() const override
		{
			uint8 Result = Super::GetCompressedFlags();
			if (bSavedWantsToSprint)
			{
				Result |= FLAG_Custom_0;
			}
			return Result;
		}

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override
		{
			if (bSavedWantsToSprint != static_cast<const FSavedMove_Ewigfels*>(NewMove.Get())->bSavedWantsToSprint)
			{
				return false;
			}
			return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
		}

		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override
		{
			Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
			if (const UEwigfelsCharacterMovementComponent* Move = Cast<UEwigfelsCharacterMovementComponent>(C->GetCharacterMovement()))
			{
				bSavedWantsToSprint = Move->bWantsToSprint;
			}
		}

		virtual void PrepMoveFor(ACharacter* C) override
		{
			Super::PrepMoveFor(C);
			if (UEwigfelsCharacterMovementComponent* Move = Cast<UEwigfelsCharacterMovementComponent>(C->GetCharacterMovement()))
			{
				Move->bWantsToSprint = bSavedWantsToSprint;
			}
		}
	};

	class FNetworkPredictionData_Client_Ewigfels : public FNetworkPredictionData_Client_Character
	{
	public:
		explicit FNetworkPredictionData_Client_Ewigfels(const UCharacterMovementComponent& ClientMovement)
			: FNetworkPredictionData_Client_Character(ClientMovement)
		{
		}

		virtual FSavedMovePtr AllocateNewMove() override
		{
			return FSavedMovePtr(new FSavedMove_Ewigfels());
		}
	};
}

UEwigfelsCharacterMovementComponent::UEwigfelsCharacterMovementComponent()
{
	bWantsToSprint = false;
	MaxWalkSpeed = 420.f;
	MaxAcceleration = 2000.f;
	BrakingDecelerationWalking = 1600.f;
	GroundFriction = 8.f;
	JumpZVelocity = 520.f;
	AirControl = 0.35f;
	RotationRate = FRotator(0.f, 600.f, 0.f);
	bOrientRotationToMovement = true;
	NavAgentProps.bCanCrouch = false;
}

FNetworkPredictionData_Client* UEwigfelsCharacterMovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UEwigfelsCharacterMovementComponent* MutableThis = const_cast<UEwigfelsCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Ewigfels(*this);
	}
	return ClientPredictionData;
}

void UEwigfelsCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

bool UEwigfelsCharacterMovementComponent::IsSprinting() const
{
	if (!bWantsToSprint || !IsMovingOnGround() || Velocity.SizeSquared2D() < 100.f)
	{
		return false;
	}
	const AActor* Owner = GetOwner();
	const UEwigfelsSpellCasterComponent* Caster = Owner ? Owner->FindComponentByClass<UEwigfelsSpellCasterComponent>() : nullptr;
	return !(Caster && (Caster->IsShieldUp() || Caster->IsCasting()));
}

float UEwigfelsCharacterMovementComponent::GetMaxSpeed() const
{
	float Speed = Super::GetMaxSpeed();
	if (MovementMode != MOVE_Walking && MovementMode != MOVE_NavWalking)
	{
		return Speed;
	}
	if (IsSprinting())
	{
		Speed = SprintSpeed;
	}
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return Speed;
	}
	if (const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(Owner))
	{
		Speed *= Combat->GetMoveSpeedMultiplier();
		if (!Combat->CanAct() && !Combat->IsDead())
		{
			Speed *= 0.15f; // taumeln/betäubt: fast stehen
		}
	}
	if (const UEwigfelsSpellCasterComponent* Caster = Owner->FindComponentByClass<UEwigfelsSpellCasterComponent>())
	{
		if (Caster->IsShieldUp())
		{
			Speed *= ShieldSpeedMultiplier;
		}
		else if (Caster->IsCasting())
		{
			const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetCastState().SpellId);
			Speed *= Spell && Spell->bTwoHanded ? TwoHandedCastingSpeedMultiplier : CastingSpeedMultiplier;
		}
	}
	return Speed;
}

// ---------------------------------------------------------------------------
// Magische Landung

float UEwigfelsCharacterMovementComponent::PredictImpactSpeed(float DownSpeed, float HeightAboveGround, float GravityAbs)
{
	// v² = v0² + 2·g·h
	return FMath::Sqrt(FMath::Max(0.f, FMath::Square(DownSpeed) + 2.f * GravityAbs * FMath::Max(0.f, HeightAboveGround)));
}

float UEwigfelsCharacterMovementComponent::ComputeCatchDeceleration(float DownSpeed, float TargetSpeed, float HeightAboveGround, float GravityAbs)
{
	if (DownSpeed <= TargetSpeed)
	{
		return 0.f;
	}
	// Bremsweg mit Reserve, damit die Figur nicht erst am Boden langsam wird; Schwerkraft wirkt weiter und wird mitgebremst
	const float Distance = FMath::Max(40.f, HeightAboveGround - 60.f);
	const float Needed = (FMath::Square(DownSpeed) - FMath::Square(TargetSpeed)) / (2.f * Distance) + GravityAbs;
	return FMath::Min(Needed, 9000.f);
}

float UEwigfelsCharacterMovementComponent::TraceGroundDistance(float MaxDistance) const
{
	const ACharacter* Char = GetCharacterOwner();
	const UCapsuleComponent* Capsule = Char ? Char->GetCapsuleComponent() : nullptr;
	if (!Capsule || !GetWorld())
	{
		return -1.f;
	}
	const FVector Bottom = Capsule->GetComponentLocation() - FVector(0.f, 0.f, Capsule->GetScaledCapsuleHalfHeight());
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsFallCatch), false, Char);
	FCollisionResponseParams Response;
	Capsule->InitSweepCollisionParams(Params, Response);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Bottom, Bottom - FVector(0.f, 0.f, MaxDistance), Capsule->GetCollisionObjectType(), Params, Response))
	{
		return Hit.Distance;
	}
	return -1.f;
}

FVector UEwigfelsCharacterMovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const
{
	FVector Result = Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);
	const float GravityAbs = FMath::Abs(Gravity.Z);
	const float DownSpeed = -Result.Z;
	if (!bFallCatchEnabled || DownSpeed <= FallCatchLandingSpeed || GravityAbs <= KINDA_SMALL_NUMBER)
	{
		return Result;
	}
	const float Height = TraceGroundDistance(FallCatchHeight);
	if (Height < 0.f)
	{
		return Result;
	}
	if (!bFallCatching && PredictImpactSpeed(DownSpeed, Height, GravityAbs) >= FallCatchTriggerSpeed)
	{
		bFallCatching = true;
	}
	if (bFallCatching)
	{
		const float Decel = ComputeCatchDeceleration(DownSpeed, FallCatchLandingSpeed, Height, GravityAbs);
		Result.Z = FMath::Min(Result.Z + Decel * DeltaTime, -FallCatchLandingSpeed);
		Result.X *= 0.985f; // etwas Schwung raus, wirkt kontrollierter
		Result.Y *= 0.985f;
	}
	return Result;
}

void UEwigfelsCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode != MOVE_Falling)
	{
		bFallCatching = false;
	}
}
