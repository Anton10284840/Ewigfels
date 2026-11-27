// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Character/EwigfelsLockOnComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UEwigfelsLockOnComponent::UEwigfelsLockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
	SetIsReplicatedByDefault(false);
}

bool UEwigfelsLockOnComponent::GetView(FVector& OutLoc, FRotator& OutRot) const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	const APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (!PC || !PC->IsLocalController())
	{
		return false;
	}
	PC->GetPlayerViewPoint(OutLoc, OutRot);
	return true;
}

TArray<AActor*> UEwigfelsLockOnComponent::GatherCandidates() const
{
	TArray<AActor*> Out;
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return Out;
	}
	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams Objects(ECC_Pawn);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsLockOn), false, Owner);
	World->OverlapMultiByObjectType(Overlaps, Owner->GetActorLocation(), FQuat::Identity, Objects, FCollisionShape::MakeSphere(MaxDistance), Params);
	for (const FOverlapResult& O : Overlaps)
	{
		AActor* A = O.GetActor();
		const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(A);
		if (A && !Out.Contains(A) && Combat && !Combat->IsDead() && UEwigfelsCombatComponent::AreHostile(Owner, A))
		{
			FHitResult Hit;
			FCollisionQueryParams LOS(SCENE_QUERY_STAT(EwigfelsLockOnLOS), false, Owner);
			LOS.AddIgnoredActor(A);
			if (!World->LineTraceSingleByChannel(Hit, Owner->GetActorLocation() + FVector(0, 0, 60), A->GetActorLocation(), ECC_Visibility, LOS))
			{
				Out.Add(A);
			}
		}
	}
	return Out;
}

float UEwigfelsLockOnComponent::Score(const AActor* Candidate, const FVector& ViewLoc, const FVector& ViewDir) const
{
	const FVector To = Candidate->GetActorLocation() - ViewLoc;
	const float Dist = To.Size();
	const float Dot = FVector::DotProduct(ViewDir, To / FMath::Max(Dist, 1.f));
	// Bildmitte zählt mehr als Nähe
	return Dot * 2.f - Dist / MaxDistance;
}

AActor* UEwigfelsLockOnComponent::FindSoftTarget(float MaxAngleDeg) const
{
	FVector ViewLoc;
	FRotator ViewRot;
	if (!GetView(ViewLoc, ViewRot))
	{
		return nullptr;
	}
	const FVector Dir = ViewRot.Vector();
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(MaxAngleDeg));
	AActor* Best = nullptr;
	float BestScore = -FLT_MAX;
	for (AActor* A : GatherCandidates())
	{
		const FVector To = (A->GetActorLocation() - ViewLoc).GetSafeNormal();
		if (FVector::DotProduct(Dir, To) < MinDot)
		{
			continue;
		}
		const float S = Score(A, ViewLoc, Dir);
		if (S > BestScore)
		{
			BestScore = S;
			Best = A;
		}
	}
	return Best;
}

void UEwigfelsLockOnComponent::SetTarget(AActor* NewTarget)
{
	if (Target.Get() == NewTarget)
	{
		return;
	}
	Target = NewTarget;
	OnTargetChanged.Broadcast(NewTarget);
}

void UEwigfelsLockOnComponent::Toggle()
{
	if (Target.IsValid())
	{
		Release();
		return;
	}
	// Erst im Bild (60°), sonst irgendwo in der Nähe
	AActor* Best = FindSoftTarget(60.f);
	if (!Best)
	{
		Best = FindSoftTarget(180.f);
	}
	SetTarget(Best);
}

void UEwigfelsLockOnComponent::Switch(int32 Direction)
{
	FVector ViewLoc;
	FRotator ViewRot;
	AActor* Current = Target.Get();
	if (!Current || !GetView(ViewLoc, ViewRot))
	{
		return;
	}
	const FVector Right = FRotationMatrix(ViewRot).GetUnitAxis(EAxis::Y);
	const float CurrentSide = FVector::DotProduct(Right, (Current->GetActorLocation() - ViewLoc).GetSafeNormal());
	AActor* Best = nullptr;
	float BestDelta = FLT_MAX;
	for (AActor* A : GatherCandidates())
	{
		if (A == Current)
		{
			continue;
		}
		const float Side = FVector::DotProduct(Right, (A->GetActorLocation() - ViewLoc).GetSafeNormal());
		const float Delta = (Side - CurrentSide) * Direction;
		if (Delta > 0.f && Delta < BestDelta)
		{
			BestDelta = Delta;
			Best = A;
		}
	}
	if (Best)
	{
		SetTarget(Best);
	}
}

void UEwigfelsLockOnComponent::Release()
{
	SetTarget(nullptr);
}

void UEwigfelsLockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Current = Target.Get();
	if (!Current)
	{
		if (Target.IsStale())
		{
			Target.Reset();
			OnTargetChanged.Broadcast(nullptr);
		}
		return;
	}
	const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(Current);
	if (!Combat || Combat->IsDead() || FVector::Dist(Current->GetActorLocation(), GetOwner()->GetActorLocation()) > BreakDistance)
	{
		// Ziel tot: automatisch zum nächsten springen, damit der Kampffluss nicht reißt
		Target = nullptr;
		AActor* Next = FindSoftTarget(70.f);
		SetTarget(Next);
		if (!Next)
		{
			OnTargetChanged.Broadcast(nullptr);
		}
		return;
	}

	// Kamera weich auf das Ziel drehen (etwas über die Mitte, leicht nach unten blickend)
	APawn* Pawn = Cast<APawn>(GetOwner());
	APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	if (PC && PC->IsLocalController())
	{
		const FVector From = Pawn->GetActorLocation() + FVector(0.f, 0.f, 70.f);
		FRotator Desired = (Current->GetActorLocation() - From).Rotation();
		Desired.Pitch = FMath::Clamp(Desired.Pitch - 12.f, -45.f, 20.f);
		const FRotator NewRot = FMath::RInterpTo(PC->GetControlRotation(), Desired, DeltaTime, CameraInterpSpeed);
		PC->SetControlRotation(FRotator(NewRot.Pitch, NewRot.Yaw, 0.f));
	}
}
