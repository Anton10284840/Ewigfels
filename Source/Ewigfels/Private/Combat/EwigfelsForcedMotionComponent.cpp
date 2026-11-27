// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Combat/EwigfelsForcedMotionComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Magic/EwigfelsSpellEffects.h"
#include "Magic/EwigfelsSpellSubsystem.h"
#include "AIController.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
	const FName LiftedStatus(TEXT("lifted"));
}

UEwigfelsForcedMotionComponent::UEwigfelsForcedMotionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false);
}

UEwigfelsForcedMotionComponent* UEwigfelsForcedMotionComponent::FindOrAdd(AActor* Actor)
{
	ACharacter* Character = Cast<ACharacter>(Actor);
	if (!Character || !Character->HasAuthority() || !Character->GetCharacterMovement())
	{
		return nullptr;
	}
	if (UEwigfelsForcedMotionComponent* Existing = Character->FindComponentByClass<UEwigfelsForcedMotionComponent>())
	{
		return Existing;
	}
	UEwigfelsForcedMotionComponent* Comp = NewObject<UEwigfelsForcedMotionComponent>(Character, TEXT("EwigfelsForcedMotion"));
	Comp->RegisterComponent();
	return Comp;
}

UCharacterMovementComponent* UEwigfelsForcedMotionComponent::GetMovement() const
{
	const ACharacter* Character = Cast<ACharacter>(GetOwner());
	return Character ? Character->GetCharacterMovement() : nullptr;
}

float UEwigfelsForcedMotionComponent::HoldHeightAt(float InElapsed, float InRiseTime, float Height)
{
	if (InRiseTime <= KINDA_SMALL_NUMBER || InElapsed >= InRiseTime)
	{
		// Schweben: ±6 % der Höhe, langsam
		const float Bob = FMath::Sin((InElapsed - InRiseTime) * 3.2f) * Height * 0.06f;
		return Height + Bob;
	}
	const float A = FMath::Clamp(InElapsed / InRiseTime, 0.f, 1.f);
	return Height * (1.f - FMath::Square(1.f - A)); // ease-out
}

void UEwigfelsForcedMotionComponent::Lift(float Height, float Duration, FName SpellId, AActor* Instigator)
{
	UCharacterMovementComponent* Move = GetMovement();
	if (!Move || !GetOwner()->HasAuthority())
	{
		return;
	}
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (AAIController* AI = Cast<AAIController>(Character->GetController()))
		{
			AI->StopMovement();
		}
	}
	bHolding = true;
	bAwaitLanding = false;
	Elapsed = 0.f;
	HoldDuration = FMath::Max(0.2f, Duration);
	HoldHeight = FMath::Max(20.f, Height);
	StartZ = GetOwner()->GetActorLocation().Z;
	MotionSpell = SpellId;
	MotionInstigator = Instigator;
	Move->SetMovementMode(MOVE_Flying);
	Move->Velocity = FVector::ZeroVector;
	if (UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(GetOwner()))
	{
		Combat->ApplyStatus(LiftedStatus, HoldDuration, Instigator);
	}
	if (UEwigfelsSpellCasterComponent* Caster = Instigator ? Instigator->FindComponentByClass<UEwigfelsSpellCasterComponent>() : nullptr)
	{
		Caster->NotifyHeldTarget(SpellId, GetOwner(), HoldDuration);
	}
	SetComponentTickEnabled(true);
}

void UEwigfelsForcedMotionComponent::Slam(float Speed, FName SpellId, AActor* Instigator)
{
	UCharacterMovementComponent* Move = GetMovement();
	if (!Move || !GetOwner()->HasAuthority())
	{
		return;
	}
	bHolding = false;
	if (UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(GetOwner()))
	{
		Combat->RemoveStatus(LiftedStatus);
	}
	Move->SetMovementMode(MOVE_Falling);
	Move->Velocity = FVector(0.f, 0.f, -FMath::Max(600.f, Speed));
	ExpectLanding(SpellId, Instigator);
}

