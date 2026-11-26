// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EwigfelsGameMode.generated.h"

/**
 * Phase-0-GameMode: beweist nur, dass C++ kompiliert und geladen wird.
 * Wird ab Phase 2 durch den echten GameMode (Quest/Save/World-Subsysteme) ersetzt.
 */
UCLASS()
class EWIGFELS_API AEwigfelsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEwigfelsGameMode();

	/** Text, der beim Spielstart im Log und auf dem Bildschirm erscheint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Debug")
	FString StartupMessage;

protected:
	virtual void BeginPlay() override;
};
