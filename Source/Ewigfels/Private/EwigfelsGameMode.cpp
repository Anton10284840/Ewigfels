// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "EwigfelsGameMode.h"
#include "Ewigfels.h"
#include "Character/EwigfelsCharacter.h"
#include "UI/EwigfelsHUD.h"
#include "World/EwigfelsTestArena.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Misc/PackageName.h"

AEwigfelsGameMode::AEwigfelsGameMode()
{
	StartupMessage = TEXT("Nocturna: Phase 3 – Testarena (F1 = Steuerung)");
	DefaultPawnClass = AEwigfelsCharacter::StaticClass();
	HUDClass = AEwigfelsHUD::StaticClass();
}

void AEwigfelsGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Blueprint-Figur auflösen (nur wenn das Asset wirklich existiert, sonst C++-Figur)
	if (!PlayerPawnClass.IsNull() && FPackageName::DoesPackageExist(PlayerPawnClass.GetLongPackageName()))
	{
		ResolvedPawnClass = PlayerPawnClass.TryLoadClass<APawn>();
	}

	UWorld* World = GetWorld();
	if (!bAutoSpawnTestArena || !World)
	{
		return;
	}
	bool bHasStart = false;
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		bHasStart = true;
		break;
	}
	TActorIterator<AEwigfelsTestArena> ArenaIt(World);
	AEwigfelsTestArena* Arena = ArenaIt ? *ArenaIt : nullptr;
	if (bHasStart && !Arena)
	{
		return; // normales Level
	}
	if (!Arena)
	{
		FActorSpawnParameters Params;
		Params.Name = TEXT("EwigfelsTestArena");
		Arena = World->SpawnActor<AEwigfelsTestArena>(AEwigfelsTestArena::StaticClass(), FTransform::Identity, Params);
		UE_LOG(LogEwigfels, Log, TEXT("GameMode: leeres Level, Testarena gespawnt"));
	}
	if (Arena && !bHasStart)
	{
		World->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), Arena->GetPlayerSpawn());
	}
}

UClass* AEwigfelsGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	return ResolvedPawnClass ? ResolvedPawnClass.Get() : Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AEwigfelsGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogEwigfels, Log, TEXT("%s"), *StartupMessage);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor(255, 196, 92), StartupMessage);
	}
}
