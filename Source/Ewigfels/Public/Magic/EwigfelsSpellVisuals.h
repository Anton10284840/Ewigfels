// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Magic/EwigfelsSpellTypes.h"
#include "EwigfelsSpellVisuals.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class UNiagaraComponent;
class UNiagaraSystem;
class UPointLightComponent;
class UPrimitiveComponent;
class UStaticMesh;
class UStaticMeshComponent;

/**
 * Kurzer Licht-Effekt (Aufprall, Welle, Parade). Rein kosmetisch, wird auf jeder Maschine lokal erzeugt,
 * nie repliziert. Platzhalter, bis die Niagara-Systeme aus Content/VFX/Spells da sind.
 */
UCLASS(NotPlaceable, Transient)
class EWIGFELS_API AEwigfelsSpellFlash : public AActor
{
	GENERATED_BODY()

public:
	AEwigfelsSpellFlash();

	void Setup(UStaticMesh* Shape, const FLinearColor& Color, const FVector& StartScale, const FVector& EndScale,
		float Duration, float LightIntensity, float LightRadius);

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<UPointLightComponent> Light;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> Material;

	FVector From = FVector::OneVector;
	FVector To = FVector::OneVector;
	float Length = 0.3f;
	float Age = 0.f;
	float PeakLight = 0.f;
	float PeakGlow = 30.f;
};

/** Hilfen für den Look der Weisen: Leucht-Material, Grundformen, Niagara mit Rückfall auf Platzhalter. */
namespace EwigfelsSpellVisuals
{
	/** M_EwigfelsGlow (vom Testmap-Skript erzeugt) oder BasicShapeMaterial. Parameter "Color", "Glow". */
	EWIGFELS_API UMaterialInterface* GetGlowMaterial();

	/** true, wenn das echte Leucht-Material da ist (sonst gibt es nur Punktlicht + Farbe). */
	EWIGFELS_API bool HasGlowMaterial();

	/** Leuchtend, additiv (Magie, Augen, Hände). Parameter: Color, Glow, Opacity. */
	EWIGFELS_API UMaterialInstanceDynamic* ApplyGlow(UPrimitiveComponent* Component, const FLinearColor& Color, float Glow);

	/** Fest, beleuchtet (Platzhalter-Körper, Arena). M_EwigfelsSolid (Color, Roughness) oder BasicShapeMaterial. */
	EWIGFELS_API UMaterialInstanceDynamic* ApplySolid(UPrimitiveComponent* Component, const FLinearColor& Color, float Roughness = 0.7f);

	/** "sphere", "cylinder", "cone", "cube" aus /Engine/BasicShapes. */
	EWIGFELS_API UStaticMesh* GetShape(FName Shape);

	/** Lädt ein Asset nur, wenn es existiert (ohne Warn-Spam), merkt sich das Ergebnis. */
	EWIGFELS_API UObject* LoadIfExists(const FSoftObjectPath& Path);

	EWIGFELS_API UNiagaraComponent* SpawnNiagaraAt(UWorld* World, const FSoftObjectPath& System, const FVector& Location,
		const FRotator& Rotation, const FLinearColor& Color, float Scale);

	EWIGFELS_API UNiagaraComponent* SpawnNiagaraAttached(USceneComponent* AttachTo, const FSoftObjectPath& System,
		const FLinearColor& Color, float Scale);

	EWIGFELS_API AEwigfelsSpellFlash* SpawnFlash(UWorld* World, FName Shape, const FVector& Location, const FRotator& Rotation,
		const FLinearColor& Color, const FVector& StartScale, const FVector& EndScale, float Duration, float LightIntensity, float LightRadius);

	/** Aufprall einer Weise: Niagara (impact_niagara) oder Licht-Platzhalter. Lokal. */
	EWIGFELS_API void PlayImpact(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Location, const FVector& Normal);

	/** Auslösen (Kegel-Welle, Area-Ring, Mündungsblitz an den Fingern). Lokal. */
	EWIGFELS_API void PlayRelease(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Origin, const FVector& Direction, const AActor* Caster);
}
