// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Magic/EwigfelsSpellEffects.h"
#include "Magic/EwigfelsSpellProjectile.h"
#include "Magic/EwigfelsSpellSubsystem.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Combat/EwigfelsForcedMotionComponent.h"
#include "Ewigfels.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

namespace
{
	/** Finger-Knochen des UE5-Mannequins/MetaHumans: Magie kommt aus der Zeigefingerspitze. */
	const FName FingerRight(TEXT("index_03_r"));
	const FName FingerLeft(TEXT("index_03_l"));
	const FName HandRight(TEXT("hand_r"));
	const FName HandLeft(TEXT("hand_l"));

	/** Wie weit der Zielpunkt des Clients maximal vom Wirker weg sein darf (Schutz gegen Unsinn). */
	constexpr float MaxAimDistance = 20000.f;
}

UEwigfelsSpellCasterComponent::UEwigfelsSpellCasterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UEwigfelsSpellCasterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEwigfelsSpellCasterComponent, Atem);
	DOREPLIFETIME(UEwigfelsSpellCasterComponent, MaxAtem);
	DOREPLIFETIME(UEwigfelsSpellCasterComponent, Loadout);
	DOREPLIFETIME_CONDITION(UEwigfelsSpellCasterComponent, Cooldowns, COND_OwnerOnly);
	DOREPLIFETIME(UEwigfelsSpellCasterComponent, CastState);
	DOREPLIFETIME(UEwigfelsSpellCasterComponent, bShieldUp);
	DOREPLIFETIME(UEwigfelsSpellCasterComponent, ShieldUpTime);
	DOREPLIFETIME_CONDITION(UEwigfelsSpellCasterComponent, RecastSpell, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UEwigfelsSpellCasterComponent, RecastUntil, COND_OwnerOnly);
}

void UEwigfelsSpellCasterComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (Owner && Owner->HasAuthority())
	{
		MaxAtem = DefaultMaxAtem;
		Atem = MaxAtem;
		if (Loadout.Num() == 0)
		{
			if (const UEwigfelsSpellSubsystem* Spells = GetSpells())
			{
				Loadout = Spells->GetDefaultLoadout();
			}
		}
	}
	if (UEwigfelsCombatComponent* Combat = GetCombat())
	{
		Combat->DamageInterceptor.BindUObject(this, &UEwigfelsSpellCasterComponent::InterceptDamage);
	}
	OnAtemChanged.Broadcast(Atem, MaxAtem);
}

void UEwigfelsSpellCasterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UEwigfelsCombatComponent* Combat = GetCombat())
	{
		Combat->DamageInterceptor.Unbind();
	}
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ReleaseTimer);
	}
	Super::EndPlay(EndPlayReason);
}

// ---------------------------------------------------------------------------
// Hilfen

float UEwigfelsSpellCasterComponent::Now() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0.f;
	}
	const AGameStateBase* GS = World->GetGameState();
	return GS ? static_cast<float>(GS->GetServerWorldTimeSeconds()) : World->GetTimeSeconds();
}

UEwigfelsCombatComponent* UEwigfelsSpellCasterComponent::GetCombat() const
{
	return UEwigfelsCombatComponent::Find(GetOwner());
}

UEwigfelsSpellSubsystem* UEwigfelsSpellCasterComponent::GetSpells() const
{
	const UWorld* World = GetWorld();
	UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
}

const FEwigfelsSpellDef* UEwigfelsSpellCasterComponent::FindSpellDef(FName SpellId) const
{
	const UEwigfelsSpellSubsystem* Spells = GetSpells();
	return Spells ? Spells->FindSpell(SpellId) : nullptr;
}

bool UEwigfelsSpellCasterComponent::IsLocallyControlledOwner() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	return Pawn && Pawn->IsLocallyControlled() && Pawn->IsPlayerControlled();
}

