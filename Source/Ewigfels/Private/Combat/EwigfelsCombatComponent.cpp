// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Combat/EwigfelsCombatComponent.h"
#include "Combat/EwigfelsCombatDirector.h"
#include "Combat/EwigfelsForcedMotionComponent.h"
#include "Magic/EwigfelsSpellSubsystem.h"
#include "Magic/EwigfelsStatusLookComponent.h"
#include "Ewigfels.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UEwigfelsCombatComponent::UEwigfelsCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Status-Ablauf und DoT brauchen keine 60 Hz
	SetIsReplicatedByDefault(true);
}

void UEwigfelsCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEwigfelsCombatComponent, Health);
	DOREPLIFETIME(UEwigfelsCombatComponent, MaxHealth);
	DOREPLIFETIME(UEwigfelsCombatComponent, Team);
	DOREPLIFETIME(UEwigfelsCombatComponent, bDead);
	DOREPLIFETIME(UEwigfelsCombatComponent, ActiveStatuses);
	DOREPLIFETIME(UEwigfelsCombatComponent, StaggerEndTime);
}

void UEwigfelsCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	// Status-Look (Eisblock, Noten, Flammen …) nur dort, wo jemand hinschaut
	AActor* LookOwner = GetOwner();
	if (LookOwner && GetNetMode() != NM_DedicatedServer && !LookOwner->FindComponentByClass<UEwigfelsStatusLookComponent>())
	{
		UEwigfelsStatusLookComponent* Look = NewObject<UEwigfelsStatusLookComponent>(LookOwner, TEXT("EwigfelsStatusLook"));
		Look->RegisterComponent();
	}
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MaxHealth = DefaultMaxHealth;
		Health = MaxHealth;
	}
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

UEwigfelsCombatComponent* UEwigfelsCombatComponent::Find(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UEwigfelsCombatComponent>() : nullptr;
}

bool UEwigfelsCombatComponent::AreHostile(const AActor* A, const AActor* B)
{
	const UEwigfelsCombatComponent* CA = Find(A);
	const UEwigfelsCombatComponent* CB = Find(B);
	if (!CA || !CB || A == B)
	{
		return false;
	}
	if (CA->Team == EEwigfelsTeam::Neutral || CB->Team == EEwigfelsTeam::Neutral)
	{
		return false;
	}
	return CA->Team != CB->Team;
}

float UEwigfelsCombatComponent::GetServerTime() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0.f;
	}
	const AGameStateBase* GS = World->GetGameState();
	return GS ? static_cast<float>(GS->GetServerWorldTimeSeconds()) : World->GetTimeSeconds();
}

const FEwigfelsStatusDef* UEwigfelsCombatComponent::StatusDef(FName Id) const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	return Spells ? Spells->FindStatus(Id) : nullptr;
}

// ---------------------------------------------------------------------------
// Schaden