void UEwigfelsForcedMotionComponent::ExpectLanding(FName SpellId, AActor* Instigator)
{
	bAwaitLanding = true;
	AwaitSince = 0.f;
	MotionSpell = SpellId;
	MotionInstigator = Instigator;
	SetComponentTickEnabled(true);
}

void UEwigfelsForcedMotionComponent::Release()
{
	const bool bWasHolding = bHolding;
	bHolding = false;
	bAwaitLanding = false;
	SetComponentTickEnabled(false);
	if (bWasHolding)
	{
		if (UCharacterMovementComponent* Move = GetMovement())
		{
			Move->SetMovementMode(MOVE_Falling);
		}
	}
}

bool UEwigfelsForcedMotionComponent::IsHeldBy(const AActor* Instigator, FName SpellId) const
{
	return bHolding && MotionSpell == SpellId && MotionInstigator.Get() == Instigator;
}

void UEwigfelsForcedMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UCharacterMovementComponent* Move = GetMovement();
	const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(GetOwner());
	if (!Move || (Combat && Combat->IsDead()))
	{
		Release();
		return;
	}

	if (bHolding)
	{
		Elapsed += DeltaTime;
		if (Elapsed >= HoldDuration || !Combat || !Combat->HasStatus(LiftedStatus))
		{
			// Zeit um oder Status entfernt (z. B. Feuer): sanft fallen lassen
			Release();
			return;
		}
		// Geschwindigkeit so setzen, dass die Figur im nächsten Frame auf Sollhöhe ist (Kollision bleibt aktiv)
		const float TargetZ = StartZ + HoldHeightAt(Elapsed, RiseTime, HoldHeight);
		const float CurrentZ = GetOwner()->GetActorLocation().Z;
		Move->Velocity = FVector(0.f, 0.f, (TargetZ - CurrentZ) / FMath::Max(DeltaTime, 0.001f));
		return;
	}

	if (bAwaitLanding)
	{
		AwaitSince += DeltaTime;
		if (AwaitSince > 0.08f && Move->IsMovingOnGround())
		{
			FireLanding();
		}
		else if (AwaitSince > 4.f)
		{
			Release(); // nie gelandet (z. B. ins Wasser) – vergessen
		}
		return;
	}
	SetComponentTickEnabled(false);
}

void UEwigfelsForcedMotionComponent::FireLanding()
{
	bAwaitLanding = false;
	SetComponentTickEnabled(false);
	AActor* Owner = GetOwner();
	AActor* Instigator = MotionInstigator.Get();
	const UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	const FEwigfelsSpellDef* Spell = Spells ? Spells->FindSpell(MotionSpell) : nullptr;
	if (!Spell || Spell->OnLand.Num() == 0 || !Instigator)
	{
		return;
	}
	const FVector Ground = Owner->GetActorLocation() - FVector(0.f, 0.f, 80.f);

	// Ziel selbst, dann alle im Aufprall-Radius (Druckwelle)
	TArray<AActor*> Victims;
	Victims.Add(Owner);
	if (Spell->LandRadius > 0.f)
	{
		for (AActor* A : EwigfelsSpellEffects::GatherSphere(Instigator, Ground, Spell->LandRadius, Owner))
		{
			Victims.AddUnique(A);
		}
	}
	for (AActor* Victim : Victims)
	{
		FEwigfelsSpellHit Hit;
		Hit.Caster = Instigator;
		Hit.Target = Victim;
		Hit.Location = Victim->GetActorLocation();
		Hit.Direction = (Victim->GetActorLocation() - Ground).GetSafeNormal2D();
		EwigfelsSpellEffects::ApplyEffects(*Spell, Spell->OnLand, Hit);
	}
	if (UEwigfelsSpellCasterComponent* Caster = Instigator->FindComponentByClass<UEwigfelsSpellCasterComponent>())
	{
		Caster->BroadcastImpactFx(Spell->Id, Ground, FVector::UpVector, true);
	}
}
