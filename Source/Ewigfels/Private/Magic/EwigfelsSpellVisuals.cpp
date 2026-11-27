// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsSpellVisuals.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/PackageName.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

namespace
{
	const TCHAR* GlowMaterialPath = TEXT("/Game/VFX/Materials/M_EwigfelsGlow.M_EwigfelsGlow");
	const TCHAR* SolidMaterialPath = TEXT("/Game/VFX/Materials/M_EwigfelsSolid.M_EwigfelsSolid");
	const TCHAR* BasicMaterialPath = TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial");

	/** Ergebnis-Cache für LoadIfExists. Schwache Zeiger, damit nichts am Leben gehalten wird. */
	TMap<FSoftObjectPath, TWeakObjectPtr<UObject>>& LoadedCache()
	{
		static TMap<FSoftObjectPath, TWeakObjectPtr<UObject>> Cache;
		return Cache;
	}

	TSet<FSoftObjectPath>& MissingCache()
	{
		static TSet<FSoftObjectPath> Missing;
		return Missing;
	}
}

UObject* EwigfelsSpellVisuals::LoadIfExists(const FSoftObjectPath& Path)
{
	if (Path.IsNull())
	{
		return nullptr;
	}
	if (UObject* Already = Path.ResolveObject())
	{
		return Already;
	}
	if (const TWeakObjectPtr<UObject>* Cached = LoadedCache().Find(Path))
	{
		if (UObject* Obj = Cached->Get())
		{
			return Obj;
		}
	}
	if (MissingCache().Contains(Path))
	{
		return nullptr;
	}
	const FString Package = Path.GetLongPackageName();
	if (!Package.StartsWith(TEXT("/Engine/")) && !FPackageName::DoesPackageExist(Package))
	{
		MissingCache().Add(Path);
		return nullptr;
	}
	UObject* Loaded = Path.TryLoad();
	if (Loaded)
	{
		LoadedCache().Add(Path, Loaded);
	}
	else
	{
		MissingCache().Add(Path);
	}
	return Loaded;
}

bool EwigfelsSpellVisuals::HasGlowMaterial()
{
	return LoadIfExists(FSoftObjectPath(GlowMaterialPath)) != nullptr;
}

UMaterialInterface* EwigfelsSpellVisuals::GetGlowMaterial()
{
	if (UMaterialInterface* Glow = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(GlowMaterialPath))))
	{
		return Glow;
	}
	return Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(BasicMaterialPath)));
}

UMaterialInstanceDynamic* EwigfelsSpellVisuals::ApplyGlow(UPrimitiveComponent* Component, const FLinearColor& Color, float Glow)
{
	UMaterialInterface* Base = GetGlowMaterial();
	if (!Component || !Base)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Component);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
	MID->SetScalarParameterValue(TEXT("Glow"), Glow);
	Component->SetMaterial(0, MID);
	return MID;
}

UMaterialInstanceDynamic* EwigfelsSpellVisuals::ApplySolid(UPrimitiveComponent* Component, const FLinearColor& Color, float Roughness)
{
	UMaterialInterface* Base = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(SolidMaterialPath)));
	if (!Base)
	{
		Base = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(BasicMaterialPath)));
	}
	if (!Component || !Base)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Component);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
	MID->SetScalarParameterValue(TEXT("Roughness"), Roughness);
	Component->SetMaterial(0, MID);
	return MID;
}

UStaticMesh* EwigfelsSpellVisuals::GetShape(FName Shape)
{
	FString Name = TEXT("Sphere");
	if (Shape == TEXT("cylinder") || Shape == TEXT("lance")) { Name = TEXT("Cylinder"); }
	else if (Shape == TEXT("cone"))                        { Name = TEXT("Cone"); }
	else if (Shape == TEXT("cube"))                        { Name = TEXT("Cube"); }
	return Cast<UStaticMesh>(LoadIfExists(FSoftObjectPath(FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), *Name, *Name))));
}

UNiagaraComponent* EwigfelsSpellVisuals::SpawnNiagaraAt(UWorld* World, const FSoftObjectPath& System, const FVector& Location,
	const FRotator& Rotation, const FLinearColor& Color, float Scale)
{
	UNiagaraSystem* Asset = Cast<UNiagaraSystem>(LoadIfExists(System));
	if (!World || !Asset)
	{
		return nullptr;
	}
	UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Asset, Location, Rotation, FVector(Scale));
	if (Comp)
	{
		Comp->SetVariableLinearColor(TEXT("User.Color"), Color);
	}
	return Comp;
}

UNiagaraComponent* EwigfelsSpellVisuals::SpawnNiagaraAttached(USceneComponent* AttachTo, const FSoftObjectPath& System,
	const FLinearColor& Color, float Scale)
{
	UNiagaraSystem* Asset = Cast<UNiagaraSystem>(LoadIfExists(System));
	if (!AttachTo || !Asset)
	{
		return nullptr;
	}
	UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAttached(Asset, AttachTo, NAME_None, FVector::ZeroVector,
		FRotator::ZeroRotator, FVector(Scale), EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::None);
	if (Comp)
	{
		Comp->SetVariableLinearColor(TEXT("User.Color"), Color);
	}
	return Comp;
}

