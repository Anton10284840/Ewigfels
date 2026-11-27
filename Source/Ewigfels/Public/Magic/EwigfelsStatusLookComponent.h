// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EwigfelsStatusLookComponent.generated.h"

class UMaterialInstanceDynamic;
class UStaticMeshComponent;

/**
 * Zeigt Status am Ziel (Status.json "look"): "ice" Eisblock + Animation steht, "notes" kreisende Noten über dem Kopf,
 * "flames" flackernde Flammen, "rings" Schweberinge unter den Füßen. Neue Looks: hier einen Fall ergänzen.
 *
 * Rein kosmetisch: läuft auf jeder Maschine lokal (nicht auf dem Dedicated Server) und folgt dem replizierten
 * Status-Array der Kampfkomponente (OnStatusesChanged). Wird von UEwigfelsCombatComponent selbst angehängt.
 */
UCLASS(ClassGroup = (Ewigfels))
class EWIGFELS_API UEwigfelsStatusLookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsStatusLookComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Gleicht die sichtbaren Looks mit den aktiven Status ab. */
	UFUNCTION()
	void Refresh();

private:
	struct FLookPiece
	{
		TObjectPtr<UStaticMeshComponent> Comp;
		TObjectPtr<UMaterialInstanceDynamic> Mat;
		FVector Offset = FVector::ZeroVector;
		FVector BaseScale = FVector::OneVector;
		float Phase = 0.f;
	};
	struct FLook
	{
		FName Look;
		TArray<FLookPiece> Pieces;
	};

	void BuildLook(FLook& Look, const FLinearColor& Color);
	void DestroyLook(FLook& Look);
	void SetAnimsPaused(bool bPaused);
	void GetBodySize(float& OutRadius, float& OutHalfHeight) const;

	/** Hält die Teile am Leben (FLookPiece ist kein USTRUCT). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Keep;

	TArray<FLook> Looks;
	float Age = 0.f;
	bool bPausedAnims = false;
};