EEwigfelsHitResult UEwigfelsCombatComponent::ApplyDamage(FEwigfelsDamageInfo Info)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority() || bDead || Info.Amount <= 0.f)
	{
		return EEwigfelsHitResult::Ignored;
	}
	const float Now = GetServerTime();
	if (Now < InvulnerableEndTime)
	{
		Multicast_HitFeedback(0.f, Info.HitLocation, EEwigfelsHitResult::Ignored, Info.Instigator);
		return EEwigfelsHitResult::Ignored;
	}

	if (const float* Mult = ElementMultipliers.Find(Info.Element))
	{
		Info.Amount *= *Mult;
	}

	EEwigfelsHitResult Result = EEwigfelsHitResult::Hit;
	if (Info.bCanBeBlocked && DamageInterceptor.IsBound())
	{
		Result = DamageInterceptor.Execute(Info);
		if (Result == EEwigfelsHitResult::Parried || Result == EEwigfelsHitResult::Ignored)
		{
			Multicast_HitFeedback(0.f, Info.HitLocation, Result, Info.Instigator);
			return Result;
		}
	}

	const float Dealt = FMath::Min(Info.Amount, Health);
	SetHealth(Health - Info.Amount);
	if (Health <= 0.f)
	{
		Result = EEwigfelsHitResult::Killed;
	}
	else if (Info.Amount > 0.f)
	{
		// Schlaf u. ä. endet beim ersten Schaden
		TArray<FName> Broken;
		for (const FEwigfelsActiveStatus& Active : ActiveStatuses)
		{
			const FEwigfelsStatusDef* ActiveDef = StatusDef(Active.Id);
			if (ActiveDef && ActiveDef->bBreaksOnDamage)
			{
				Broken.Add(Active.Id);
			}
		}
		for (const FName Id : Broken)
		{
			RemoveStatus(Id);
		}
	}

	if (!Info.bSilent || Result == EEwigfelsHitResult::Killed)
	{
		Multicast_HitFeedback(Dealt, Info.HitLocation, Result, Info.Instigator);
	}
	if (Result == EEwigfelsHitResult::Killed)
	{
		Die(Info.Instigator);
	}
	else if (Result == EEwigfelsHitResult::Hit && !Info.bSilent && Info.Amount >= StaggerThreshold)
	{
		Stagger(StaggerDuration);
	}
	return Result;
}

void UEwigfelsCombatComponent::Heal(float Amount)
{
	if (GetOwner() && GetOwner()->HasAuthority() && !bDead && Amount > 0.f)
	{
		SetHealth(Health + Amount);
	}
}

void UEwigfelsCombatComponent::SetHealth(float NewHealth)
{
	const float Old = Health;
	Health = FMath::Clamp(NewHealth, 0.f, MaxHealth);
	if (!FMath::IsNearlyEqual(Old, Health))
	{
		OnHealthChanged.Broadcast(Health, MaxHealth);
	}
}

void UEwigfelsCombatComponent::SetMaxHealth(float NewMax, bool bRefill)
{
	MaxHealth = FMath::Max(1.f, NewMax);
	DefaultMaxHealth = MaxHealth;
	SetHealth(bRefill ? MaxHealth : FMath::Min(Health, MaxHealth));
}

void UEwigfelsCombatComponent::Die(AActor* Killer)
{
	if (bDead)
	{
		return;
	}
	bDead = true;
	ActiveStatuses.Reset();
	OnStatusesChanged.Broadcast();
	OnDied.Broadcast(Killer);
}

void UEwigfelsCombatComponent::ResetCombatState()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}
	bDead = false;
	ActiveStatuses.Reset();
	if (UEwigfelsForcedMotionComponent* Motion = GetOwner()->FindComponentByClass<UEwigfelsForcedMotionComponent>())
	{
		Motion->Release();
	}
	StaggerEndTime = 0.f;
	InvulnerableEndTime = 0.f;
	SetHealth(MaxHealth);
	OnHealthChanged.Broadcast(Health, MaxHealth);
	OnStatusesChanged.Broadcast();
}

void UEwigfelsCombatComponent::ApplyImpulse(FVector Velocity)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority() || bDead)
	{
		return;
	}
	if (ACharacter* Character = Cast<ACharacter>(Owner))
	{
		// XY überschreiben, damit der Stoß sich klar anfühlt; Z nur, wenn nach oben gestoßen wird.
		Character->LaunchCharacter(Velocity, true, Velocity.Z > 0.f);
	}
}

void UEwigfelsCombatComponent::Stagger(float Seconds)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || bDead || Seconds <= 0.f)
	{
		return;
	}
	StaggerEndTime = FMath::Max(StaggerEndTime, GetServerTime() + Seconds);
	Multicast_Staggered(Seconds);
}

void UEwigfelsCombatComponent::SetInvulnerableFor(float Seconds)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InvulnerableEndTime = FMath::Max(InvulnerableEndTime, GetServerTime() + Seconds);
	}
}

