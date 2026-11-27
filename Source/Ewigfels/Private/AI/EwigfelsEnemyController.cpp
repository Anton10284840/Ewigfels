// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "AI/EwigfelsEnemyController.h"
#include "AI/EwigfelsEnemy.h"
#include "AI/EwigfelsCreatureTypes.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Combat/EwigfelsCombatDirector.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

namespace
{
	constexpr float AlertDuration = 0.45f;
	constexpr float OrbitRadius = 420.f;
}

AEwigfelsEnemyController::AEwigfelsEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
	bSetControlRotationFromPawnOrientation = false;
}

void AEwigfelsEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Home = InPawn ? InPawn->GetActorLocation() : FVector::ZeroVector;
	StrafeSign = FMath::RandBool() ? 1.f : -1.f;
	NextTargetScan = 0.f;
	SetState(EEwigfelsEnemyState::Idle);
}

void AEwigfelsEnemyController::SetState(EEwigfelsEnemyState NewState)
{
	if (State == NewState)
	{
		return;
	}
	State = NewState;
	StateTime = 0.f;
	if (NewState != EEwigfelsEnemyState::Attack)
	{
		if (UEwigfelsCombatDirector* Director = GetWorld()->GetSubsystem<UEwigfelsCombatDirector>())
		{
			if (NewState == EEwigfelsEnemyState::Idle || NewState == EEwigfelsEnemyState::Dead)
			{
				Director->ReleaseMeleeToken(GetPawn());
			}
		}
	}
}

bool AEwigfelsEnemyController::HasLineOfSight(const AActor* Goal) const
{
	const APawn* Self = GetPawn();
	if (!Self || !Goal)
	{
		return false;
	}
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsEnemyLOS), false, Self);
	Params.AddIgnoredActor(Goal);
	FHitResult Hit;
	return !GetWorld()->LineTraceSingleByChannel(Hit, Self->GetActorLocation() + FVector(0, 0, 50), Goal->GetActorLocation() + FVector(0, 0, 40), ECC_Visibility, Params);
}

void AEwigfelsEnemyController::UpdateTarget()
{
	const AEwigfelsEnemy* Self = Cast<AEwigfelsEnemy>(GetPawn());
	const FEwigfelsCreatureDef* Def = Self ? Self->GetCreatureDef() : nullptr;
	if (!Def || Def->AggroRadius <= 0.f)
	{
		Target = nullptr;
		return;
	}
	const FVector Loc = Self->GetActorLocation();
	AActor* Current = Target.Get();
	const UEwigfelsCombatComponent* CurrentCombat = UEwigfelsCombatComponent::Find(Current);
	float CurrentDist = FLT_MAX;
	if (Current && CurrentCombat && !CurrentCombat->IsDead())
	{
		CurrentDist = FVector::Dist(Loc, Current->GetActorLocation());
		if (CurrentDist > Def->AggroRadius * 1.4f)
		{
			Current = nullptr;
			CurrentDist = FLT_MAX;
		}
	}
	else
	{
		Current = nullptr;
	}

	// Alle feindlichen Spielfiguren (Koop: mehrere) – nächste sichtbare
	AActor* Best = Current;
	float BestDist = CurrentDist;
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* Candidate = *It;
		if (!Candidate || Candidate == Self || !Candidate->IsPlayerControlled())
		{
			continue;
		}
		const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(Candidate);
		if (!Combat || Combat->IsDead() || !UEwigfelsCombatComponent::AreHostile(Self, Candidate))
		{
			continue;
		}
		const float Dist = FVector::Dist(Loc, Candidate->GetActorLocation());
		// Nur wechseln, wenn deutlich näher (verhindert Hin-und-Her im Koop)
		const float Threshold = Candidate == Current ? Dist : Dist + 250.f;
		if (Dist <= Def->AggroRadius && Threshold < BestDist && HasLineOfSight(Candidate))
		{
			Best = Candidate;
			BestDist = Dist;
		}
	}
	if (Best != Target.Get())
	{
		Target = Best;
		if (Best && State == EEwigfelsEnemyState::Idle)
		{
			SetState(EEwigfelsEnemyState::Alert);
		}
	}
}

