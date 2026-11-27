// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "World/EwigfelsInteractable.h"
#include "EwigfelsMirror.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

/**
 * Spiegel in einem Laden (oder im Turmzimmer). E drücken → Charakter-Editor im Laden-Modus: nur die Bereiche des
 * Ladens (Shops.json) sind änderbar, mit Preis. Der Server nimmt Laden-Änderungen nur an, wenn die Figur nahe an
 * einem Spiegel mit derselben ShopId steht (UEwigfelsAppearanceComponent::ShopMirrorRange).
 *
 * Im Level platzieren und ShopId setzen (schneiderei, haarstube, taetowierer, ringschmied, handschuhmacher,
 * spiegelhexe, turmzimmer). Ohne eigenes Mesh baut er sich aus Grundformen (Rahmen + leuchtendes Glas).
 */
UCLASS()
class EWIGFELS_API AEwigfelsMirror : public AActor, public IEwigfelsInteractable
{
	GENERATED_BODY()

public:
	AEwigfelsMirror();

	virtual void BeginPlay() override;

	virtual FText GetInteractPrompt_Implementation() const override;
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	/** Laden aus Content/Data/Appearance/Shops.json. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Look")
	FName ShopId = TEXT("schneiderei");

	/** Optionales Spiegel-Mesh (sonst Grundformen). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Look")
	TSoftObjectPtr<UStaticMesh> MirrorMesh;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Look")
	TObjectPtr<UStaticMeshComponent> Frame;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Look")
	TObjectPtr<UStaticMeshComponent> Glass;
};