void UEwigfelsCombatComponent::Multicast_HitFeedback_Implementation(float Amount, FVector_NetQuantize Location, EEwigfelsHitResult Result, AActor* Instigator)
{
	OnHitFeedback.Broadcast(Amount, Location, Result, Instigator);
	if (UEwigfelsCombatDirector* Director = GetWorld() ? GetWorld()->GetSubsystem<UEwigfelsCombatDirector>() : nullptr)
	{
		FEwigfelsHitEvent Event;
		Event.Victim = GetOwner();
		Event.Instigator = Instigator;
		Event.Amount = Amount;
		Event.Location = Location;
		Event.Result = Result;
		Director->BroadcastHit(Event);
	}
}

void UEwigfelsCombatComponent::Multicast_Staggered_Implementation(float Seconds)
{
	OnStaggered.Broadcast(Seconds);
}

void UEwigfelsCombatComponent::OnRep_Health(float OldHealth)
{
	OnHealthChanged.Broadcast(Health, MaxHealth);
}

void UEwigfelsCombatComponent::OnRep_Dead()
{
	if (bDead)
	{
		OnDied.Broadcast(nullptr);
	}
}

void UEwigfelsCombatComponent::OnRep_Statuses()
{
	OnStatusesChanged.Broadcast();
}

// ---------------------------------------------------------------------------
// Status-Effekte

bool UEwigfelsCombatComponent::ApplyStatus(FName StatusId, float Duration, AActor* Instigator)
{
	if (!GetOwner() || !GetOwner()->HasAuthority() || bDead || StatusId.IsNone())
	{
		return false;
	}
	const FEwigfelsStatusDef* Def = StatusDef(StatusId);
	if (!Def)
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Status '%s' ist nicht in Content/Data/Spells definiert"), *StatusId.ToString());
		return false;
	}

	// Gegen-Status aufheben (Feuer taut Eis). Hebt der neue Status einen bestehenden auf, verbraucht er sich dabei
	// nicht – Glut auf Gefroren = taut auf UND brennt.
	bool bChanged = false;
	for (const FName Cancel : Def->Cancels)
	{
		bChanged |= ActiveStatuses.RemoveAll([Cancel](const FEwigfelsActiveStatus& S) { return S.Id == Cancel; }) > 0;
	}

	const float Now = GetServerTime();
	const float Length = Duration > 0.f ? Duration : Def->Duration;
	FEwigfelsActiveStatus* Existing = ActiveStatuses.FindByPredicate([StatusId](const FEwigfelsActiveStatus& S) { return S.Id == StatusId; });
	if (Existing)
	{
		Existing->Stacks += 1;
		Existing->EndTime = FMath::Max(Existing->EndTime, Now + Length);
		Existing->Duration = Length;
		Existing->Instigator = Instigator;
	}
	else
	{
		FEwigfelsActiveStatus& New = ActiveStatuses.AddDefaulted_GetRef();
		New.Id = StatusId;
		New.EndTime = Now + Length;
		New.Duration = Length;
		New.Stacks = 1;
		New.Instigator = Instigator;
		Existing = &New;
	}

	// Eskalation: 3× Unterkühlt = Gefroren
	if (Def->EscalateAt > 0 && !Def->EscalateTo.IsNone() && Existing->Stacks >= Def->EscalateAt)
	{
		ActiveStatuses.RemoveAll([StatusId](const FEwigfelsActiveStatus& S) { return S.Id == StatusId; });
		OnStatusesChanged.Broadcast();
		return ApplyStatus(Def->EscalateTo, 0.f, Instigator);
	}

	OnStatusesChanged.Broadcast();
	return true;
}

bool UEwigfelsCombatComponent::RemoveStatus(FName StatusId)
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return false;
	}
	const bool bRemoved = ActiveStatuses.RemoveAll([StatusId](const FEwigfelsActiveStatus& S) { return S.Id == StatusId; }) > 0;
	if (bRemoved)
	{
		OnStatusesChanged.Broadcast();
	}
	return bRemoved;
}

