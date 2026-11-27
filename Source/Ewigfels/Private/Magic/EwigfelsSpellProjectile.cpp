// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsSpellProjectile.h"
#include "Magic/EwigfelsSpellEffects.h"
#include "Magic/EwigfelsSpellSubsystem.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"

AEwigfelsSpellProjectile::AEwigfelsSpellProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);
	SetCanBeDamaged(false);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(15.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);
	Collision->bReturnMaterialOnMove = false;
	Collision->CanCharacterStepUpOn = ECB_No;
	RootComponent = Collision;

	Core = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Core"));
	Core->SetupAttachment(Collision);
	Core->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Core->SetCastShadow(false);
	Core->SetMobility(EComponentMobility::Movable);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(Collision);
	Light->SetMobility(EComponentMobility::Movable);
	Light->SetIntensityUnits(ELightUnits::Lumens);
	Light->SetIntensity(3000.f);
	Light->SetAttenuationRadius(450.f);
	Light->SetCastShadows(false);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->UpdatedComponent = Collision;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;
	Movement->ProjectileGravityScale = 0.f;
	Movement->InitialSpeed = 3000.f;
	Movement->MaxSpeed = 8000.f;
}

void AEwigfelsSpellProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AEwigfelsSpellProjectile, SpellId, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AEwigfelsSpellProjectile, bChainEnd, COND_InitialOnly);
}

void AEwigfelsSpellProjectile::InitSpell(FName InSpellId, bool bInChainEnd, AActor* InHomingTarget)
{
	SpellId = InSpellId;
	bChainEnd = bInChainEnd;
	HomingTarget = InHomingTarget;
}

const FEwigfelsSpellDef* AEwigfelsSpellProjectile::GetSpell() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	return Spells ? Spells->FindSpell(SpellId) : nullptr;
}

void AEwigfelsSpellProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Server: InitSpell lief schon (deferred spawn). Client: SpellId kommt evtl. erst per OnRep.
	ConfigureMovement();
}

void AEwigfelsSpellProjectile::ConfigureMovement()
{
	const FEwigfelsSpellDef* Spell = GetSpell();
	if (!Spell || bMovementConfigured)
	{
		return;
	}
	bMovementConfigured = true;
	const auto& D = Spell->Delivery;
	Collision->SetSphereRadius(FMath::Max(2.f, D.Radius));
	Movement->InitialSpeed = D.Speed;
	Movement->MaxSpeed = FMath::Max(D.Speed * 1.5f, 100.f);
	Movement->ProjectileGravityScale = D.Gravity;
	Movement->Velocity = GetActorForwardVector() * D.Speed;
	AActor* Target = HomingTarget.Get();
	if (D.Homing > 0.f && Target && Target->GetRootComponent())
	{
		Movement->bIsHomingProjectile = true;
		Movement->HomingTargetComponent = Target->GetRootComponent();
		Movement->HomingAccelerationMagnitude = D.Homing * D.Speed * 6.f;
	}
	PierceLeft = D.Pierce;
}

void AEwigfelsSpellProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Caster = GetOwner())
	{
		Collision->IgnoreActorWhenMoving(Caster, true);
	}
	if (HasAuthority())
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AEwigfelsSpellProjectile::OnOverlap);
		Collision->OnComponentHit.AddDynamic(this, &AEwigfelsSpellProjectile::OnBlockingHit);
		if (const FEwigfelsSpellDef* Spell = GetSpell())
		{
			SetLifeSpan(Spell->Delivery.Range / FMath::Max(100.f, Spell->Delivery.Speed) + 0.1f);
		}
		else
		{
			SetLifeSpan(2.f);
		}
	}
	ApplyVisuals();
}

void AEwigfelsSpellProjectile::OnRep_SpellId()
{
	ConfigureMovement();
	ApplyVisuals();
}

