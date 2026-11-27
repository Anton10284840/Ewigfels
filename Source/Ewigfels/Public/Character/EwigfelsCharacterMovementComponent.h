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
	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	/** Magische Landung läuft gerade (Fall wird kurz vor dem Boden abgebremst). Gilt auf Server und eigenem Client. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Movement")
	bool IsFallCatching() const { return bFallCatching; }

	/**
	 * Reine Rechenfunktion (getestet): Aufprall-Tempo ohne Bremsen, aus Fallgeschwindigkeit (positiv = nach unten),
	 * Höhe über dem Boden und Schwerkraft (positiv).
	 */
	static float PredictImpactSpeed(float DownSpeed, float HeightAboveGround, float GravityAbs);

	/** Bremsverzögerung (cm/s², positiv), um von DownSpeed auf TargetSpeed zu kommen, bevor der Boden da ist. */
	static float ComputeCatchDeceleration(float DownSpeed, float TargetSpeed, float HeightAboveGround, float GravityAbs);

	/** Magische Landung an/aus. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement|FallCatch")
	bool bFallCatchEnabled = true;

	/** Ab diesem Aufprall-Tempo (cm/s) greift der Abfangzauber (1100 ≈ 6 m Fallhöhe). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement|FallCatch")
	float FallCatchTriggerSpeed = 1100.f;

	/** So hoch über dem Boden setzt das Bremsen ein (cm). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement|FallCatch")
	float FallCatchHeight = 420.f;

	/** Mit diesem Tempo setzt die Figur auf (cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Movement|FallCatch")
	float FallCatchLandingSpeed = 260.f;

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

private:
	/** Abgeleiteter Zustand: rechnen Server und Client gleich aus (gleiche Position, gleiche Welt). Zurück beim Landen. */
	mutable bool bFallCatching = false;

	/** Abstand Kapsel-Unterkante → Boden (cm), oder < 0, wenn innerhalb MaxDistance nichts ist. */
	float TraceGroundDistance(float MaxDistance) const;
};
