// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EwigfelsCharacterMovementComponent.generated.h"

/**
 * Bewegung mit Sprinten (netzwerk-vorhergesagt über Saved Moves, kein Ruckeln im Koop) und Tempo-Faktoren aus
 * Status (Unterkühlt, Gefroren, Verflucht), Zaubern und Schild.
 */
UCLASS()
class EWIGFELS_API UEwigfelsCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UEwigfelsCharacterMovementComponent();

	virtual float GetMaxSpeed() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Movement")
	void SetSprinting(bool bSprint) { bWantsToSprint = bSprint; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Movement")
	bool IsSprinting() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement")
	float SprintSpeed = 720.f;

	/** Tempo beim Zaubern mit einer Hand bzw. beiden Händen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement")
	float CastingSpeedMultiplier = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement")
	float TwoHandedCastingSpeedMultiplier = 0.55f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement")
	float ShieldSpeedMultiplier = 0.5f;

	/** Vom Client gewünscht, per Saved Move zum Server. */
	uint8 bWantsToSprint : 1;
};