void AEwigfelsEnemyController::SetEngagedRotation(bool bEngaged)
{
	ACharacter* Self = Cast<ACharacter>(GetPawn());
	if (!Self)
	{
		return;
	}
	UCharacterMovementComponent* Move = Self->GetCharacterMovement();
	Move->bOrientRotationToMovement = !bEngaged;
	Move->bUseControllerDesiredRotation = bEngaged;
	if (bEngaged && Target.IsValid())
	{
		SetFocus(Target.Get());
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}

void AEwigfelsEnemyController::SteerDirection(const FVector& Direction, float Scale)
{
	APawn* Self = GetPawn();
	if (!Self)
	{
		return;
	}
	if (GetPathFollowingComponent() && GetPathFollowingComponent()->GetStatus() != EPathFollowingStatus::Idle)
	{
		StopMovement();
	}
	FVector Dir = Direction.GetSafeNormal2D();
	// Einfaches Ausweichen vor Hindernissen: blockiert vorne → seitlich vorbei
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsEnemySteer), false, Self);
	const FVector From = Self->GetActorLocation();
	if (GetWorld()->SweepSingleByChannel(Hit, From, From + Dir * 150.f, FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(30.f), Params))
	{
		const FVector Side = FVector::CrossProduct(FVector::UpVector, Dir) * StrafeSign;
		Dir = (Dir * 0.2f + Side).GetSafeNormal();
	}
	Self->AddMovementInput(Dir, Scale);
}

void AEwigfelsEnemyController::SteerTowards(const FVector& Destination, float Scale)
{
	APawn* Self = GetPawn();
	if (!Self)
	{
		return;
	}
	// Mit Navmesh: echte Wegfindung (um Säulen herum); ohne: direkt steuern
	UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (Nav && Nav->GetDefaultNavDataInstance(FNavigationSystem::DontCreate))
	{
		// Pfad nur neu anfordern, wenn das Ziel spürbar gewandert ist (sonst ruckelt die Wegverfolgung)
		const UPathFollowingComponent* Path = GetPathFollowingComponent();
		const bool bFollowing = Path && Path->GetStatus() == EPathFollowingStatus::Moving;
		if (!bFollowing || FVector::DistSquared2D(Destination, LastMoveGoal) > FMath::Square(120.f))
		{
			LastMoveGoal = Destination;
			MoveToLocation(Destination, 60.f, true, true, true, true);
		}
		return;
	}
	SteerDirection(Destination - Self->GetActorLocation(), Scale);
}

bool AEwigfelsEnemyController::TryCast(FName SpellId, AActor* Goal)
{
	AEwigfelsEnemy* Self = Cast<AEwigfelsEnemy>(GetPawn());
	if (!Self || !Goal || Self->GetCaster()->CanCast(SpellId) != EEwigfelsCastFailReason::None)
	{
		return false;
	}
	StopMovement();
	return Self->GetCaster()->CastSpell(SpellId, Goal->GetActorLocation(), Goal) == EEwigfelsCastFailReason::None;
}

void AEwigfelsEnemyController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AEwigfelsEnemy* Self = Cast<AEwigfelsEnemy>(GetPawn());
	if (!Self)
	{
		return;
	}
	StateTime += DeltaSeconds;
	const UEwigfelsCombatComponent* Combat = Self->GetCombat();
	if (Combat->IsDead())
	{
		if (State != EEwigfelsEnemyState::Dead)
		{
			SetState(EEwigfelsEnemyState::Dead);
			StopMovement();
			ClearFocus(EAIFocusPriority::Gameplay);
		}
		return;
	}
	const FEwigfelsCreatureDef* Def = Self->GetCreatureDef();
	if (!Def || Def->Behavior == EEwigfelsCreatureBehavior::Dummy)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now >= NextTargetScan)
	{
		NextTargetScan = Now + 0.3f;
		UpdateTarget();
	}
	AActor* Goal = Target.Get();

	if (!Goal)
	{
		SetEngagedRotation(false);
		Self->SetRunning(false);
		if (FVector::Dist2D(Self->GetActorLocation(), Home) > 150.f)
		{
			SteerTowards(Home, 0.6f);
		}
		SetState(EEwigfelsEnemyState::Idle);
		return;
	}

	// Gefroren, betäubt, taumelnd, oder gerade am Ausholen: nichts entscheiden
	if (!Combat->CanAct() || Self->GetCaster()->IsCasting())
	{
		if (!Combat->CanAct())
		{
			StopMovement();
		}
		return;
	}

	if (State == EEwigfelsEnemyState::Alert)
	{
		SetEngagedRotation(true);
		if (StateTime < AlertDuration)
		{
			return;
		}
		SetState(EEwigfelsEnemyState::Engage);
	}
	if (State == EEwigfelsEnemyState::Attack || State == EEwigfelsEnemyState::Recover)
	{
		SetState(EEwigfelsEnemyState::Recover);
		if (StateTime < 0.35f)
		{
			return; // kurze Atempause nach jedem Angriff = Fenster für den Spieler
		}
		SetState(EEwigfelsEnemyState::Engage);
	}

	SetEngagedRotation(true);
	if (Def->Behavior == EEwigfelsCreatureBehavior::Melee)
	{
		TickMelee(DeltaSeconds, Self, Goal);
	}
	else
	{
		TickRanged(DeltaSeconds, Self, Goal);
	}
}

