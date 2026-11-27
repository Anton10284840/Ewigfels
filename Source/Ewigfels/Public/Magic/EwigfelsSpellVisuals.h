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
 * Prozeduraler Blitz (delivery "beam", bis es ein Niagara-System gibt). Gleiche Technik wie die Web-Vorschau:
 * Hauptpfad per Midpoint Displacement, Äste an ~22 % der Segmente (Tiefe 2), drei additive Schichten
 * (weiter weicher Saum, mittlerer Glow, fast weißer Kern), 2–4× neu gezackt, Helligkeit springt 60–100 %,
 * danach 150–300 ms bläuliches Nachbild. Rein kosmetisch, lokal, nie repliziert.
 */
UCLASS(NotPlaceable, Transient)
class EWIGFELS_API AEwigfelsLightningBolt : public AActor
{
	GENERATED_BODY()

public:
	AEwigfelsLightningBolt();

	/** Width = Stärke (1 normal, 1.6 dritter Kettenschlag, 2.5 Lanze). */
	void Setup(const FVector& Start, const FVector& End, const FLinearColor& Color, float Width, float Duration, int32 Regenerations);

	/** Vor Setup aufrufen: Saumfarbe (2. Grundton), Zackigkeit (1 = Blitz, 0.2 = ruhiges Band), Äste an/aus. */
	void SetStyle(const FLinearColor& InGlowColor, float InJaggedness, bool bInBranches);

	virtual void Tick(float DeltaSeconds) override;

	/** Reine Rechenfunktion (getestet): Zickzack-Punkte von A nach B, Enden fest, Ausschlag in der Mitte am größten. */
	static void BuildJaggedPath(FRandomStream& Rng, const FVector& A, const FVector& B, int32 Segments, float Amplitude, TArray<FVector>& OutPoints);

	/** Reine Rechenfunktion (getestet): Midpoint Displacement, 2^Levels Segmente, Versatz halbiert sich pro Ebene. */
	static void MidpointDisplace(FRandomStream& Rng, const FVector& A, const FVector& B, int32 Levels, float Amount, TArray<FVector>& OutPoints);

private:
	struct FBoltPath
	{
		TArray<FVector> Points;
		float Width = 1.f;
		float Alpha = 1.f;
	};
	struct FBoltPiece
	{
		TObjectPtr<UStaticMeshComponent> Outer;
		TObjectPtr<UStaticMeshComponent> Mid;
		TObjectPtr<UStaticMeshComponent> Core;
		TObjectPtr<UMaterialInstanceDynamic> OuterMat;
		TObjectPtr<UMaterialInstanceDynamic> MidMat;
		TObjectPtr<UMaterialInstanceDynamic> CoreMat;
	};

	void Generate();
	void GrowBranches(const TArray<FVector>& Points, int32 Depth);
	void Place(float Brightness, bool bAfterglow);
	UStaticMeshComponent* MakePiece(const FLinearColor& Color, float Glow, UMaterialInstanceDynamic*& OutMat);

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<UPointLightComponent> StartLight;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<UPointLightComponent> EndLight;

	/** Hält Teile und Materialien am Leben (FBoltPiece ist kein USTRUCT). */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Keep;

	TArray<FBoltPath> Paths;
	TArray<FBoltPiece> Pool;
	FRandomStream Rng;
	FVector From = FVector::ZeroVector;
	FVector To = FVector::ZeroVector;
	FLinearColor BoltColor = FLinearColor::White;
	FLinearColor GlowColor = FLinearColor::White;
	FLinearColor AfterglowColor = FLinearColor(0.35f, 0.5f, 1.f);
	bool bHasGlowColor = false;
	bool bBranches = true;
	float Jaggedness = 1.f;
	float BaseWidth = 1.f;
	float Life = 0.2f;
	float AfterLife = 0.2f;
	float Age = 0.f;
	float NextRegen = 0.f;
	float Bright = 1.f;
	int32 RegensLeft = 3;
	int32 Regens = 3;
};


/** Funkenregen am Einschlag: schnelle Funken mit Schwerkraft und kurzen Leuchtspuren, prallen am Boden ab. Lokal. */
UCLASS(NotPlaceable, Transient)
class EWIGFELS_API AEwigfelsSparkBurst : public AActor
{
	GENERATED_BODY()

public:
	AEwigfelsSparkBurst();
	void Setup(const FVector& Location, const FVector& Normal, const FLinearColor& Color, int32 Count, float Speed);
	virtual void Tick(float DeltaSeconds) override;

private:
	struct FSpark
	{
		FVector Pos = FVector::ZeroVector;
		FVector Vel = FVector::ZeroVector;
		float Life = 0.4f;
		TObjectPtr<UStaticMeshComponent> Comp;
		TObjectPtr<UMaterialInstanceDynamic> Mat;
	};

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|VFX")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> Keep;

	TArray<FSpark> Sparks;
	float FloorZ = 0.f;
	float Age = 0.f;
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

	/** Strahl-Material M_EwigfelsBeam (weicher Rand, Mitte hell) oder M_EwigfelsGlow. Parameter: Color, Glow, Opacity. */
	EWIGFELS_API UMaterialInstanceDynamic* ApplyBeam(UPrimitiveComponent* Component, const FLinearColor& Color, float Glow);

	/** Kurzer Vollbild-Aufheller für den lokalen Spieler (0–0.25), 30–80 ms. */
	EWIGFELS_API void FlashScreen(UWorld* World, float Alpha, float Seconds);

	/** Fest, beleuchtet (Platzhalter-Körper, Arena). M_EwigfelsSolid (Color, Roughness) oder BasicShapeMaterial. */
	EWIGFELS_API UMaterialInstanceDynamic* ApplySolid(UPrimitiveComponent* Component, const FLinearColor& Color, float Roughness = 0.7f);

	/**
	 * Echte PBR-Oberfläche /Game/Environment/Materials/MI_Ewigfels_<Surface> (Ground, Wall, Pillar, Rock, Paving;
	 * angelegt von Scripts/Python/create_photoreal.py), sonst ApplySolid mit Fallback-Farbe. Tint färbt leicht ein.
	 */
	EWIGFELS_API UMaterialInstanceDynamic* ApplySurface(UPrimitiveComponent* Component, FName Surface, const FLinearColor& Fallback,
		float Roughness = 0.7f, const FLinearColor& Tint = FLinearColor::White);

	/** Klares Eis mit Lichtbrechung (M_EwigfelsIce) oder Leucht-Material. Parameter: Color, Glow. */
	EWIGFELS_API UMaterialInstanceDynamic* ApplyIce(UPrimitiveComponent* Component, const FLinearColor& Color);

	/** Hitzeflimmern (M_EwigfelsHeat). nullptr, wenn das Material fehlt (dann kein Flimmern). Parameter: Strength. */
	EWIGFELS_API UMaterialInstanceDynamic* ApplyHeat(UPrimitiveComponent* Component, float Strength);

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

	/** Schwerer Aufprall am Boden (Schmettern, Wurf): Druckring, Staub-/Funkenregen, Licht. Lokal. */
	EWIGFELS_API void PlayLandImpact(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Location, bool bBig);

	/** Auslösen (Kegel-Welle, Area-Ring, Mündungsblitz an den Fingern). Lokal. */
	EWIGFELS_API void PlayRelease(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Origin, const FVector& Direction, const AActor* Caster);
}
