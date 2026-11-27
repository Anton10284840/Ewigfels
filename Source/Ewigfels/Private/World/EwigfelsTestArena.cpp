// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "World/EwigfelsTestArena.h"
#include "World/EwigfelsTestNPC.h"
#include "AI/EwigfelsEnemy.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Ewigfels.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	constexpr int32 PillarCount = 12;
	constexpr float PillarRadius = 2300.f;
	constexpr float ArenaHalf = 3000.f;

	// Farben (ART_BIBLE: warmer Stein, Bernstein-Licht, kühle Schatten)
	const FLinearColor Stone(0.16f, 0.15f, 0.14f);
	const FLinearColor StoneLight(0.32f, 0.29f, 0.25f);
	const FLinearColor DaisColor(0.22f, 0.2f, 0.18f);
	const FLinearColor Amber(1.f, 0.62f, 0.25f);
}

AEwigfelsTestArena::AEwigfelsTestArena()
{
	PrimaryActorTick.bCanEverTick = false;
	// Repliziert, damit eine vom GameMode gespawnte Arena auch bei Koop-Mitspielern auftaucht. Im Level platziert
	// existiert sie ohnehin überall. Gegner und Lio spawnt nur der Server.
	bReplicates = true;
	bAlwaysRelevant = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Static);
	RootComponent = Root;

	auto MakeMesh = [this](const TCHAR* Name, UStaticMesh* Mesh, const FVector& Loc, const FVector& Scale, const FRotator& Rot = FRotator::ZeroRotator)
	{
		UStaticMeshComponent* C = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		C->SetupAttachment(Root);
		C->SetStaticMesh(Mesh);
		C->SetRelativeLocationAndRotation(Loc, Rot);
		C->SetRelativeScale3D(Scale);
		C->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		C->SetMobility(EComponentMobility::Static);
		return C;
	};

	// Boden: 60 × 60 m, Oberkante bei Z = 0
	Floor = MakeMesh(TEXT("Floor"), CubeMesh.Object, FVector(0.f, 0.f, -50.f), FVector(ArenaHalf / 50.f, ArenaHalf / 50.f, 1.f));
	// Erhöhter runder Platz in der Mitte (Treffpunkt mit Lio)
	Dais = MakeMesh(TEXT("Dais"), CylinderMesh.Object, FVector(0.f, 0.f, 10.f), FVector(11.f, 11.f, 0.2f));
	DaisRing = MakeMesh(TEXT("DaisRing"), CylinderMesh.Object, FVector(0.f, 0.f, 11.f), FVector(11.3f, 11.3f, 0.19f));
	DaisRing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dais->SetCanEverAffectNavigation(true);

	// Säulenring mit glimmenden Klangquarzen oben
	for (int32 i = 0; i < PillarCount; ++i)
	{
		const float Angle = 2.f * PI * i / PillarCount;
		const FVector Pos(FMath::Cos(Angle) * PillarRadius, FMath::Sin(Angle) * PillarRadius, 0.f);
		const float Height = (i % 3 == 0) ? 7.f : 5.2f; // jede dritte Säule höher
		Pillars.Add(MakeMesh(*FString::Printf(TEXT("Pillar_%02d"), i), CylinderMesh.Object, Pos + FVector(0.f, 0.f, Height * 50.f), FVector(1.1f, 1.1f, Height)));
		UStaticMeshComponent* Glow = MakeMesh(*FString::Printf(TEXT("PillarGlow_%02d"), i), SphereMesh.Object, Pos + FVector(0.f, 0.f, Height * 100.f + 40.f), FVector(0.45f));
		Glow->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Glow->SetCastShadow(false);
		Glow->SetMobility(EComponentMobility::Movable);
		PillarGlows.Add(Glow);
		if (i % 2 == 0)
		{
			UPointLightComponent* L = CreateDefaultSubobject<UPointLightComponent>(*FString::Printf(TEXT("PillarLight_%02d"), i));
			L->SetupAttachment(Root);
			L->SetRelativeLocation(Pos + FVector(0.f, 0.f, Height * 100.f + 40.f));
			L->SetIntensityUnits(ELightUnits::Lumens);
			L->SetIntensity(2500.f);
			L->SetAttenuationRadius(1400.f);
			L->SetLightColor(Amber);
			L->SetCastShadows(false);
			L->SetMobility(EComponentMobility::Movable);
			PillarLights.Add(L);
		}
	}

	// Niedrige Randmauern
	const float WallH = 1.4f;
	Walls.Add(MakeMesh(TEXT("WallN"), CubeMesh.Object, FVector(ArenaHalf, 0.f, WallH * 50.f), FVector(1.f, ArenaHalf / 50.f, WallH)));
	Walls.Add(MakeMesh(TEXT("WallS"), CubeMesh.Object, FVector(-ArenaHalf, 0.f, WallH * 50.f), FVector(1.f, ArenaHalf / 50.f, WallH)));
	Walls.Add(MakeMesh(TEXT("WallE"), CubeMesh.Object, FVector(0.f, ArenaHalf, WallH * 50.f), FVector(ArenaHalf / 50.f, 1.f, WallH)));
	Walls.Add(MakeMesh(TEXT("WallW"), CubeMesh.Object, FVector(0.f, -ArenaHalf, WallH * 50.f), FVector(ArenaHalf / 50.f, 1.f, WallH)));

	// Deckung: zerbrochene Mauerstücke (Sichtlinien für den Fluchsänger)
	const FVector CoverSpots[] = { {900.f, 700.f, 0.f}, {1300.f, -600.f, 0.f}, {600.f, -1500.f, 0.f}, {-800.f, 1400.f, 0.f}, {1700.f, 200.f, 0.f} };
	for (int32 i = 0; i < UE_ARRAY_COUNT(CoverSpots); ++i)
	{
		const FRotator Rot(0.f, 25.f * i + 10.f, 0.f);
		Cover.Add(MakeMesh(*FString::Printf(TEXT("Cover_%d"), i), CubeMesh.Object, CoverSpots[i] + FVector(0.f, 0.f, 75.f), FVector(2.6f, 0.6f, 1.5f), Rot));
	}

	// Licht: goldene Stunde (ART_BIBLE §4), Sonne tief im Westen
	Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	Sun->SetupAttachment(Root);
	Sun->SetRelativeRotation(FRotator(-14.f, 215.f, 0.f));
	Sun->SetIntensity(9.f);
	Sun->SetLightColor(FLinearColor(1.f, 0.78f, 0.55f));
	Sun->SetAtmosphereSunLight(true);
	Sun->SetMobility(EComponentMobility::Movable);
	Sun->SetDynamicShadowCascades(4);

	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(Root);
	SkyLight->SetMobility(EComponentMobility::Movable);
	SkyLight->bRealTimeCapture = true;
	SkyLight->SourceType = ESkyLightSourceType::SLS_CapturedScene;
	SkyLight->SetIntensity(1.f);

	Atmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("Atmosphere"));
	Atmosphere->SetupAttachment(Root);

	Fog = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("Fog"));
	Fog->SetupAttachment(Root);
	Fog->SetFogDensity(0.012f);
	Fog->SetFogHeightFalloff(0.25f);
	Fog->SetVolumetricFog(true);
	Fog->SetFogInscatteringColor(FLinearColor(0.35f, 0.38f, 0.5f));

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->SetupAttachment(Root);
	PostProcess->bUnbound = true;
	FPostProcessSettings& PP = PostProcess->Settings;
	PP.bOverride_BloomIntensity = true;            PP.BloomIntensity = 0.9f;
	PP.bOverride_AutoExposureBias = true;          PP.AutoExposureBias = 0.4f;
	PP.bOverride_VignetteIntensity = true;         PP.VignetteIntensity = 0.35f;
	PP.bOverride_FilmGrainIntensity = true;        PP.FilmGrainIntensity = 0.04f;
	PP.bOverride_ColorSaturation = true;           PP.ColorSaturation = FVector4(1.05f, 1.02f, 0.98f, 1.f);
	PP.bOverride_SceneFringeIntensity = true;      PP.SceneFringeIntensity = 0.2f;

	// Gegnerwellen (steigend). Novize = Nahkampf, Fluchsaenger = Fernkampf (Content/Data/Creatures)
	FEwigfelsArenaWave W1; W1.Creatures = { TEXT("Novize") };
	FEwigfelsArenaWave W2; W2.Creatures = { TEXT("Novize"), TEXT("Novize") };
	FEwigfelsArenaWave W3; W3.Creatures = { TEXT("Fluchsaenger"), TEXT("Novize") };
	FEwigfelsArenaWave W4; W4.Creatures = { TEXT("Novize"), TEXT("Novize"), TEXT("Novize"), TEXT("Fluchsaenger") };
	FEwigfelsArenaWave W5; W5.Creatures = { TEXT("Fluchsaenger"), TEXT("Fluchsaenger"), TEXT("Novize"), TEXT("Novize") };
	Waves = { W1, W2, W3, W4, W5 };
}