void AEwigfelsSpellProjectile::ApplyVisuals()
{
	const FEwigfelsSpellDef* Spell = GetSpell();
	if (!Spell || bVisualsApplied || GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	bVisualsApplied = true;
	const FLinearColor Color = Spell->Visual.Color;
	Light->SetLightColor(Color);

	Trail = EwigfelsSpellVisuals::SpawnNiagaraAttached(Collision, Spell->Visual.Niagara, Color, Spell->Visual.Scale);
	if (Trail)
	{
		Core->SetVisibility(false);
		return;
	}
	// Platzhalter: leuchtende Kugel bzw. Eislanze
	const bool bLance = Spell->Visual.Shape == TEXT("lance");
	Core->SetStaticMesh(EwigfelsSpellVisuals::GetShape(bLance ? FName(TEXT("cylinder")) : FName(TEXT("sphere"))));
	const float S = Spell->Visual.Scale;
	if (bLance)
	{
		// Zylinder liegt auf der Z-Achse: um 90° kippen, damit er in Flugrichtung zeigt
		Core->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		Core->SetRelativeScale3D(FVector(0.12f * S, 0.12f * S, 1.1f * S));
	}
	else
	{
		Core->SetRelativeScale3D(FVector(S * 0.5f));
	}
	EwigfelsSpellVisuals::ApplyGlow(Core, Color, 40.f);
	// Weicher Halo in der Glühfarbe (2. Grundton): macht aus der Kugel ein leuchtendes Licht statt Plastik
	EwigfelsSpellVisuals::AddHalo(Collision, Spell->Visual.GlowColor, (bLance ? 90.f : 140.f) * S, 6.f);
}

void AEwigfelsSpellProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bImpacted || !OtherActor || OtherActor == GetOwner())
	{
		return;
	}
	if (!UEwigfelsCombatComponent::AreHostile(GetOwner(), OtherActor))
	{
		return; // Verbündete und Neutrale werden durchflogen
	}
	const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(OtherActor);
	if (!Combat || Combat->IsDead() || AlreadyHit.Contains(OtherActor))
	{
		return;
	}
	const FVector Location = bFromSweep ? FVector(SweepResult.ImpactPoint) : GetActorLocation();
	HitTarget(OtherActor, Location);

	const FEwigfelsSpellDef* Spell = GetSpell();
	if (Spell && Spell->Delivery.ExplodeRadius <= 0.f && PierceLeft > 0)
	{
		--PierceLeft;
		Multicast_Impact(Location, -GetActorForwardVector());
		return;
	}
	Impact(Location, -GetActorForwardVector());
}

void AEwigfelsSpellProjectile::OnBlockingHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bImpacted)
	{
		Impact(Hit.ImpactPoint, Hit.ImpactNormal);
	}
}

void AEwigfelsSpellProjectile::HitTarget(AActor* Target, const FVector& Location)
{
	const FEwigfelsSpellDef* Spell = GetSpell();
	if (!Spell)
	{
		return;
	}
	AlreadyHit.Add(Target);
	FEwigfelsSpellHit Hit;
	Hit.Caster = GetOwner();
	Hit.Target = Target;
	Hit.Location = Location;
	Hit.Direction = GetVelocity().GetSafeNormal();
	Hit.bChainEnd = bChainEnd;
	EwigfelsSpellEffects::ApplyEffects(*Spell, Spell->Effects, Hit);
}

void AEwigfelsSpellProjectile::Impact(const FVector& Location, const FVector& Normal)
{
	if (bImpacted || !HasAuthority())
	{
		return;
	}
	bImpacted = true;

	const FEwigfelsSpellDef* Spell = GetSpell();
	if (Spell && Spell->Delivery.ExplodeRadius > 0.f)
	{
		for (AActor* Target : EwigfelsSpellEffects::GatherSphere(GetOwner(), Location, Spell->Delivery.ExplodeRadius))
		{
			if (!AlreadyHit.Contains(Target))
			{
				HitTarget(Target, Target->GetActorLocation());
			}
		}
	}

	Multicast_Impact(Location, Normal);
	Movement->StopMovementImmediately();
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorHiddenInGame(true);
	// Kurz am Leben lassen, damit der Multicast sicher ankommt
	SetLifeSpan(0.3f);
}

void AEwigfelsSpellProjectile::Multicast_Impact_Implementation(FVector_NetQuantize Location, FVector_NetQuantizeNormal Normal)
{
	if (const FEwigfelsSpellDef* Spell = GetSpell())
	{
		EwigfelsSpellVisuals::PlayImpact(GetWorld(), *Spell, Location, Normal);
	}
}
