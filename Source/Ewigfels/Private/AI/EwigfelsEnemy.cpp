// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "AI/EwigfelsEnemy.h"
#include "AI/EwigfelsEnemyController.h"
#include "Character/EwigfelsCharacterMovementComponent.h"
#include "Character/EwigfelsPlaceholderBody.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Combat/EwigfelsCombatDirector.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Quest/EwigfelsQuestSubsystem.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

AEwigfelsEnemy::AEwigfelsEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UEwigfelsCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(38.f, 90.f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	bUseControllerRotationYaw = false;
	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = true;
	Move->RotationRate = FRotator(0.f, 480.f, 0.f);
	Move->bUseRVOAvoidance = true;
	Move->AvoidanceConsiderationRadius = 300.f;
	Move->MaxWalkSpeed = 260.f;

	Combat = CreateDefaultSubobject<UEwigfelsCombatComponent>(TEXT("Combat"));
	Combat->SetTeam(EEwigfelsTeam::Enemy);

	Caster = CreateDefaultSubobject<UEwigfelsSpellCasterComponent>(TEXT("Caster"));
	Caster->ShieldSpell = NAME_None;
	Caster->BasicSpell = NAME_None;
	Caster->bParrySlowMotion = false;

	PlaceholderBody = CreateDefaultSubobject<UEwigfelsPlaceholderBody>(TEXT("PlaceholderBody"));
	PlaceholderBody->SetupAttachment(RootComponent);

	AIControllerClass = AEwigfelsEnemyController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEwigfelsEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AEwigfelsEnemy, CreatureId, COND_InitialOnly);
}

const FEwigfelsCreatureDef* AEwigfelsEnemy::GetCreatureDef() const
{
	const UWorld* World = GetWorld();
	const UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
	const UEwigfelsCreatureSubsystem* Creatures = GI ? GI->GetSubsystem<UEwigfelsCreatureSubsystem>() : nullptr;
	return Creatures ? Creatures->FindCreature(CreatureId) : nullptr;
}

FText AEwigfelsEnemy::GetDisplayName() const
{
	const FEwigfelsCreatureDef* Def = GetCreatureDef();
	return Def ? Def->Name : FText::FromName(CreatureId);
}

void AEwigfelsEnemy::ApplyCreatureDef()
{
	const FEwigfelsCreatureDef* Def = GetCreatureDef();
	if (!Def || bDefApplied)
	{
		return;
	}
	bDefApplied = true;

	// Optik: echtes Mesh oder Platzhalter
	if (USkeletalMesh* Mesh = Cast<USkeletalMesh>(EwigfelsSpellVisuals::LoadIfExists(Def->Look.Mesh)))
	{
		GetMesh()->SetSkeletalMesh(Mesh);
		if (UClass* AnimClass = Def->Look.AnimClass.TryLoadClass<UAnimInstance>())
		{
			GetMesh()->SetAnimInstanceClass(AnimClass);
		}
	}
	PlaceholderBody->SetLook(Def->Look.Body, Def->Look.Accent, Def->Look.Eyes, Def->Look.Height, Def->Look.bHood);
	const float Scale = Def->Look.Height;
	GetCapsuleComponent()->SetCapsuleSize(38.f * FMath::Sqrt(Scale), 90.f * Scale);

	GetCharacterMovement()->MaxWalkSpeed = Def->WalkSpeed;
	if (Def->Behavior == EEwigfelsCreatureBehavior::Dummy)
	{
		GetCharacterMovement()->DisableMovement();
	}

	Combat->DefaultMaxHealth = Def->Health;
	Combat->ElementMultipliers = Def->ElementMultipliers;
	Combat->StaggerThreshold = Def->StaggerThreshold;
	if (HasAuthority())
	{
		Caster->SetLoadout(Def->Spells);
	}
}

void AEwigfelsEnemy::OnRep_CreatureId()
{
	ApplyCreatureDef();
}