void AEwigfelsTestArena::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	// Materialien hier (nicht im Konstruktor), damit dynamische Instanzen auch im Editor-Vorschaubild sitzen
	EwigfelsSpellVisuals::ApplySolid(Floor, Stone, 0.85f);
	EwigfelsSpellVisuals::ApplySolid(Dais, DaisColor, 0.6f);
	EwigfelsSpellVisuals::ApplyGlow(DaisRing, Amber, 3.f);
	for (UStaticMeshComponent* P : Pillars)    { EwigfelsSpellVisuals::ApplySolid(P, StoneLight, 0.75f); }
	for (UStaticMeshComponent* G : PillarGlows) { EwigfelsSpellVisuals::ApplyGlow(G, Amber, 18.f); }
	for (UStaticMeshComponent* W : Walls)      { EwigfelsSpellVisuals::ApplySolid(W, StoneLight, 0.8f); }
	for (UStaticMeshComponent* C : Cover)      { EwigfelsSpellVisuals::ApplySolid(C, Stone, 0.9f); }
}

FTransform AEwigfelsTestArena::GetPlayerSpawn() const
{
	return FTransform(FRotator(0.f, 0.f, 0.f), GetActorLocation() + FVector(-1400.f, 0.f, 100.f));
}

void AEwigfelsTestArena::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		return;
	}
	SpawnStatic();
	if (bAutoWaves)
	{
		WaveClearedTime = GetWorld()->GetTimeSeconds() - WaveDelay + FirstWaveDelay;
		GetWorldTimerManager().SetTimer(WaveTimer, this, &AEwigfelsTestArena::CheckWave, 1.f, true);
	}
}