FVector UEwigfelsSpellCasterComponent::GetCastOrigin(bool bTwoHanded) const
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return FVector::ZeroVector;
	}
	if (const ACharacter* Character = Cast<ACharacter>(Owner))
	{
		const USkeletalMeshComponent* Mesh = Character->GetMesh();
		if (Mesh && Mesh->GetSkeletalMeshAsset())
		{
			const FName Right = Mesh->DoesSocketExist(FingerRight) ? FingerRight : HandRight;
			const FName Left = Mesh->DoesSocketExist(FingerLeft) ? FingerLeft : HandLeft;
			if (Mesh->DoesSocketExist(Right))
			{
				const FVector R = Mesh->GetSocketLocation(Right);
				if (bTwoHanded && Mesh->DoesSocketExist(Left))
				{
					return (R + Mesh->GetSocketLocation(Left)) * 0.5f;
				}
				return R;
			}
		}
	}
	// Platzhalter-Körper: Komponenten mit Tag "CastOriginR" / "CastOriginL"
	const FName Tag = bTwoHanded ? FName(TEXT("CastOriginBoth")) : FName(TEXT("CastOriginR"));
	TArray<UActorComponent*> Tagged = Owner->GetComponentsByTag(USceneComponent::StaticClass(), Tag);
	if (Tagged.Num() == 0 && bTwoHanded)
	{
		Tagged = Owner->GetComponentsByTag(USceneComponent::StaticClass(), TEXT("CastOriginR"));
	}
	if (Tagged.Num() > 0)
	{
		return CastChecked<USceneComponent>(Tagged[0])->GetComponentLocation();
	}
	return Owner->GetActorLocation() + Owner->GetActorForwardVector() * 60.f + FVector(0.f, 0.f, 40.f);
}

FVector UEwigfelsSpellCasterComponent::ComputeLocalAimPoint() const
{
	if (AimPointProvider)
	{
		return AimPointProvider();
	}
	const AActor* Owner = GetOwner();
	return Owner ? Owner->GetActorLocation() + Owner->GetActorForwardVector() * 3000.f : FVector::ZeroVector;
}

float UEwigfelsSpellCasterComponent::GetCooldownRemaining(FName SpellId) const
{
	const FEwigfelsSpellCooldown* Cd = Cooldowns.FindByPredicate([SpellId](const FEwigfelsSpellCooldown& C) { return C.SpellId == SpellId; });
	return Cd ? FMath::Max(0.f, Cd->ReadyTime - Now()) : 0.f;
}

bool UEwigfelsSpellCasterComponent::IsCasting() const
{
	const float T = Now();
	return !CastState.SpellId.IsNone() && T < CastState.BusyUntil;
}

uint8 UEwigfelsSpellCasterComponent::NextChainIndex(const FEwigfelsSpellDef& Spell, FName LastSpell, uint8 LastIndex, float LastReleaseTime, float NowTime)
{
	if (Spell.Delivery.Chain <= 1 || LastSpell != Spell.Id)
	{
		return 0;
	}
	if (NowTime - LastReleaseTime > Spell.Delivery.ChainWindow)
	{
		return 0;
	}
	const int32 Next = LastIndex + 1;
	return Next >= Spell.Delivery.Chain ? 0 : static_cast<uint8>(Next);
}

EEwigfelsCastFailReason UEwigfelsSpellCasterComponent::CanCast(FName SpellId) const
{
	const FEwigfelsSpellDef* Spell = FindSpellDef(SpellId);
	if (!Spell)
	{
		return EEwigfelsCastFailReason::UnknownSpell;
	}
	const UEwigfelsCombatComponent* Combat = GetCombat();
	if (Combat && !Combat->CanAct())
	{
		return EEwigfelsCastFailReason::CannotAct;
	}
	if (Now() < CastState.BusyUntil)
	{
		return EEwigfelsCastFailReason::Busy;
	}
	if (IsRecastReady(SpellId))
	{
		return EEwigfelsCastFailReason::None; // Zweitwirkung: ohne Abklingzeit und Atem
	}
	if (GetCooldownRemaining(SpellId) > 0.f)
	{
		return EEwigfelsCastFailReason::Cooldown;
	}
	if (Atem + KINDA_SMALL_NUMBER < Spell->Cost)
	{
		return EEwigfelsCastFailReason::NoAtem;
	}
	return EEwigfelsCastFailReason::None;
}

// ---------------------------------------------------------------------------
// Lokaler Spieler → Server

void UEwigfelsSpellCasterComponent::RequestCastSlot(int32 SlotIndex)
{
	if (SlotIndex == 0)
	{
		RequestCast(BasicSpell);
		return;
	}
	// Slot 1–4 = Loadout ohne Basis- und Schild-Weise
	int32 Slot = 0;
	for (const FName Id : Loadout)
	{
		if (Id == BasicSpell || Id == ShieldSpell)
		{
			continue;
		}
		if (++Slot == SlotIndex)
		{
			RequestCast(Id);
			return;
		}
	}
}

