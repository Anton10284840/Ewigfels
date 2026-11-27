// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EwigfelsInteractable.generated.h"

class APawn;

UINTERFACE(MinimalAPI, BlueprintType)
class UEwigfelsInteractable : public UInterface
{
	GENERATED_BODY()
};

/** Alles, womit man per E/X interagiert (NPCs, Türen, Truhen). Wird auf der Maschine des Spielers aufgerufen. */
class EWIGFELS_API IEwigfelsInteractable
{
	GENERATED_BODY()

public:
	/** Text im HUD, z. B. "Mit Lio sprechen". */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ewigfels|Interaction")
	FText GetInteractPrompt() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ewigfels|Interaction")
	void Interact(APawn* InstigatorPawn);
};