AEwigfelsEnemy* AEwigfelsTestArena::SpawnEnemy(FName CreatureId, const FVector& Location, const FRotator& Rotation)
{
	const FTransform TM(Rotation, Location);
	AEwigfelsEnemy* Enemy = GetWorld()->SpawnActorDeferred<AEwigfelsEnemy>(AEwigfelsEnemy::StaticClass(), TM, this, nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	if (Enemy)
	{
		Enemy->SetCreatureId(CreatureId);
		UGameplayStatics::FinishSpawningActor(Enemy, TM);
	}
	return Enemy;
}

void AEwigfelsTestArena::SpawnStatic()
{
	const FVector Base = GetActorLocation();
	// Zwei Übungspuppen vor dem Startpunkt
	SpawnEnemy(TEXT("Uebungspuppe"), Base + FVector(-700.f, -250.f, 100.f), FRotator(0.f, 180.f, 0.f));
	SpawnEnemy(TEXT("Uebungspuppe"), Base + FVector(-700.f, 250.f, 100.f), FRotator(0.f, 180.f, 0.f));

	// Lio auf dem Platz
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetWorld()->SpawnActor<AEwigfelsTestNPC>(AEwigfelsTestNPC::StaticClass(), Base + FVector(-1100.f, 450.f, 120.f), FRotator(0.f, -150.f, 0.f), Params);
}

void AEwigfelsTestArena::CheckWave()
{
	WaveEnemies.RemoveAll([](const TWeakObjectPtr<AActor>& A)
	{
		const UEwigfelsCombatComponent* C = UEwigfelsCombatComponent::Find(A.Get());
		return !C || C->IsDead();
	});
	const float Now = GetWorld()->GetTimeSeconds();
	if (WaveEnemies.Num() > 0)
	{
		WaveClearedTime = -1.f;
		return;
	}
	if (WaveClearedTime < 0.f)
	{
		WaveClearedTime = Now;
		return;
	}
	if (Now - WaveClearedTime >= WaveDelay)
	{
		SpawnNextWave();
	}
}

void AEwigfelsTestArena::SpawnNextWave()
{
	if (!HasAuthority() || Waves.Num() == 0)
	{
		return;
	}
	const FEwigfelsArenaWave& Wave = Waves[NextWave % Waves.Num()];
	// Ab der zweiten Runde durch alle Wellen: ein Gegner mehr
	TArray<FName> Creatures = Wave.Creatures;
	if (NextWave >= Waves.Num())
	{
		Creatures.Add(TEXT("Novize"));
	}
	const FVector Base = GetActorLocation();
	for (int32 i = 0; i < Creatures.Num(); ++i)
	{
		// Am Ostende der Arena, gestaffelt
		const float Spread = (i - (Creatures.Num() - 1) * 0.5f) * 380.f;
		const FVector Loc = Base + FVector(1900.f + (i % 2) * 250.f, Spread, 110.f);
		if (AEwigfelsEnemy* Enemy = SpawnEnemy(Creatures[i], Loc, FRotator(0.f, 180.f, 0.f)))
		{
			WaveEnemies.Add(Enemy);
			EwigfelsSpellVisuals::SpawnFlash(GetWorld(), TEXT("sphere"), Loc, FRotator::ZeroRotator, FLinearColor(0.55f, 0.3f, 1.f),
				FVector(0.2f), FVector(3.f), 0.6f, 15000.f, 800.f);
		}
	}
	UE_LOG(LogEwigfels, Log, TEXT("Arena: Welle %d (%d Gegner)"), NextWave + 1, Creatures.Num());
	++NextWave;
	WaveClearedTime = -1.f;
}

void AEwigfelsTestArena::ClearEnemies()
{
	for (TActorIterator<AEwigfelsEnemy> It(GetWorld()); It; ++It)
	{
		if (It->GetCreatureId() != TEXT("Uebungspuppe"))
		{
			It->Destroy();
		}
	}
	WaveEnemies.Reset();
}

namespace
{
	AEwigfelsTestArena* FindArena(UWorld* World)
	{
		for (TActorIterator<AEwigfelsTestArena> It(World); It; ++It)
		{
			if (It->HasAuthority())
			{
				return *It;
			}
		}
		return nullptr;
	}

	FAutoConsoleCommandWithWorld GArenaWave(
		TEXT("Ewigfels.Arena.Wave"),
		TEXT("Startet sofort die nächste Gegnerwelle in der Testarena."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (AEwigfelsTestArena* Arena = FindArena(World))
			{
				Arena->SpawnNextWave();
			}
		}));

	FAutoConsoleCommandWithWorld GArenaClear(
		TEXT("Ewigfels.Arena.Clear"),
		TEXT("Entfernt alle Gegner (außer Übungspuppen)."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (AEwigfelsTestArena* Arena = FindArena(World))
			{
				Arena->ClearEnemies();
			}
		}));
}