void UEwigfelsSpellCasterComponent::RequestCast(FName SpellId)
{
	const EEwigfelsCastFailReason Reason = CanCast(SpellId);
	if (Reason != EEwigfelsCastFailReason::None)
	{
		// Busy bei Ketten-Weisen nicht melden: Spieler hämmert die Taste, das ist gewollt
		if (Reason != EEwigfelsCastFailReason::Busy)
		{
			OnCastFailed.Broadcast(SpellId, Reason);
		}
		return;
	}
	const FVector AimPoint = ComputeLocalAimPoint();
	AActor* Target = AimTarget.Get();

	// Vorhersage: eigener Client spielt Ausholen sofort (nicht auf dem Server, der macht das in CastSpell)
	if (!GetOwner()->HasAuthority())
	{
		if (const FEwigfelsSpellDef* Spell = FindSpellDef(SpellId))
		{
			const uint8 Chain = NextChainIndex(*Spell, CastState.SpellId, CastState.ChainIndex, CastState.ReleaseTime, Now());
			bHasPrediction = true;
			PredictedSerial = CastState.Serial + 1;
			OnCastStarted.Broadcast(SpellId, Chain, Spell->CastTime);
		}
	}
	Server_Cast(SpellId, AimPoint, Target);
}

void UEwigfelsSpellCasterComponent::Server_Cast_Implementation(FName SpellId, FVector_NetQuantize AimPoint, AActor* Target)
{
	const EEwigfelsCastFailReason Reason = CastSpell(SpellId, AimPoint, Target);
	if (Reason != EEwigfelsCastFailReason::None)
	{
		Client_CastRejected(SpellId, Reason);
	}
}

void UEwigfelsSpellCasterComponent::Client_CastRejected_Implementation(FName SpellId, EEwigfelsCastFailReason Reason)
{
	bHasPrediction = false;
	if (Reason != EEwigfelsCastFailReason::Busy)
	{
		OnCastFailed.Broadcast(SpellId, Reason);
	}
}

void UEwigfelsSpellCasterComponent::RequestShield(bool bUp)
{
	if (bUp == bShieldUp)
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		OnShieldChanged.Broadcast(bUp); // Vorhersage für Animation
	}
	Server_SetShield(bUp);
}

void UEwigfelsSpellCasterComponent::Server_SetShield_Implementation(bool bUp)
{
	SetShieldServer(bUp);
}

// ---------------------------------------------------------------------------
// Server: Wirken

EEwigfelsCastFailReason UEwigfelsSpellCasterComponent::CastSpell(FName SpellId, FVector AimPoint, AActor* Target)
{
	return CastSpellInternal(SpellId, AimPoint, Target, false);
}

