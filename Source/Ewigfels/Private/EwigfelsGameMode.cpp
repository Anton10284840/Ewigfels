// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "EwigfelsGameMode.h"
#include "Ewigfels.h"
#include "Engine/Engine.h"

AEwigfelsGameMode::AEwigfelsGameMode()
{
	StartupMessage = TEXT("Ewigfels: C++-Modul geladen (Phase 0 OK)");
}

void AEwigfelsGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogEwigfels, Log, TEXT("%s"), *StartupMessage);

	if (GEngine)
	{
		// Key -1 = neue Zeile, 8 s sichtbar, goldene Farbe (Signature-Look :)
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor(255, 196, 92), StartupMessage);
	}
}