void AEwigfelsEnemyController::TickMelee(float DeltaSeconds, AEwigfelsEnemy* Self, AActor* Goal)
{
	const FEwigfelsCreatureDef* Def = Self->GetCreatureDef();
	UEwigfelsCombatDirector* Director = GetWorld()->GetSubsystem<UEwigfelsCombatDirector>();
	const FVector ToGoal = Goal->GetActorLocation() - Self->GetActorLocation();
	const float Dist = ToGoal.Size2D();
	const bool bHasToken = Director && Director->RequestMeleeToken(Self, Goal);

	if (bHasToken)
	{
		// Zweiter Angriff (Ansturm) aus mittlerer Entfernung, sonst Standardhieb aus der Nähe
		if (Def->Spells.Num() > 1 && Dist > Def->AttackRange + 40.f && Dist < 430.f && TryCast(Def->Spells[1], Goal))
		{
			SetState(EEwigfelsEnemyState::Attack);
			return;
		}
		if (Def->Spells.Num() > 0 && Dist <= Def->AttackRange + 20.f && TryCast(Def->Spells[0], Goal))
		{
			SetState(EEwigfelsEnemyState::Attack);
			return;
		}
		Self->SetRunning(Dist > 650.f);
		SteerTowards(Goal->GetActorLocation());
		return;
	}

	// Kein Platz frei: auf Abstand umkreisen, bedrohlich, aber ohne zu stören
	Self->SetRunning(Dist > 900.f);
	const FVector Radial = -ToGoal.GetSafeNormal2D();
	const FVector Tangent = FVector::CrossProduct(FVector::UpVector, Radial) * StrafeSign;
	const float RadiusError = (Dist - OrbitRadius) / OrbitRadius;
	SteerDirection(Tangent - Radial * FMath::Clamp(RadiusError * 2.f, -1.f, 1.f), 0.55f);
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now > NextStrafeFlip)
	{
		NextStrafeFlip = Now + FMath::FRandRange(2.f, 4.f);
		StrafeSign = -StrafeSign;
	}
}

void AEwigfelsEnemyController::TickRanged(float DeltaSeconds, AEwigfelsEnemy* Self, AActor* Goal)
{
	const FEwigfelsCreatureDef* Def = Self->GetCreatureDef();
	const FVector ToGoal = Goal->GetActorLocation() - Self->GetActorLocation();
	const float Dist = ToGoal.Size2D();
	const FVector Away = -ToGoal.GetSafeNormal2D();
	const bool bSees = HasLineOfSight(Goal);

	// Zu nah: Fluchkreis (stößt weg), sonst zurückweichen
	if (Dist < Def->FleeRange)
	{
		if (Def->Spells.Num() > 1 && TryCast(Def->Spells[1], Goal))
		{
			SetState(EEwigfelsEnemyState::Attack);
			return;
		}
		Self->SetRunning(true);
		SteerDirection(Away + FVector::CrossProduct(FVector::UpVector, Away) * StrafeSign * 0.5f);
		return;
	}

	if (bSees && Dist <= Def->AttackRange && Def->Spells.Num() > 0 && TryCast(Def->Spells[0], Goal))
	{
		SetState(EEwigfelsEnemyState::Attack);
		return;
	}

	Self->SetRunning(false);
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now > NextStrafeFlip)
	{
		NextStrafeFlip = Now + FMath::FRandRange(1.8f, 3.2f);
		StrafeSign = -StrafeSign;
	}
	const FVector Tangent = FVector::CrossProduct(FVector::UpVector, Away) * StrafeSign;
	if (!bSees || Dist > Def->PreferredRange + 300.f)
	{
		SteerTowards(Goal->GetActorLocation());
	}
	else if (Dist < Def->PreferredRange - 300.f)
	{
		SteerDirection(Away + Tangent * 0.6f, 0.8f);
	}
	else
	{
		SteerDirection(Tangent, 0.5f); // seitlich wandern = schwerer zu treffen
	}
}
