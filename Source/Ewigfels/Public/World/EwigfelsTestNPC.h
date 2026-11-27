// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "World/EwigfelsInteractable.h"
#include "EwigfelsTestNPC.generated.h"

class UEwigfelsCombatComponent;
class UEwigfelsPlaceholderBody;

/**
 * Gesprächspartner in der Testarena (Lio). Zeigt die Dialog-Kamera mit einem kurzen Platzhalter-Gespräch.
 * Echte NPCs bekommen in Phase 5 SUDS-Skripte (UEwigfelsDialogueBridge) statt der festen Zeilen hier.
 */
UCLASS()
class EWIGFELS_API AEwigfelsTestNPC : public ACharacter, public IEwigfelsInteractable
{
	GENERATED_BODY()

public:
	AEwigfelsTestNPC();

	virtual void Tick(float DeltaSeconds) override;

	virtual FText GetInteractPrompt_Implementation() const override;
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	/** Weltstatus-/Quest-Name (WorldState.json: npcs). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|NPC")
	FName NpcId = TEXT("Lio");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|NPC")
	FText DisplayName;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|NPC")
	TObjectPtr<UEwigfelsCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|NPC")
	TObjectPtr<UEwigfelsPlaceholderBody> PlaceholderBody;

private:
	TWeakObjectPtr<APawn> TalkingTo;
};
