// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EwigfelsPlaceholderBody.generated.h"

class UMaterialInstanceDynamic;
class UPointLightComponent;
class UStaticMeshComponent;

/**
 * Platzhalter-Körper aus Grundformen (Robe, Kopf, Kapuze, leuchtende Hände/Augen), solange kein Mannequin/MetaHuman
 * zugewiesen ist. Bewegt sich ein wenig (Atmen, Schritt-Wippen, Hände heben beim Zaubern), damit Kampf und Zaubern
 * schon ohne Animationen lesbar sind. Blendet sich selbst aus, sobald die Figur ein Skeletal Mesh hat.
 *
 * Die Hände tragen die Tags "CastOriginR"/"CastOriginL" – daraus kommt die Magie (UEwigfelsSpellCasterComponent).
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsPlaceholderBody : public USceneComponent
{
	GENERATED_BODY()

public:
	UEwigfelsPlaceholderBody();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Farben: Robe, Akzent (Hände, Saum), Augen. Größe 1 = 180 cm. */
	void SetLook(const FLinearColor& InBody, const FLinearColor& InAccent, const FLinearColor& InEyes, float InHeight, bool bInHood);

	/** Hände nach vorne (0–1) und Handfarbe der aktuellen Weise. */
	void SetCastPose(float Alpha, const FLinearColor& Color, bool bTwoHanded);

	/** Vorwarnung (Gegner holt aus): Augen und Hände glühen. 0–1. */
	void SetTelegraph(float Alpha);

	/** Kurzer Treffer-Ruck + Aufblitzen. */
	void PlayHit();

	/** Umkippen und verblassen. */
	void PlayDeath();

	/** Nach Respawn wieder aufrecht. */
	void Revive();

	bool IsPlayingDeath() const { return DeathTime >= 0.f; }

	/** Schild-Glocke vor der Figur. */
	void SetShieldVisible(bool bVisible, const FLinearColor& Color);

	bool IsActiveBody() const { return bActive; }

private:
	UStaticMeshComponent* MakePart(FName Name, FName Shape, const FVector& Location, const FVector& Scale, USceneComponent* Parent);
	void RefreshMaterials();

	UPROPERTY(Transient) TObjectPtr<USceneComponent> Pivot;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> Robe;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> Torso;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> Head;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> Hood;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> EyeL;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> EyeR;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> HandL;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> HandR;
	UPROPERTY(Transient) TObjectPtr<UStaticMeshComponent> Shield;
	UPROPERTY(Transient) TObjectPtr<UPointLightComponent> HandLight;

	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> BodyMat;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> AccentMat;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> EyeMat;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> HandMat;
	UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> ShieldMat;

	FLinearColor BodyColor = FLinearColor(0.08f, 0.09f, 0.14f);
	FLinearColor AccentColor = FLinearColor(1.f, 0.8f, 0.45f);
	FLinearColor EyeColor = FLinearColor(1.f, 0.95f, 0.8f);
	FLinearColor CastColor = FLinearColor(1.f, 0.9f, 0.6f);
	float Height = 1.f;
	bool bHood = true;

	float CastAlpha = 0.f;
	float CastTarget = 0.f;
	bool bCastTwoHanded = false;
	float Telegraph = 0.f;
	float HitKick = 0.f;
	float DeathTime = -1.f;
	float Clock = 0.f;
	bool bActive = false;
	bool bBuilt = false;
};