EEwigfelsCastFailReason UEwigfelsSpellCasterComponent::CastSpellInternal(FName SpellId, FVector AimPoint, AActor* Target, bool bSkipLoadoutCheck)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return EEwigfelsCastFailReason::CannotAct;
	}
	const FEwigfelsSpellDef* Spell = FindSpellDef(SpellId);
	if (!Spell)
	{
		return EEwigfelsCastFailReason::UnknownSpell;
	}
	// Spieler dürfen nur Weisen aus ihrer Belegung wirken; KI und Konter rufen CastSpell direkt (ohne Server_Cast)
	const APawn* Pawn = Cast<APawn>(Owner);
	if (!bSkipLoadoutCheck && Pawn && Pawn->IsPlayerControlled() && !Loadout.Contains(SpellId) && SpellId != BasicSpell)
	{
		return EEwigfelsCastFailReason::NotInLoadout;
	}
	if (Spell->Delivery.Type == EEwigfelsSpellDelivery::Shield)
	{
		SetShieldServer(true);
		return EEwigfelsCastFailReason::None;
	}
	const EEwigfelsCastFailReason Reason = CanCast(SpellId);
	if (Reason != EEwigfelsCastFailReason::None)
	{
		return Reason;
	}
	if (IsRecastReady(SpellId) && TryRecast(*Spell))
	{
		return EEwigfelsCastFailReason::None;
	}

	// Schild senkt sich beim Wirken
	if (bShieldUp)
	{
		SetShieldServer(false);
	}

	const float T = Now();
	const float Speed = GetCombat() ? GetCombat()->GetActionSpeedMultiplier() : 1.f;
	const float CastTime = Spell->CastTime / Speed;
	const float Recovery = Spell->Recovery / Speed;

	SpendAtem(Spell->Cost);
	if (Spell->Cooldown > 0.f)
	{
		FEwigfelsSpellCooldown* Cd = Cooldowns.FindByPredicate([SpellId](const FEwigfelsSpellCooldown& C) { return C.SpellId == SpellId; });
		if (!Cd)
		{
			Cd = &Cooldowns.AddDefaulted_GetRef();
			Cd->SpellId = SpellId;
		}
		Cd->ReadyTime = T + Spell->Cooldown;
	}

	const uint8 Chain = NextChainIndex(*Spell, CastState.SpellId, CastState.ChainIndex, CastState.ReleaseTime, T);
	CastState.SpellId = SpellId;
	CastState.StartTime = T;
	CastState.ReleaseTime = T + CastTime;
	CastState.BusyUntil = T + CastTime + Recovery;
	CastState.ChainIndex = Chain;
	CastState.Serial++;

	const FVector Origin = GetCastOrigin(Spell->bTwoHanded);
	if (FVector::DistSquared(Origin, AimPoint) > FMath::Square(MaxAimDistance))
	{
		AimPoint = Origin + (AimPoint - Origin).GetSafeNormal() * MaxAimDistance;
	}
	PendingSpell = SpellId;
	PendingAimPoint = AimPoint;
	PendingTarget = Target;

	OnCastStarted.Broadcast(SpellId, Chain, CastTime);

	if (CastTime <= KINDA_SMALL_NUMBER)
	{
		ReleaseSpell(SpellId, Chain);
	}
	else
	{
		FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &UEwigfelsSpellCasterComponent::ReleaseSpell, SpellId, Chain);
		GetWorld()->GetTimerManager().SetTimer(ReleaseTimer, Delegate, CastTime, false);
	}
	return EEwigfelsCastFailReason::None;
}

void UEwigfelsSpellCasterComponent::InterruptCast()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	if (GetWorld()->GetTimerManager().IsTimerActive(ReleaseTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(ReleaseTimer);
		CastState.BusyUntil = Now();
		CastState.SpellId = NAME_None;
		CastState.Serial++;
		PendingSpell = NAME_None;
	}
}

void UEwigfelsSpellCasterComponent::ReleaseSpell(FName SpellId, uint8 ChainIndex)
{
	const FEwigfelsSpellDef* Spell = FindSpellDef(SpellId);
	const UEwigfelsCombatComponent* Combat = GetCombat();
	if (!Spell || PendingSpell != SpellId || (Combat && Combat->IsDead()))
	{
		return;
	}
	PendingSpell = NAME_None;

	const FVector Origin = GetCastOrigin(Spell->bTwoHanded);
	// Zielt das Lock-On-Ziel noch? Dann darauf (Brusthöhe), sonst auf den Zielpunkt
	FVector AimPoint = PendingAimPoint;
	if (const AActor* Target = PendingTarget.Get())
	{
		AimPoint = Target->GetActorLocation();
	}
	FVector Direction = (AimPoint - Origin).GetSafeNormal();
	if (Direction.IsNearlyZero())
	{
		Direction = GetOwner()->GetActorForwardVector();
	}
	ExecuteDelivery(*Spell, ChainIndex, Origin, Direction);
}