bool UEwigfelsCombatComponent::HasStatus(FName StatusId) const
{
	return ActiveStatuses.ContainsByPredicate([StatusId](const FEwigfelsActiveStatus& S) { return S.Id == StatusId; });
}

int32 UEwigfelsCombatComponent::GetStatusStacks(FName StatusId) const
{
	const FEwigfelsActiveStatus* Found = ActiveStatuses.FindByPredicate([StatusId](const FEwigfelsActiveStatus& S) { return S.Id == StatusId; });
	return Found ? Found->Stacks : 0;
}

float UEwigfelsCombatComponent::GetMoveSpeedMultiplier() const
{
	float Mult = 1.f;
	for (const FEwigfelsActiveStatus& S : ActiveStatuses)
	{
		if (const FEwigfelsStatusDef* Def = StatusDef(S.Id))
		{
			Mult *= Def->MoveSpeedMultiplier;
		}
	}
	return Mult;
}

float UEwigfelsCombatComponent::GetActionSpeedMultiplier() const
{
	float Mult = 1.f;
	for (const FEwigfelsActiveStatus& S : ActiveStatuses)
	{
		if (const FEwigfelsStatusDef* Def = StatusDef(S.Id))
		{
			Mult *= Def->ActionSpeedMultiplier;
		}
	}
	return FMath::Max(0.05f, Mult);
}

float UEwigfelsCombatComponent::GetAtemRegenMultiplier() const
{
	float Mult = 1.f;
	for (const FEwigfelsActiveStatus& S : ActiveStatuses)
	{
		if (const FEwigfelsStatusDef* Def = StatusDef(S.Id))
		{
			Mult *= Def->AtemRegenMultiplier;
		}
	}
	return Mult;
}

bool UEwigfelsCombatComponent::IsStaggered() const
{
	return GetServerTime() < StaggerEndTime;
}

bool UEwigfelsCombatComponent::CanAct() const
{
	if (bDead || IsStaggered())
	{
		return false;
	}
	for (const FEwigfelsActiveStatus& S : ActiveStatuses)
	{
		const FEwigfelsStatusDef* Def = StatusDef(S.Id);
		if (Def && Def->bStopsActions)
		{
			return false;
		}
	}
	return true;
}

void UEwigfelsCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		TickStatusesServer(DeltaTime);
	}
}

void UEwigfelsCombatComponent::TickStatusesServer(float DeltaTime)
{
	if (ActiveStatuses.Num() == 0 || bDead)
	{
		return;
	}
	const float Now = GetServerTime();

	// Schaden über Zeit, gesammelt und als ganze Punkte ausgeteilt (keine 0,5er-Zahlen im HUD)
	float Dot = 0.f;
	AActor* DotInstigator = nullptr;
	for (const FEwigfelsActiveStatus& S : ActiveStatuses)
	{
		const FEwigfelsStatusDef* Def = StatusDef(S.Id);
		if (Def && Def->DamagePerSecond > 0.f)
		{
			Dot += Def->DamagePerSecond * DeltaTime;
			DotInstigator = S.Instigator;
		}
	}
	DotAccumulator += Dot;
	if (DotAccumulator >= 1.f)
	{
		FEwigfelsDamageInfo Info;
		Info.Amount = FMath::FloorToFloat(DotAccumulator);
		Info.Element = TEXT("Glut");
		Info.Instigator = DotInstigator;
		Info.Source = TEXT("status");
		Info.HitLocation = GetOwner()->GetActorLocation();
		Info.bCanBeBlocked = false;
		Info.bSilent = true;
		DotAccumulator -= Info.Amount;
		ApplyDamage(Info);
	}

	const int32 Removed = ActiveStatuses.RemoveAll([Now](const FEwigfelsActiveStatus& S) { return S.EndTime <= Now; });
	if (Removed > 0)
	{
		OnStatusesChanged.Broadcast();
	}
}
