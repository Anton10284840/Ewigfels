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

/**
 * Prozeduraler Blitz (delivery "beam", bis es ein Niagara-System gibt): zackiger Hauptstrahl mit weißem Kern und
 * farbigem Saum, ein paar Verästelungen, flackert (neu gezackt alle ~35 ms) und verglüht in Sekundenbruchteilen.
 * Rein kosmetisch, lokal, nie repliziert.
 */
UCLASS(NotPlaceable, Transient)
class EWIGFELS_API AEwigfelsLightningBolt : public AActor
{
	GENERATED_BODY()

public:
	AEwigfelsLightningBolt();

	/** bStrong = letzter Schlag der Kette: dicker, mehr Äste, länger sichtbar. */
	void Setup(const FVector& Start, const FVector& End, const FLinearColor& Color, float Thickness, float Duration, bool bStrong);

	virtual void Tick(float DeltaSeconds) override;

	/** Reine Rechenfunktion (getestet): Zickzack-Punkte von A nach B, Enden fest, Ausschlag in der Mitte am größten. */
	static void BuildJaggedPath(FRandomStream& Rng, const FVector& A, const FVector& B, int32 Segments, float Amplitude, TArray<FVector>& OutPoints);

private:
	struct FBoltSegment
	{
		TObjectPtr<UStaticMeshComponent> Core;
		TObjectPtr<UStaticMeshComponent> Halo;
		FVector A = FVector::ZeroVector;
		FVector B = FVector::ZeroVector;
		float Width = 1.f;
	};

	void Rebuild();
	void PlaceSegments(float WidthScale);
	UStaticMeshComponent* MakePiece(const FLinearColor& Color, float Glow);

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<UPointLightComponent> StartLight;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<UPointLightComponent> EndLight;

	/** Hält die Teile am Leben (FBoltSegment ist kein USTRUCT). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> Pieces;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> Materials;

	TArray<FBoltSegment> Segments;
	FRandomStream Rng;
	FVector From = FVector::ZeroVector;
	FVector To = FVector::ZeroVector;
	FLinearColor BoltColor = FLinearColor::White;
	float BaseWidth = 6.f;
	float Length = 0.15f;
	float Age = 0.f;
	float NextJitter = 0.f;
	float PeakStartLight = 0.f;
	float PeakEndLight = 0.f;
	int32 Forks = 2;
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

	/** Strahl-Weise: Niagara (vfx.niagara mit User.BeamEnd) oder prozeduraler Blitz, dazu Mündungs- und Einschlagsblitz. Lokal. */
	EWIGFELS_API void PlayBeam(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Start, const FVector& End, const FVector& Normal,
		bool bHitTarget, uint8 ChainIndex);

	/** Auslösen (Kegel-Welle, Area-Ring, Mündungsblitz an den Fingern). Lokal. */
	EWIGFELS_API void PlayRelease(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Origin, const FVector& Direction, const AActor* Caster);
}