void UEwigfelsSpellCasterComponent::ExecuteDelivery(const FEwigfelsSpellDef& Spell, uint8 ChainIndex, const FVector& Origin, const FVector& Direction)
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	const bool bChainEnd = ChainIndex + 1 >= Spell.Delivery.Chain;

	switch (Spell.Delivery.Type)
	{
	case EEwigfelsSpellDelivery::Projectile:
	{
		const FTransform SpawnTM(Direction.Rotation(), Origin);
		AEwigfelsSpellProjectile* Projectile = World->SpawnActorDeferred<AEwigfelsSpellProjectile>(
			AEwigfelsSpellProjectile::StaticClass(), SpawnTM, Owner, Cast<APawn>(Owner), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Projectile)
		{
			Projectile->InitSpell(Spell.Id, bChainEnd, PendingTarget.Get());
			UGameplayStatics::FinishSpawningActor(Projectile, SpawnTM);
		}
		break;
	}
	case EEwigfelsSpellDelivery::Cone:
	{
		const FVector Flat = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
		const FVector ConeOrigin = Owner->GetActorLocation();
		for (AActor* Target : EwigfelsSpellEffects::GatherCone(Owner, ConeOrigin, Flat, Spell.Delivery.Range, Spell.Delivery.Angle))
		{
			FEwigfelsSpellHit Hit;
			Hit.Caster = Owner;
			Hit.Target = Target;
			Hit.Location = Target->GetActorLocation();
			Hit.Direction = (Target->GetActorLocation() - ConeOrigin).GetSafeNormal2D();
			Hit.bChainEnd = bChainEnd;
			EwigfelsSpellEffects::ApplyEffects(Spell, Spell.Effects, Hit);
		}
		break;
	}
	case EEwigfelsSpellDelivery::Area:
	{
		const FVector Center = Owner->GetActorLocation();
		for (AActor* Target : EwigfelsSpellEffects::GatherSphere(Owner, Center, Spell.Delivery.Radius))
		{
			FEwigfelsSpellHit Hit;
			Hit.Caster = Owner;
			Hit.Target = Target;
			Hit.Location = Target->GetActorLocation();
			Hit.Direction = (Target->GetActorLocation() - Center).GetSafeNormal2D();
			EwigfelsSpellEffects::ApplyEffects(Spell, Spell.Effects, Hit);
		}
		break;
	}
	case EEwigfelsSpellDelivery::Self:
	{
		FEwigfelsSpellHit Hit;
		Hit.Caster = Owner;
		Hit.Target = Owner;
		Hit.Location = Owner->GetActorLocation();
		EwigfelsSpellEffects::ApplyEffects(Spell, Spell.Effects, Hit);
		break;
	}
	case EEwigfelsSpellDelivery::Beam:
	{
		FVector End;
		FVector Normal;
		const TArray<AActor*> Targets = EwigfelsSpellEffects::TraceBeam(Owner, Origin, Direction, Spell.Delivery.Range,
			Spell.Delivery.Radius, 1 + Spell.Delivery.Pierce, End, Normal);
		for (AActor* Target : Targets)
		{
			FEwigfelsSpellHit Hit;
			Hit.Caster = Owner;
			Hit.Target = Target;
			Hit.Location = FMath::ClosestPointOnSegment(Target->GetActorLocation(), Origin, End);
			Hit.Direction = Direction;
			Hit.bChainEnd = bChainEnd;
			EwigfelsSpellEffects::ApplyEffects(Spell, Spell.Effects, Hit);
			ApplyHitStop(Target, bChainEnd ? 0.09f : 0.05f);
		}
		Multicast_BeamFired(Spell.Id, Origin, End, Normal, Targets.Num() > 0, ChainIndex);
		if (Spell.Delivery.Bounces > 0 && Targets.Num() > 0)
		{
			BounceBeam(Spell, Targets.Last(), Targets);
		}
		break;
	}
	case EEwigfelsSpellDelivery::Shield:
		break;
	}

	Multicast_SpellReleased(Spell.Id, Origin, Direction);
}

void UEwigfelsSpellCasterComponent::BounceBeam(const FEwigfelsSpellDef& Spell, AActor* FirstTarget, const TArray<AActor*>& AlreadyHit)
{
	// Kettenblitz: vom Ziel zum nächsten noch nicht getroffenen Gegner springen (Sturmweise)
	AActor* Owner = GetOwner();
	TArray<AActor*> Hit = AlreadyHit;
	AActor* From = FirstTarget;
	for (int32 Jump = 0; Jump < Spell.Delivery.Bounces && From; ++Jump)
	{
		const FVector FromLoc = From->GetActorLocation();
		AActor* Best = nullptr;
		float BestDist = TNumericLimits<float>::Max();
		for (AActor* Candidate : EwigfelsSpellEffects::GatherSphere(Owner, FromLoc, Spell.Delivery.BounceRange, From))
		{
			const float D = FVector::DistSquared(FromLoc, Candidate->GetActorLocation());
			if (!Hit.Contains(Candidate) && D < BestDist)
			{
				Best = Candidate;
				BestDist = D;
			}
		}
		if (!Best)
		{
			break;
		}
		const FVector ToLoc = Best->GetActorLocation();
		FEwigfelsSpellHit HitInfo;
		HitInfo.Caster = Owner;
		HitInfo.Target = Best;
		HitInfo.Location = ToLoc;
		HitInfo.Direction = (ToLoc - FromLoc).GetSafeNormal();
		EwigfelsSpellEffects::ApplyEffects(Spell, Spell.Effects, HitInfo);
		ApplyHitStop(Best, 0.05f);
		Multicast_BeamFired(Spell.Id, FromLoc, ToLoc, -HitInfo.Direction, true, 0);
		Hit.Add(Best);
		From = Best;
	}
}

