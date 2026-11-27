// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EwigfelsAnimInstance.generated.h"

/**
 * C++-Basis für ABP_EwigfelsCharacter (ANIMATION_BIBLE §2): rechnet alle Werte, das AnimBP verdrahtet nur.
 * Locomotion läuft über Motion Matching (Pose Search, Datenbanken aus dem Game Animation Sample);
 * die Trajektorie erzeugt das AnimBP selbst (Knoten "Pose Search Generate Trajectory"), hier stehen die Zustände
 * für Choosers, Overlay (Zaubern) und Additive.
 *
 * Werte werden im Game-Thread eingesammelt (NativeUpdateAnimation) und thread-sicher weiterverarbeitet
 * (NativeThreadSafeUpdateAnimation) – im AnimBP nur "Property Access" / thread-safe Funktionen nutzen.
 */
UCLASS(Transient, Blueprintable)
class EWIGFELS_API UEwigfelsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	// --- Locomotion ---
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	float GroundSpeed = 0.f;

	/** Laufrichtung relativ zur Blickrichtung (-180..180), für Strafe-Choosers. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	float Direction = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	bool bIsMoving = false;

	/** Will sich bewegen (Eingabe), auch wenn noch steht – für Starts. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	bool bHasMoveInput = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	bool bIsSprinting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	bool bIsStrafing = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	bool bIsDodging = false;

	/** Neigung in Kurven (-1..1), für Lean-Additive. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Locomotion")
	float Lean = 0.f;

	// --- Zielen (Aim Offset, Hand-Zielen) ---
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Aim")
	float AimPitch = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Aim")
	float AimYaw = 0.f;

	// --- Zaubern (Overlay-Layer ab spine_01) ---
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bIsCasting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bTwoHandedCast = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bShieldUp = false;

	/** Schule der aktuellen Weise (Schlag, Schutz, …) – wählt die Grundgeste. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName CastSchool;

	/** Erster Grundton – wählt die Fingerpose aus PA_Hand_Chords. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName CastNote;

	/** 0..1 zwischen Ruhe und Bereit (Finger gespreizt, Funken). Weich nachgeführt. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	float HandReadyAlpha = 0.f;

	// --- Kampf ---
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Combat")
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Combat")
	bool bIsStaggered = false;

	/** 0..1, Anteil fehlender Gesundheit – für Erschöpft-Idle und Atmungs-Additive. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Combat")
	float Exhaustion = 0.f;

private:
	UPROPERTY(Transient)
	TObjectPtr<class AEwigfelsCharacter> OwnerCharacter;

	// Im Game-Thread gesammelt
	FRotator ActorRotation = FRotator::ZeroRotator;
	FRotator AimRotation = FRotator::ZeroRotator;
	float PreviousYaw = 0.f;
	bool bHandReadyTarget = false;
};
