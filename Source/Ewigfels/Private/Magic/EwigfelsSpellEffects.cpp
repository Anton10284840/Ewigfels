// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsSpellEffects.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

float EwigfelsSpellEffects::ComputeDamage(const FEwigfelsSpellEffect& Effect, const UEwigfelsCombatComponent* Target, bool& bOutConsume)
{
	bOutConsume = false;
	float Amount = Effect.Amount;
	if (!Effect.Bonus.IfStatus.IsNone() && Target && Target->HasStatus(Effect.Bonus.IfStatus))
	{
		Amount *= Effect.Bonus.Multiplier;
		bOutConsume = Effect.Bonus.bConsume;
	}
	return FMath::Max(0.f, Amount);
}

bool EwigfelsSpellEffects::IsInCone(const FVector& Origin, const FVector& Forward, float Range, float AngleDeg, const FVector& Point)
{
	FVector Delta = Point - Origin;
	Delta.Z *= 0.5f;
	const float Dist = Delta.Size();
	if (Dist > Range)
	{
		return false;
	}
	if (Dist < KINDA_SMALL_NUMBER)
	{
		return true;
	}
	const FVector Flat = FVector(Forward.X, Forward.Y, 0.f).GetSafeNormal();
	const FVector Dir = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();
	if (Dir.IsNearlyZero())
	{
		return true; // direkt über/unter dem Wirker
	}
	const float CosHalf = FMath::Cos(FMath::DegreesToRadians(AngleDeg * 0.5f));
	return FVector::DotProduct(Flat, Dir) >= CosHalf;
}

namespace
{
	bool HasLineOfSight(UWorld* World, const FVector& From, const AActor* Target, const AActor* Caster)
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsSpellLOS), false, Caster);
		Params.AddIgnoredActor(Target);
		FHitResult Hit;
		return !World->LineTraceSingleByChannel(Hit, From, Target->GetActorLocation(), ECC_Visibility, Params);
	}

	TArray<AActor*> OverlapHostiles(AActor* Caster, const FVector& Origin, float Radius, const AActor* Ignore)
	{
		TArray<AActor*> Out;
		UWorld* World = Caster ? Caster->GetWorld() : nullptr;
		if (!World)
		{
			return Out;
		}
		TArray<FOverlapResult> Overlaps;
		FCollisionObjectQueryParams Objects;
		Objects.AddObjectTypesToQuery(ECC_Pawn);
		Objects.AddObjectTypesToQuery(ECC_PhysicsBody);
		FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsSpellOverlap), false, Caster);
		World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, Objects, FCollisionShape::MakeSphere(Radius), Params);
		for (const FOverlapResult& O : Overlaps)
		{
			AActor* A = O.GetActor();
			if (!A || A == Ignore || Out.Contains(A))
			{
				continue;
			}
			const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(A);
			if (Combat && !Combat->IsDead() && UEwigfelsCombatComponent::AreHostile(Caster, A))
			{
				Out.Add(A);
			}
		}
		return Out;
	}
}

TArray<AActor*> EwigfelsSpellEffects::GatherCone(AActor* Caster, const FVector& Origin, const FVector& Forward, float Range, float AngleDeg)
{
	TArray<AActor*> Out;
	for (AActor* A : OverlapHostiles(Caster, Origin, Range, nullptr))
	{
		if (IsInCone(Origin, Forward, Range, AngleDeg, A->GetActorLocation()) && HasLineOfSight(Caster->GetWorld(), Origin, A, Caster))
		{
			Out.Add(A);
		}
	}
	return Out;
}

TArray<AActor*> EwigfelsSpellEffects::GatherSphere(AActor* Caster, const FVector& Origin, float Radius, const AActor* Ignore)
{
	TArray<AActor*> Out;
	for (AActor* A : OverlapHostiles(Caster, Origin, Radius, Ignore))
	{
		if (HasLineOfSight(Caster->GetWorld(), Origin, A, Caster))
		{
			Out.Add(A);
		}
	}
	return Out;
}

