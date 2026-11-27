// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EwigfelsLockOnComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEwigfelsLockOnChanged, AActor*, NewTarget);

/**
 * Zielaufschaltung (GDD §5): wählt den Gegner, der am nächsten an der Bildmitte ist, hält die Kamera darauf,
 * wechselt per Mausrad/Stick. Rein lokal (jeder Spieler hat sein eigenes Ziel); der Server bekommt das Ziel
 * nur mit jeder Weise mitgeschickt.
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsLockOnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsLockOnComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|LockOn")
	void Toggle();

	/** +1 = nächstes Ziel rechts, -1 = links. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|LockOn")
	void Switch(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|LockOn")
	void Release();

	UFUNCTION(BlueprintPure, Category = "Ewigfels|LockOn")
	AActor* GetTarget() const { return Target.Get(); }

	/** Bestes Ziel für weiche Zielhilfe ohne Lock-On (kleiner Kegel um die Bildmitte). */
	AActor* FindSoftTarget(float MaxAngleDeg) const;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|LockOn")
	FEwigfelsLockOnChanged OnTargetChanged;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|LockOn")
	float MaxDistance = 2800.f;

	/** Ab dieser Entfernung löst sich die Aufschaltung. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|LockOn")
	float BreakDistance = 3400.f;

	/** Wie stark die Kamera zum Ziel dreht (höher = fester). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|LockOn")
	float CameraInterpSpeed = 8.f;

private:
	TArray<AActor*> GatherCandidates() const;
	float Score(const AActor* Candidate, const FVector& ViewLoc, const FVector& ViewDir) const;
	bool GetView(FVector& OutLoc, FRotator& OutRot) const;
	void SetTarget(AActor* NewTarget);

	TWeakObjectPtr<AActor> Target;
};