AEwigfelsSpellFlash* EwigfelsSpellVisuals::SpawnFlash(UWorld* World, FName Shape, const FVector& Location, const FRotator& Rotation,
	const FLinearColor& Color, const FVector& StartScale, const FVector& EndScale, float Duration, float LightIntensity, float LightRadius)
{
	if (!World || World->GetNetMode() == NM_DedicatedServer)
	{
		return nullptr;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AEwigfelsSpellFlash* Flash = World->SpawnActor<AEwigfelsSpellFlash>(AEwigfelsSpellFlash::StaticClass(), Location, Rotation, Params);
	if (Flash)
	{
		Flash->Setup(GetShape(Shape), Color, StartScale, EndScale, Duration, LightIntensity, LightRadius);
	}
	return Flash;
}

void EwigfelsSpellVisuals::PlayImpact(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Location, const FVector& Normal)
{
	const FRotator Rot = Normal.IsNearlyZero() ? FRotator::ZeroRotator : Normal.Rotation();
	if (SpawnNiagaraAt(World, Spell.Visual.ImpactNiagara, Location, Rot, Spell.Visual.Color, Spell.Visual.Scale))
	{
		return;
	}
	const float Size = Spell.Delivery.ExplodeRadius > 0.f ? Spell.Delivery.ExplodeRadius / 50.f : 0.9f * Spell.Visual.Scale;
	SpawnFlash(World, TEXT("sphere"), Location, Rot, Spell.Visual.Color, FVector(0.2f), FVector(Size), 0.28f,
		Spell.Delivery.ExplodeRadius > 0.f ? 20000.f : 4000.f, FMath::Max(400.f, Spell.Delivery.ExplodeRadius * 3.f));
}

void EwigfelsSpellVisuals::PlayRelease(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Origin, const FVector& Direction, const AActor* Caster)
{
	const FVector Flat = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
	switch (Spell.Delivery.Type)
	{
	case EEwigfelsSpellDelivery::Cone:
	{
		const FVector Base = Caster ? Caster->GetActorLocation() - FVector(0.f, 0.f, 60.f) : Origin;
		if (SpawnNiagaraAt(World, Spell.Visual.Niagara, Base, Flat.Rotation(), Spell.Visual.Color, Spell.Visual.Scale))
		{
			return;
		}
		// Platzhalter: flache Druckwelle, die nach vorne aufzieht (Zylinder liegt flach, wächst in Blickrichtung)
		const float Width = Spell.Delivery.Range * FMath::Tan(FMath::DegreesToRadians(Spell.Delivery.Angle * 0.5f)) / 50.f;
		SpawnFlash(World, TEXT("cylinder"), Base + Flat * Spell.Delivery.Range * 0.5f, Flat.Rotation(), Spell.Visual.Color,
			FVector(0.5f, 0.5f, 0.05f), FVector(Spell.Delivery.Range / 100.f, Width, 0.12f), 0.35f, 25000.f, Spell.Delivery.Range * 1.2f);
		SpawnFlash(World, TEXT("sphere"), Origin, FRotator::ZeroRotator, Spell.Visual.Color, FVector(0.1f), FVector(1.2f), 0.2f, 8000.f, 500.f);
		break;
	}
	case EEwigfelsSpellDelivery::Area:
	{
		const FVector Base = Caster ? Caster->GetActorLocation() - FVector(0.f, 0.f, 80.f) : Origin;
		if (!SpawnNiagaraAt(World, Spell.Visual.Niagara, Base, FRotator::ZeroRotator, Spell.Visual.Color, Spell.Visual.Scale))
		{
			SpawnFlash(World, TEXT("cylinder"), Base, FRotator::ZeroRotator, Spell.Visual.Color, FVector(0.2f, 0.2f, 0.04f),
				FVector(Spell.Delivery.Radius / 50.f, Spell.Delivery.Radius / 50.f, 0.06f), 0.4f, 20000.f, Spell.Delivery.Radius * 1.5f);
		}
		break;
	}
	case EEwigfelsSpellDelivery::Projectile:
		// Mündungsblitz an den Fingerspitzen
		SpawnFlash(World, TEXT("sphere"), Origin, FRotator::ZeroRotator, Spell.Visual.Color, FVector(0.05f), FVector(0.45f * Spell.Visual.Scale),
			0.12f, 2500.f, 300.f);
		break;
	default:
		break;
	}
}

// ---------------------------------------------------------------------------

AEwigfelsSpellFlash::AEwigfelsSpellFlash()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;
	SetCanBeDamaged(false);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCastShadow(false);
	Mesh->SetMobility(EComponentMobility::Movable);
	RootComponent = Mesh;

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(Mesh);
	Light->SetCastShadows(false);
	Light->SetIntensityUnits(ELightUnits::Lumens);
}

void AEwigfelsSpellFlash::Setup(UStaticMesh* Shape, const FLinearColor& Color, const FVector& StartScale, const FVector& EndScale,
	float Duration, float LightIntensity, float LightRadius)
{
	if (Shape)
	{
		Mesh->SetStaticMesh(Shape);
	}
	Material = EwigfelsSpellVisuals::ApplyGlow(Mesh, Color, PeakGlow);
	From = StartScale;
	To = EndScale;
	Length = FMath::Max(0.05f, Duration);
	PeakLight = LightIntensity;
	Light->SetLightColor(Color);
	Light->SetIntensity(LightIntensity);
	Light->SetAttenuationRadius(LightRadius);
	SetActorScale3D(From);
	SetLifeSpan(Length + 0.1f);
}

void AEwigfelsSpellFlash::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Age += DeltaSeconds;
	const float T = FMath::Clamp(Age / Length, 0.f, 1.f);
	const float Ease = 1.f - FMath::Square(1.f - T); // schnell auf, weich aus
	SetActorScale3D(FMath::Lerp(From, To, Ease));
	const float Fade = 1.f - T;
	Light->SetIntensity(PeakLight * Fade * Fade);
	if (Material)
	{
		Material->SetScalarParameterValue(TEXT("Glow"), PeakGlow * Fade);
		Material->SetScalarParameterValue(TEXT("Opacity"), Fade);
	}
	if (T >= 1.f)
	{
		Destroy();
	}
}