TArray<AActor*> EwigfelsSpellEffects::TraceBeam(AActor* Caster, const FVector& Origin, const FVector& Direction, float Range, float Radius,
	int32 MaxTargets, FVector& OutEnd, FVector& OutNormal)
{
	TArray<AActor*> Out;
	const FVector Dir = Direction.GetSafeNormal();
	OutEnd = Origin + Dir * Range;
	OutNormal = -Dir;
	UWorld* World = Caster ? Caster->GetWorld() : nullptr;
	if (!World || Dir.IsNearlyZero())
	{
		return Out;
	}
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsBeam), false, Caster);

	// 1) Wand: dünne Linie, damit der Strahl nicht an Kanten hängen bleibt
	FHitResult Wall;
	float Length = Range;
	if (World->LineTraceSingleByChannel(Wall, Origin, OutEnd, ECC_Visibility, Params) && !Cast<APawn>(Wall.GetActor()))
	{
		Length = Wall.Distance;
		OutEnd = Wall.ImpactPoint;
		OutNormal = Wall.ImpactNormal;
	}

	// 2) Ziele: dicker Sweep bis zur Wand, sortiert nach Abstand
	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_Pawn);
	Objects.AddObjectTypesToQuery(ECC_PhysicsBody);
	TArray<FHitResult> Hits;
	World->SweepMultiByObjectType(Hits, Origin, Origin + Dir * Length, FQuat::Identity, Objects, FCollisionShape::MakeSphere(FMath::Max(1.f, Radius)), Params);
	Hits.Sort([](const FHitResult& A, const FHitResult& B) { return A.Distance < B.Distance; });
	for (const FHitResult& H : Hits)
	{
		AActor* A = H.GetActor();
		if (!A || A == Caster || Out.Contains(A) || !UEwigfelsCombatComponent::AreHostile(Caster, A))
		{
			continue;
		}
		const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(A);
		if (!Combat || Combat->IsDead())
		{
			continue;
		}
		Out.Add(A);
		if (Out.Num() >= FMath::Max(1, MaxTargets))
		{
			// Strahl endet im letzten Ziel (Brusthöhe statt Kapselrand sieht besser aus)
			OutEnd = FMath::ClosestPointOnSegment(A->GetActorLocation(), Origin, Origin + Dir * Length);
			OutNormal = -Dir;
			break;
		}
	}
	return Out;
}

void EwigfelsSpellEffects::ApplyEffects(const FEwigfelsSpellDef& Spell, const TArray<FEwigfelsSpellEffect>& Effects,
	const FEwigfelsSpellHit& Hit, AActor* Attacker)
{
	AActor* Target = Hit.Target;
	if (!Target || !Target->HasAuthority())
	{
		return;
	}
	UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(Target);

	FVector Push = Hit.Direction;
	Push.Z = 0.f;
	Push = Push.GetSafeNormal();
	if (Push.IsNearlyZero() && Hit.Caster)
	{
		Push = (Target->GetActorLocation() - Hit.Caster->GetActorLocation()).GetSafeNormal2D();
	}

	for (const FEwigfelsSpellEffect& Effect : Effects)
	{
		if (Effect.bChainEndOnly && !Hit.bChainEnd)
		{
			continue;
		}
		switch (Effect.Type)
		{
		case EEwigfelsSpellEffectType::Damage:
			if (Combat && !Combat->IsDead())
			{
				bool bConsume = false;
				FEwigfelsDamageInfo Info;
				Info.Amount = ComputeDamage(Effect, Combat, bConsume);
				Info.Element = Effect.Element;
				Info.Instigator = Hit.Caster;
				Info.Source = Spell.Id;
				Info.HitLocation = Hit.Location;
				if (bConsume)
				{
					Combat->RemoveStatus(Effect.Bonus.IfStatus);
				}
				const EEwigfelsHitResult Result = Combat->ApplyDamage(Info);
				if (Result == EEwigfelsHitResult::Parried)
				{
					return; // Parade schluckt den Rest der Weise (kein Status, kein Stoß)
				}
			}
			break;

		case EEwigfelsSpellEffectType::Status:
			if (Combat && !Combat->IsDead())
			{
				Combat->ApplyStatus(Effect.Status, Effect.Duration, Hit.Caster);
			}
			break;

		case EEwigfelsSpellEffectType::Knockback:
			if (Combat)
			{
				Combat->ApplyImpulse(Push * Effect.Amount + FVector::UpVector * Effect.Up);
			}
			break;

		case EEwigfelsSpellEffectType::Pull:
			if (Combat)
			{
				Combat->ApplyImpulse(-Push * Effect.Amount + FVector::UpVector * FMath::Max(Effect.Up, 150.f));
			}
			break;

		case EEwigfelsSpellEffectType::RestoreAtem:
			if (UEwigfelsSpellCasterComponent* Caster = Target->FindComponentByClass<UEwigfelsSpellCasterComponent>())
			{
				Caster->RestoreAtem(Effect.Amount);
			}
			break;

		case EEwigfelsSpellEffectType::Heal:
			if (Combat)
			{
				Combat->Heal(Effect.Amount);
			}
			break;

		case EEwigfelsSpellEffectType::StaggerAttacker:
			if (UEwigfelsCombatComponent* AttackerCombat = UEwigfelsCombatComponent::Find(Attacker))
			{
				AttackerCombat->Stagger(FMath::Max(0.2f, Effect.Amount));
			}
			break;
		}
	}
}