bool UEwigfelsSpellCasterComponent::IsRecastReady(FName SpellId) const
{
	return !SpellId.IsNone() && RecastSpell == SpellId && Now() < RecastUntil;
}

void UEwigfelsSpellCasterComponent::NotifyHeldTarget(FName SpellId, AActor* Target, float Seconds)
{
	const FEwigfelsSpellDef* Spell = FindSpellDef(SpellId);
	if (!Spell || Spell->RecastWindow <= 0.f || !GetOwner()->HasAuthority())
	{
		return;
	}
	if (RecastSpell != SpellId || Now() >= RecastUntil)
	{
		RecastTargets.Reset();
	}
	RecastTargets.AddUnique(Target);
	RecastSpell = SpellId;
	RecastUntil = Now() + FMath::Min(Seconds, Spell->RecastWindow);
}

bool UEwigfelsSpellCasterComponent::TryRecast(const FEwigfelsSpellDef& Spell)
{
	AActor* Owner = GetOwner();
	TArray<AActor*> Targets;
	for (const TWeakObjectPtr<AActor>& Weak : RecastTargets)
	{
		AActor* A = Weak.Get();
		const UEwigfelsForcedMotionComponent* Motion = A ? A->FindComponentByClass<UEwigfelsForcedMotionComponent>() : nullptr;
		if (Motion && Motion->IsHeldBy(Owner, Spell.Id))
		{
			Targets.Add(A);
		}
	}
	RecastSpell = NAME_None;
	RecastUntil = 0.f;
	RecastTargets.Reset();
	if (Targets.Num() == 0)
	{
		return false; // niemand mehr gehalten → normales Wirken
	}

	const float T = Now();
	CastState.SpellId = Spell.Id;
	CastState.StartTime = T;
	CastState.ReleaseTime = T;
	CastState.BusyUntil = T + Spell.Recovery;
	CastState.ChainIndex = 1; // 1 = Zweitwirkung (Animation: Schmettern)
	CastState.Serial++;
	OnCastStarted.Broadcast(Spell.Id, 1, 0.f);
	for (AActor* Target : Targets)
	{
		FEwigfelsSpellHit Hit;
		Hit.Caster = Owner;
		Hit.Target = Target;
		Hit.Location = Target->GetActorLocation();
		Hit.Direction = (Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal2D();
		EwigfelsSpellEffects::ApplyEffects(Spell, Spell.RecastEffects, Hit);
	}
	return true;
}

void UEwigfelsSpellCasterComponent::BroadcastImpactFx(FName SpellId, const FVector& Location, const FVector& Normal, bool bBig)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Multicast_ImpactFx(SpellId, Location, Normal, bBig);
	}
}

void UEwigfelsSpellCasterComponent::Multicast_ImpactFx_Implementation(FName SpellId, FVector_NetQuantize Location,
	FVector_NetQuantizeNormal Normal, bool bBig)
{
	if (const FEwigfelsSpellDef* Spell = FindSpellDef(SpellId))
	{
		EwigfelsSpellVisuals::PlayLandImpact(GetWorld(), *Spell, Location, bBig);
		OnImpactFx.Broadcast(SpellId, Location, Normal, bBig);
	}
}

void UEwigfelsSpellCasterComponent::Multicast_SpellReleased_Implementation(FName SpellId, FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction)
{
	OnSpellReleased.Broadcast(SpellId, Origin, Direction);
}