void AEwigfelsEnemy::BeginPlay()
{
	// Vor Super::BeginPlay, damit Komponenten (Platzhalter-Höhe, Max-Leben) die Werte beim Start schon haben
	ApplyCreatureDef();
	Super::BeginPlay();

	Caster->OnCastStarted.AddDynamic(this, &AEwigfelsEnemy::HandleCastStarted);
	Caster->OnSpellReleased.AddDynamic(this, &AEwigfelsEnemy::HandleSpellReleased);
	Combat->OnHitFeedback.AddDynamic(this, &AEwigfelsEnemy::HandleHitFeedback);
	Combat->OnDied.AddDynamic(this, &AEwigfelsEnemy::HandleDied);
	Combat->OnStaggered.AddDynamic(this, &AEwigfelsEnemy::HandleStaggered);
}

void AEwigfelsEnemy::SetRunning(bool bRun)
{
	if (const FEwigfelsCreatureDef* Def = GetCreatureDef())
	{
		GetCharacterMovement()->MaxWalkSpeed = bRun ? Def->RunSpeed : Def->WalkSpeed;
	}
}

bool AEwigfelsEnemy::IsTelegraphing() const
{
	return GetWorld() && GetWorld()->GetTimeSeconds() < TelegraphUntil;
}

void AEwigfelsEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const float Now = GetWorld()->GetTimeSeconds();
	if (PlaceholderBody->IsActiveBody())
	{
		// Vorwarnung baut sich über die Ausholzeit auf: je heller, desto näher der Schlag
		const float Remaining = TelegraphUntil - Now;
		PlaceholderBody->SetTelegraph(Remaining > 0.f ? FMath::Clamp(1.f - Remaining / 0.9f, 0.25f, 1.f) : 0.f);
	}
	// Übungspuppe heilt sich nach 2 s Ruhe
	if (HasAuthority() && !Combat->IsDead())
	{
		const FEwigfelsCreatureDef* Def = GetCreatureDef();
		if (Def && Def->Behavior == EEwigfelsCreatureBehavior::Dummy && Now - LastHitTime > 2.f)
		{
			Combat->Heal(60.f * DeltaSeconds);
		}
	}
}

void AEwigfelsEnemy::HandleCastStarted(FName SpellId, uint8 ChainIndex, float CastTime)
{
	TelegraphUntil = GetWorld()->GetTimeSeconds() + CastTime;
	if (const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(SpellId))
	{
		PlaceholderBody->SetCastPose(1.f, Spell->Visual.Color, Spell->bTwoHanded);
	}
}

void AEwigfelsEnemy::HandleSpellReleased(FName SpellId, FVector Origin, FVector Direction)
{
	TelegraphUntil = 0.f;
	PlaceholderBody->SetCastPose(0.f, FLinearColor::White, false);
	if (const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(SpellId))
	{
		EwigfelsSpellVisuals::PlayRelease(GetWorld(), *Spell, Origin, Direction, this);
	}
}

void AEwigfelsEnemy::HandleHitFeedback(float Amount, FVector Location, EEwigfelsHitResult Result, AActor* InstigatorActor)
{
	if (Amount > 0.f)
	{
		LastHitTime = GetWorld()->GetTimeSeconds();
		PlaceholderBody->PlayHit();
	}
}

void AEwigfelsEnemy::HandleStaggered(float Seconds)
{
	TelegraphUntil = 0.f;
	PlaceholderBody->SetCastPose(0.f, FLinearColor::White, false);
	if (HasAuthority())
	{
		Caster->InterruptCast();
	}
}

void AEwigfelsEnemy::HandleDied(AActor* Killer)
{
	TelegraphUntil = 0.f;
	PlaceholderBody->PlayDeath();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	if (HasAuthority())
	{
		Caster->InterruptCast();
		if (UEwigfelsCombatDirector* Director = GetWorld()->GetSubsystem<UEwigfelsCombatDirector>())
		{
			Director->ReleaseMeleeToken(this);
		}
		// Quest-Ziel {kind: defeat, target: <CreatureId>}
		if (UEwigfelsQuestSubsystem* Quests = GetGameInstance() ? GetGameInstance()->GetSubsystem<UEwigfelsQuestSubsystem>() : nullptr)
		{
			Quests->ReportEvent(TEXT("defeat"), CreatureId.ToString());
		}
		SetLifeSpan(4.f);
	}
}
