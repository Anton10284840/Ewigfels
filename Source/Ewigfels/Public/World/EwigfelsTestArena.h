// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EwigfelsTestArena.generated.h"

class UStaticMeshComponent;
class UDirectionalLightComponent;
class USkyLightComponent;
class USkyAtmosphereComponent;
class UExponentialHeightFogComponent;
class UPostProcessComponent;
class UPointLightComponent;

/** Eine Gegnerwelle: welche Typen, wie viele. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsArenaWave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Arena")
	TArray<FName> Creatures;
};

/**
 * Testarena für Phase 3: Steinplatz bei goldener Stunde mit Säulenring, Deckung, Licht/Himmel/Nebel,
 * Übungspuppen, einem Gesprächspartner (Lio, Dialog-Kamera) und Gegnerwellen.
 * Baut sich komplett aus Engine-Grundformen – braucht keine Assets. Der GameMode setzt sie automatisch in leere Level.
 *
 * Konsole: Ewigfels.Arena.Wave (nächste Welle sofort) · Ewigfels.Arena.Clear (alle Gegner weg)
 */
UCLASS()
class EWIGFELS_API AEwigfelsTestArena : public AActor
{
	GENERATED_BODY()

public:
	AEwigfelsTestArena();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	/** Startpunkt des Spielers (Weltkoordinaten). */
	FTransform GetPlayerSpawn() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Arena")
	void SpawnNextWave();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Arena")
	void ClearEnemies();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Arena")
	TArray<FEwigfelsArenaWave> Waves;

	/** Pause zwischen zwei Wellen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Arena")
	float WaveDelay = 6.f;

	/** Wellen automatisch starten (aus = nur per Konsole). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Arena")
	bool bAutoWaves = true;

	/** Erste Welle erst nach dieser Zeit (Zeit zum Ausprobieren an den Puppen). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Arena")
	float FirstWaveDelay = 20.f;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<USceneComponent> Root;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<UStaticMeshComponent> Floor;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<UStaticMeshComponent> Dais;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<UStaticMeshComponent> DaisRing;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TArray<TObjectPtr<UStaticMeshComponent>> Pillars;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TArray<TObjectPtr<UStaticMeshComponent>> PillarGlows;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TArray<TObjectPtr<UPointLightComponent>> PillarLights;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TArray<TObjectPtr<UStaticMeshComponent>> Walls;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TArray<TObjectPtr<UStaticMeshComponent>> Cover;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<UDirectionalLightComponent> Sun;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<USkyLightComponent> SkyLight;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<USkyAtmosphereComponent> Atmosphere;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<UExponentialHeightFogComponent> Fog;
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Arena") TObjectPtr<UPostProcessComponent> PostProcess;

private:
	void CheckWave();
	void SpawnStatic();
	class AEwigfelsEnemy* SpawnEnemy(FName CreatureId, const FVector& Location, const FRotator& Rotation);

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> WaveEnemies;

	int32 NextWave = 0;
	float WaveClearedTime = -1.f;
	FTimerHandle WaveTimer;
};