void UEwigfelsSpellCasterComponent::ApplyHitStop(AActor* Target, float Seconds)
{
	// Treff-Stopp: das getroffene Ziel friert kurz ein (Wucht-Gefühl). Nur für KI-Ziele auf dem Server,
	// Spielerfiguren nicht (deren Bewegung sagt der Client voraus, ein Stopp nur auf dem Server würde ruckeln).
	const APawn* Pawn = Cast<APawn>(Target);
	if (!Target || !GetOwner()->HasAuthority() || (Pawn && Pawn->IsPlayerControlled()) || Seconds <= 0.f)
	{
		return;
	}
	Target->CustomTimeDilation = 0.03f;
	FTimerHandle Handle;
	TWeakObjectPtr<AActor> Weak(Target);
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([Weak]()
	{
		if (AActor* A = Weak.Get())
		{
			A->CustomTimeDilation = 1.f;
		}
	}), Seconds, false);
}

void UEwigfelsSpellCasterComponent::Multicast_BeamFired_Implementation(FName SpellId, FVector_NetQuantize Start, FVector_NetQuantize End,
	FVector_NetQuantizeNormal Normal, bool bHitTarget, uint8 ChainIndex)
{
	if (const FEwigfelsSpellDef* Spell = FindSpellDef(SpellId))
	{
		// Start lokal an den Fingern neu bestimmen: auf Clients steht die Hand minimal anders als auf dem Server
		const FVector LocalStart = GetOwner() ? GetCastOrigin(Spell->bTwoHanded) : FVector(Start);
		const FVector From = FVector::DistSquared(LocalStart, Start) < FMath::Square(80.f) ? LocalStart : FVector(Start);
		EwigfelsSpellVisuals::PlayBeam(GetWorld(), *Spell, From, End, Normal, bHitTarget, ChainIndex);
		OnBeamFired.Broadcast(SpellId, From, End, bHitTarget, ChainIndex);
	}
}

void UEwigfelsSpellCasterComponent::OnRep_CastState()
{
	if (CastState.SpellId.IsNone())
	{
		bHasPrediction = false;
		return;
	}
	if (bHasPrediction && IsLocallyControlledOwner())
	{
		bHasPrediction = false;
		if (CastState.Serial == PredictedSerial)
		{
			return; // schon lokal gespielt
		}
	}
	const FEwigfelsSpellDef* Spell = FindSpellDef(CastState.SpellId);
	OnCastStarted.Broadcast(CastState.SpellId, CastState.ChainIndex, Spell ? CastState.ReleaseTime - CastState.StartTime : 0.f);
}

// ---------------------------------------------------------------------------
// Atem

void UEwigfelsSpellCasterComponent::SpendAtem(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}
	Atem = FMath::Max(0.f, Atem - Amount);
	LastSpendTime = Now();
	OnAtemChanged.Broadcast(Atem, MaxAtem);
}

void UEwigfelsSpellCasterComponent::RestoreAtem(float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	Atem = FMath::Min(MaxAtem, Atem + Amount);
	OnAtemChanged.Broadcast(Atem, MaxAtem);
}

bool UEwigfelsSpellCasterComponent::TrySpendAtem(float Amount)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || Atem + KINDA_SMALL_NUMBER < Amount)
	{
		return false;
	}
	SpendAtem(Amount);
	return true;
}

void UEwigfelsSpellCasterComponent::OnRep_Atem()
{
	OnAtemChanged.Broadcast(Atem, MaxAtem);
}

void UEwigfelsSpellCasterComponent::SetLoadout(const TArray<FName>& NewLoadout)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Loadout = NewLoadout;
	}
}

void UEwigfelsSpellCasterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}
	const UEwigfelsCombatComponent* Combat = GetCombat();
	if (Combat && Combat->IsDead())
	{
		return;
	}
	// Atem lädt nach kurzer Pause nach, nicht bei erhobenem Schild
	if (!bShieldUp && Atem < MaxAtem && Now() - LastSpendTime >= AtemRegenDelay)
	{
		const float Mult = Combat ? Combat->GetAtemRegenMultiplier() : 1.f;
		Atem = FMath::Min(MaxAtem, Atem + AtemRegenPerSecond * Mult * DeltaTime);
		OnAtemChanged.Broadcast(Atem, MaxAtem);
	}
	// Abgelaufene Abklingzeiten aufräumen (spart Replikation)
	const float T = Now();
	Cooldowns.RemoveAll([T](const FEwigfelsSpellCooldown& C) { return C.ReadyTime <= T; });
}

// ---------------------------------------------------------------------------
// Schild und Parade (Reinklang)

void UEwigfelsSpellCasterComponent::SetShieldServer(bool bUp)
{
	if (bUp)
	{
		const UEwigfelsCombatComponent* Combat = GetCombat();
		if ((Combat && !Combat->CanAct()) || Now() < CastState.BusyUntil || !FindSpellDef(ShieldSpell))
		{
			bUp = false;
		}
	}
	if (bUp == bShieldUp)
	{
		return;
	}
	bShieldUp = bUp;
	if (bUp)
	{
		ShieldUpTime = Now();
	}
	OnShieldChanged.Broadcast(bShieldUp);
}

void UEwigfelsSpellCasterComponent::OnRep_ShieldUp()
{
	OnShieldChanged.Broadcast(bShieldUp);
}

EEwigfelsHitResult UEwigfelsSpellCasterComponent::InterceptDamage(FEwigfelsDamageInfo& Info)
{
	const FEwigfelsSpellDef* Shield = FindSpellDef(ShieldSpell);
	AActor* Owner = GetOwner();
	if (!bShieldUp || !Shield || !Owner)
	{
		return EEwigfelsHitResult::Hit;
	}
	// Nur Treffer aus dem vorderen Bogen
	const FVector From = Info.Instigator ? Info.Instigator->GetActorLocation() : Info.HitLocation;
	const FVector ToSource = (From - Owner->GetActorLocation()).GetSafeNormal2D();
	const float CosHalf = FMath::Cos(FMath::DegreesToRadians(ShieldArc * 0.5f));
	if (!ToSource.IsNearlyZero() && FVector::DotProduct(Owner->GetActorForwardVector(), ToSource) < CosHalf)
	{
		return EEwigfelsHitResult::Hit;
	}

	const FEwigfelsSpellDeliveryDef& D = Shield->Delivery;
	if (IsWithinParryWindow(ShieldUpTime, Now(), D.ParryWindow))
	{
		// Reinklang!
		AActor* Attacker = Info.Instigator;
		FEwigfelsSpellHit Self;
		Self.Caster = Owner;
		Self.Target = Owner;
		Self.Location = Info.HitLocation;
		EwigfelsSpellEffects::ApplyEffects(*Shield, Shield->OnParry, Self, Attacker);
		if (!Shield->CounterSpell.IsNone() && Attacker && UEwigfelsCombatComponent::AreHostile(Owner, Attacker))
		{
			// Konter darf trotz Erholung raus: Busy kurz freigeben
			CastState.BusyUntil = 0.f;
			CastSpellInternal(Shield->CounterSpell, Attacker->GetActorLocation(), Attacker, true);
		}
		Multicast_Parried(Attacker, Info.HitLocation);
		return EEwigfelsHitResult::Parried;
	}

	const float AtemCost = Info.Amount * D.BlockAtemPerDamage;
	if (Atem < AtemCost)
	{
		// Schild bricht: Atem leer, voller Schaden, kurz taumeln
		SpendAtem(Atem);
		SetShieldServer(false);
		if (UEwigfelsCombatComponent* Combat = GetCombat())
		{
			Combat->Stagger(0.6f);
		}
		return EEwigfelsHitResult::Hit;
	}
	SpendAtem(AtemCost);
	Info.Amount *= D.BlockDamageMultiplier;
	return EEwigfelsHitResult::Blocked;
}

void UEwigfelsSpellCasterComponent::Multicast_Parried_Implementation(AActor* Attacker, FVector_NetQuantize Location)
{
	OnParried.Broadcast(Attacker, Location);

	// Zeitlupe nur im Einzelspiel: Im Koop würde globale Zeitdehnung alle Spieler treffen
	UWorld* World = GetWorld();
	if (bParrySlowMotion && World && World->GetNetMode() == NM_Standalone)
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 0.25f);
		FTimerHandle Restore;
		TWeakObjectPtr<UWorld> WeakWorld = World;
		// 0,3 s echte Zeit = 0,075 s Spielzeit bei 0,25
		World->GetTimerManager().SetTimer(Restore, FTimerDelegate::CreateLambda([WeakWorld]()
		{
			if (UWorld* W = WeakWorld.Get())
			{
				UGameplayStatics::SetGlobalTimeDilation(W, 1.f);
			}
		}), 0.3f * 0.25f, false);
	}
}
