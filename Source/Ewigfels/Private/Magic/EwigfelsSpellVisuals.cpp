// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsSpellVisuals.h"
#include "Components/MaterialBillboardComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Misc/PackageName.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

namespace
{
	const TCHAR* BeamMaterialPath = TEXT("/Game/VFX/Materials/M_EwigfelsBeam.M_EwigfelsBeam");
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

UMaterialInstanceDynamic* EwigfelsSpellVisuals::ApplyBeam(UPrimitiveComponent* Component, const FLinearColor& Color, float Glow)
{
	UMaterialInterface* Base = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(BeamMaterialPath)));
	if (!Base)
	{
		return ApplyGlow(Component, Color, Glow);
	}
	if (!Component)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Component);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
	MID->SetScalarParameterValue(TEXT("Glow"), Glow);
	Component->SetMaterial(0, MID);
	return MID;
}

void EwigfelsSpellVisuals::FlashScreen(UWorld* World, float Alpha, float Seconds)
{
	if (!World || Alpha <= 0.f)
	{
		return;
	}
	APlayerController* PC = World->GetFirstPlayerController();
	if (PC && PC->IsLocalController())
	{
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->StartCameraFade(FMath::Min(Alpha, 0.25f), 0.f, FMath::Clamp(Seconds, 0.03f, 0.08f),
				FLinearColor(0.82f, 0.9f, 1.f), false, false);
		}
	}
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

UMaterialInstanceDynamic* EwigfelsSpellVisuals::ApplySurface(UPrimitiveComponent* Component, FName Surface, const FLinearColor& Fallback,
	float Roughness, const FLinearColor& Tint)
{
	const FString Name = FString::Printf(TEXT("MI_Ewigfels_%s"), *Surface.ToString());
	UMaterialInterface* Base = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(
		FString::Printf(TEXT("/Game/Environment/Materials/%s.%s"), *Name, *Name))));
	if (!Component || !Base)
	{
		return ApplySolid(Component, Fallback, Roughness);
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Component);
	MID->SetVectorParameterValue(TEXT("Tint"), Tint);
	Component->SetMaterial(0, MID);
	return MID;
}

UMaterialInstanceDynamic* EwigfelsSpellVisuals::ApplyIce(UPrimitiveComponent* Component, const FLinearColor& Color)
{
	UMaterialInterface* Base = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(TEXT("/Game/VFX/Materials/M_EwigfelsIce.M_EwigfelsIce"))));
	if (!Component || !Base)
	{
		return ApplyGlow(Component, Color, 1.6f);
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Component);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
	Component->SetMaterial(0, MID);
	return MID;
}

UMaterialInstanceDynamic* EwigfelsSpellVisuals::ApplyHeat(UPrimitiveComponent* Component, float Strength)
{
	UMaterialInterface* Base = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(TEXT("/Game/VFX/Materials/M_EwigfelsHeat.M_EwigfelsHeat"))));
	if (!Component || !Base)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Component);
	MID->SetScalarParameterValue(TEXT("Strength"), Strength);
	Component->SetMaterial(0, MID);
	return MID;
}

UMaterialInstanceDynamic* EwigfelsSpellVisuals::AddHalo(USceneComponent* AttachTo, const FLinearColor& Color, float SizeCm, float Glow)
{
	UMaterialInterface* Base = Cast<UMaterialInterface>(LoadIfExists(FSoftObjectPath(TEXT("/Game/VFX/Materials/M_EwigfelsSoftSprite.M_EwigfelsSoftSprite"))));
	AActor* Owner = AttachTo ? AttachTo->GetOwner() : nullptr;
	if (!Base || !Owner)
	{
		return nullptr;
	}
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Owner);
	MID->SetVectorParameterValue(TEXT("Color"), Color);
	MID->SetScalarParameterValue(TEXT("Glow"), Glow);
	UMaterialBillboardComponent* Halo = NewObject<UMaterialBillboardComponent>(Owner);
	Halo->SetupAttachment(AttachTo);
	Halo->SetUsingAbsoluteScale(true); // Größe in cm, unabhängig vom Wachsen der Form
	Halo->SetCastShadow(false);
	Halo->RegisterComponent();
	Halo->AddElement(MID, nullptr, false, SizeCm, SizeCm, nullptr);
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

void EwigfelsSpellVisuals::SetNiagaraColors(UNiagaraComponent* Comp, const FLinearColor& Color, const FLinearColor& Glow, const FLinearColor& Spark)
{
	if (!Comp)
	{
		return;
	}
	// Alle drei Akkord-Farben; Systeme nutzen, was sie brauchen. A = 0 heißt „nicht gesetzt“ = Kernfarbe.
	Comp->SetVariableLinearColor(TEXT("User.Color"), Color);
	Comp->SetVariableLinearColor(TEXT("User.GlowColor"), Glow.A > 0.f ? Glow : Color);
	Comp->SetVariableLinearColor(TEXT("User.SparkColor"), Spark.A > 0.f ? Spark : Color);
}

UNiagaraComponent* EwigfelsSpellVisuals::SpawnNiagaraAt(UWorld* World, const FSoftObjectPath& System, const FVector& Location,
	const FRotator& Rotation, const FLinearColor& Color, float Scale, const FLinearColor& Glow, const FLinearColor& Spark)
{
	UNiagaraSystem* Asset = Cast<UNiagaraSystem>(LoadIfExists(System));
	if (!World || !Asset || World->GetNetMode() == NM_DedicatedServer)
	{
		return nullptr;
	}
	UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Asset, Location, Rotation, FVector(Scale));
	SetNiagaraColors(Comp, Color, Glow, Spark);
	return Comp;
}

UNiagaraComponent* EwigfelsSpellVisuals::SpawnNiagaraAttached(USceneComponent* AttachTo, const FSoftObjectPath& System,
	const FLinearColor& Color, float Scale, const FLinearColor& Glow, const FLinearColor& Spark)
{
	UNiagaraSystem* Asset = Cast<UNiagaraSystem>(LoadIfExists(System));
	if (!AttachTo || !Asset)
	{
		return nullptr;
	}
	UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAttached(Asset, AttachTo, NAME_None, FVector::ZeroVector,
		FRotator::ZeroRotator, FVector(Scale), EAttachLocation::KeepRelativeOffset, true, ENCPoolMethod::None);
	SetNiagaraColors(Comp, Color, Glow, Spark);
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
	if (SpawnNiagaraAt(World, Spell.Visual.ImpactNiagara, Location, Rot, Spell.Visual.Color, Spell.Visual.Scale, Spell.Visual.GlowColor, Spell.Visual.SparkColor))
	{
		return;
	}
	const float Size = Spell.Delivery.ExplodeRadius > 0.f ? Spell.Delivery.ExplodeRadius / 50.f : 0.9f * Spell.Visual.Scale;
	SpawnFlash(World, TEXT("sphere"), Location, Rot, Spell.Visual.Color, FVector(0.2f), FVector(Size), 0.28f,
		Spell.Delivery.ExplodeRadius > 0.f ? 20000.f : 4000.f, FMath::Max(400.f, Spell.Delivery.ExplodeRadius * 3.f));
}

void EwigfelsSpellVisuals::PlayBeam(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Start, const FVector& End,
	const FVector& Normal, bool bHitTarget, uint8 ChainIndex)
{
	if (!World || World->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	const bool bStrong = ChainIndex + 1 >= Spell.Delivery.Chain && Spell.Delivery.Chain > 1;
	const FVector Dir = (End - Start).GetSafeNormal();
	const float Scale = Spell.Visual.Scale * (bStrong ? 1.6f : 1.f);

	if (UNiagaraComponent* Beam = SpawnNiagaraAt(World, Spell.Visual.Niagara, Start, Dir.Rotation(), Spell.Visual.Color, Scale, Spell.Visual.GlowColor, Spell.Visual.SparkColor))
	{
		Beam->SetVariableVec3(TEXT("User.BeamEnd"), End);
	}
	else
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (AEwigfelsLightningBolt* Bolt = World->SpawnActor<AEwigfelsLightningBolt>(AEwigfelsLightningBolt::StaticClass(), FTransform::Identity, Params))
		{
			if (Spell.Visual.BeamStyle == TEXT("tether"))
			{
				// Klangband (Ziehklang): ruhig gewellt, ohne Äste, pulsiert länger
				Bolt->SetStyle(Spell.Visual.GlowColor, 0.22f, false);
				Bolt->Setup(Start, End, Spell.Visual.Color, Scale * 2.f, 0.38f, 5);
			}
			else
			{
				Bolt->SetStyle(Spell.Visual.GlowColor, 1.f, true);
				Bolt->Setup(Start, End, Spell.Visual.Color, Scale * 1.6f, bStrong ? 0.26f : FMath::FRandRange(0.18f, 0.24f), bStrong ? 4 : 2);
			}
		}
	}

	// Mündungsblitz: kurzer Funke vor den Fingern (etwas vor der Hand, schwaches Licht, damit die Figur nicht weiß wird)
	SpawnFlash(World, TEXT("sphere"), Start + Dir * 18.f, FRotator::ZeroRotator, FLinearColor::LerpUsingHSV(Spell.Visual.Color, FLinearColor::White, 0.4f),
		FVector(0.05f), FVector(0.18f * Scale), 0.08f, 1200.f, 220.f);

	// Einschlag: Niagara oder Lichtkugel; bei Treffer stärker, beim Kettenende mit Druckring
	const FRotator Rot = Normal.IsNearlyZero() ? FRotator::ZeroRotator : Normal.Rotation();
	if (!SpawnNiagaraAt(World, Spell.Visual.ImpactNiagara, End, Rot, Spell.Visual.Color, Scale, Spell.Visual.GlowColor, Spell.Visual.SparkColor))
	{
		SpawnFlash(World, TEXT("sphere"), End, Rot, Spell.Visual.Color, FVector(0.15f), FVector((bHitTarget ? 0.9f : 0.55f) * Scale),
			bStrong ? 0.3f : 0.2f, bHitTarget ? 14000.f : 6000.f, bStrong ? 900.f : 550.f);
		if (AEwigfelsSparkBurst* Sparks = World->SpawnActor<AEwigfelsSparkBurst>(AEwigfelsSparkBurst::StaticClass(), FTransform(End)))
		{
			Sparks->Setup(End, Normal, Spell.Visual.SparkColor, bStrong ? 55 : (bHitTarget ? 30 : 20), bStrong ? 900.f : 650.f);
		}
		if (bStrong)
		{
			// flacher Ring senkrecht zur Schlagrichtung
			const FRotator RingRot = FRotationMatrix::MakeFromZ(Dir).Rotator();
			SpawnFlash(World, TEXT("cylinder"), End, RingRot, Spell.Visual.GlowColor, FVector(0.2f, 0.2f, 0.01f), FVector(2.4f, 2.4f, 0.015f),
				0.28f, 0.f, 0.f);
		}
	}
}

void EwigfelsSpellVisuals::PlayLandImpact(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Location, bool bBig)
{
	if (!World || World->GetNetMode() == NM_DedicatedServer)
	{
		return;
	}
	const float Radius = FMath::Max(Spell.LandRadius, 150.f) * (bBig ? 1.f : 0.6f);
	// flacher Druckring am Boden in Glühfarbe, heller Kern, Funken/Splitter in der Funkenfarbe
	SpawnFlash(World, TEXT("cylinder"), Location + FVector(0.f, 0.f, 4.f), FRotator::ZeroRotator, Spell.Visual.GlowColor,
		FVector(0.3f, 0.3f, 0.02f), FVector(Radius / 50.f, Radius / 50.f, 0.03f), 0.38f, 30000.f, Radius * 2.5f);
	SpawnFlash(World, TEXT("sphere"), Location + FVector(0.f, 0.f, 30.f), FRotator::ZeroRotator,
		FLinearColor::LerpUsingHSV(Spell.Visual.Color, FLinearColor::White, 0.5f), FVector(0.2f), FVector(1.6f), 0.16f, 0.f, 0.f);
	if (AEwigfelsSparkBurst* Sparks = World->SpawnActor<AEwigfelsSparkBurst>(AEwigfelsSparkBurst::StaticClass(), FTransform(Location)))
	{
		Sparks->Setup(Location + FVector(0.f, 0.f, 10.f), FVector::UpVector, Spell.Visual.SparkColor, bBig ? 60 : 30, bBig ? 1000.f : 700.f);
	}
	FlashScreen(World, bBig ? 0.12f : 0.06f, 0.06f);
}

void EwigfelsSpellVisuals::PlayRelease(UWorld* World, const FEwigfelsSpellDef& Spell, const FVector& Origin, const FVector& Direction, const AActor* Caster)
{
	const FVector Flat = FVector(Direction.X, Direction.Y, 0.f).GetSafeNormal();
	switch (Spell.Delivery.Type)
	{
	case EEwigfelsSpellDelivery::Cone:
	{
		const FVector Base = Caster ? Caster->GetActorLocation() - FVector(0.f, 0.f, 60.f) : Origin;
		if (SpawnNiagaraAt(World, Spell.Visual.Niagara, Base, Flat.Rotation(), Spell.Visual.Color, Spell.Visual.Scale, Spell.Visual.GlowColor, Spell.Visual.SparkColor))
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
		if (!SpawnNiagaraAt(World, Spell.Visual.Niagara, Base, FRotator::ZeroRotator, Spell.Visual.Color, Spell.Visual.Scale, Spell.Visual.GlowColor, Spell.Visual.SparkColor))
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

AEwigfelsLightningBolt::AEwigfelsLightningBolt()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;
	SetCanBeDamaged(false);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Movable);
	RootComponent = Root;

	StartLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("StartLight"));
	EndLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("EndLight"));
	for (UPointLightComponent* L : { StartLight.Get(), EndLight.Get() })
	{
		L->SetupAttachment(Root);
		L->SetMobility(EComponentMobility::Movable);
		L->SetCastShadows(false);
		L->SetIntensityUnits(ELightUnits::Lumens);
	}
}

void AEwigfelsLightningBolt::BuildJaggedPath(FRandomStream& InRng, const FVector& A, const FVector& B, int32 NumSegments, float Amplitude,
	TArray<FVector>& OutPoints)
{
	OutPoints.Reset();
	NumSegments = FMath::Max(1, NumSegments);
	const FVector Dir = (B - A).GetSafeNormal();
	FVector Side;
	FVector Up;
	Dir.FindBestAxisVectors(Side, Up);
	OutPoints.Add(A);
	for (int32 i = 1; i < NumSegments; ++i)
	{
		const float T = static_cast<float>(i) / NumSegments;
		const float Envelope = FMath::Sin(T * PI);
		const FVector Offset = (Side * InRng.FRandRange(-1.f, 1.f) + Up * InRng.FRandRange(-1.f, 1.f)) * Amplitude * Envelope;
		OutPoints.Add(FMath::Lerp(A, B, T) + Offset);
	}
	OutPoints.Add(B);
}

void AEwigfelsLightningBolt::MidpointDisplace(FRandomStream& InRng, const FVector& A, const FVector& B, int32 Levels, float Amount,
	TArray<FVector>& OutPoints)
{
	OutPoints.Reset();
	OutPoints.Add(A);
	OutPoints.Add(B);
	float D = Amount;
	TArray<FVector> Next;
	for (int32 L = 0; L < FMath::Clamp(Levels, 0, 8); ++L)
	{
		Next.Reset(OutPoints.Num() * 2);
		Next.Add(OutPoints[0]);
		for (int32 i = 0; i + 1 < OutPoints.Num(); ++i)
		{
			const FVector& P = OutPoints[i];
			const FVector& Q = OutPoints[i + 1];
			const FVector Seg = (Q - P).GetSafeNormal();
			FVector Side;
			FVector Up;
			Seg.FindBestAxisVectors(Side, Up);
			// senkrecht zum Segment versetzen, in beliebiger Richtung um die Achse (3D statt nur seitlich)
			const float Spin = InRng.FRandRange(0.f, 2.f * PI);
			const FVector Perp = Side * FMath::Cos(Spin) + Up * FMath::Sin(Spin);
			Next.Add((P + Q) * 0.5f + Perp * InRng.FRandRange(-1.f, 1.f) * D);
			Next.Add(Q);
		}
		Swap(OutPoints, Next);
		D *= 0.5f; // Versatz halbiert sich pro Ebene
	}
}

UStaticMeshComponent* AEwigfelsLightningBolt::MakePiece(const FLinearColor& Color, float Glow, UMaterialInstanceDynamic*& OutMat)
{
	UStaticMeshComponent* C = NewObject<UStaticMeshComponent>(this);
	C->SetStaticMesh(EwigfelsSpellVisuals::GetShape(TEXT("cylinder")));
	C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	C->SetCastShadow(false);
	C->SetMobility(EComponentMobility::Movable);
	C->SetupAttachment(Root);
	C->RegisterComponent();
	OutMat = EwigfelsSpellVisuals::ApplyBeam(C, Color, Glow);
	Keep.Add(C);
	if (OutMat)
	{
		Keep.Add(OutMat);
	}
	return C;
}

void AEwigfelsLightningBolt::SetStyle(const FLinearColor& InGlowColor, float InJaggedness, bool bInBranches)
{
	GlowColor = InGlowColor;
	bHasGlowColor = true;
	Jaggedness = FMath::Clamp(InJaggedness, 0.f, 2.f);
	bBranches = bInBranches;
	if (Jaggedness < 0.5f)
	{
		AfterglowColor = InGlowColor; // ruhiges Band: Nachbild in der eigenen Farbe statt bläulich
	}
}

void AEwigfelsLightningBolt::Setup(const FVector& Start, const FVector& End, const FLinearColor& Color, float Width, float Duration, int32 Regenerations)
{
	From = Start;
	To = End;
	BoltColor = Color;
	BaseWidth = FMath::Max(0.2f, Width);
	Life = FMath::Max(0.05f, Duration);
	AfterLife = FMath::FRandRange(0.15f, 0.3f);
	Regens = RegensLeft = FMath::Clamp(Regenerations, 0, 6);
	NextRegen = Life / (Regens + 1);
	Rng.Initialize(FMath::Rand());

	const FLinearColor Core = FLinearColor::LerpUsingHSV(Color, FLinearColor::White, 0.85f);
	const FLinearColor Mid = FLinearColor::LerpUsingHSV(Color, FLinearColor::White, 0.35f);
	StartLight->SetLightColor(Core);
	EndLight->SetLightColor(Color);
	StartLight->SetAttenuationRadius(380.f + 150.f * BaseWidth);
	EndLight->SetAttenuationRadius(600.f + 300.f * BaseWidth);
	StartLight->SetWorldLocation(From);
	EndLight->SetWorldLocation(To);

	Generate();
	// Feste Obergrenze an Teilen, damit ein Blitz nie hunderte Komponenten anlegt
	constexpr int32 MaxPieces = 72;
	int32 Needed = 0;
	for (const FBoltPath& P : Paths)
	{
		Needed += FMath::Max(0, P.Points.Num() - 1);
	}
	Pool.SetNum(FMath::Min(Needed + 8, MaxPieces));
	for (FBoltPiece& Piece : Pool)
	{
		UMaterialInstanceDynamic* M = nullptr;
		Piece.Outer = MakePiece(bHasGlowColor ? GlowColor : Color, 4.f, M);
		Piece.OuterMat = M;
		Piece.Mid = MakePiece(Mid, 16.f, M);
		Piece.MidMat = M;
		Piece.Core = MakePiece(Core, 70.f, M);
		Piece.CoreMat = M;
	}
	Place(1.f, false);
	SetLifeSpan(Life + AfterLife + 0.2f);
}

void AEwigfelsLightningBolt::Generate()
{
	Paths.Reset();
	const float Dist = FVector::Dist(From, To);
	FBoltPath& Main = Paths.AddDefaulted_GetRef();
	// 5 Ebenen = 32 Segmente; Versatz am Anfang ~12 % der Länge
	MidpointDisplace(Rng, From, To, 5, FMath::Min(Dist * 0.12f, 160.f) * Jaggedness, Main.Points);
	if (bBranches)
	{
		const TArray<FVector> MainPoints = Main.Points;
		GrowBranches(MainPoints, 1);
	}
}

void AEwigfelsLightningBolt::GrowBranches(const TArray<FVector>& Points, int32 Depth)
{
	if (Depth > 2 || Points.Num() < 6)
	{
		return;
	}
	const FVector End = Points.Last();
	const float Chance = Depth == 1 ? 0.22f : 0.12f;
	for (int32 i = 2; i + 3 < Points.Num(); ++i)
	{
		if (Rng.GetFraction() > Chance)
		{
			continue;
		}
		const FVector& P = Points[i];
		const FVector Seg = (Points[i + 1] - P).GetSafeNormal();
		FVector Side;
		FVector Up;
		Seg.FindBestAxisVectors(Side, Up);
		const float Spin = Rng.FRandRange(0.f, 2.f * PI);
		const FVector Perp = Side * FMath::Cos(Spin) + Up * FMath::Sin(Spin);
		const float Angle = FMath::DegreesToRadians(Rng.FRandRange(15.f, 45.f));
		const FVector Dir = (Seg * FMath::Cos(Angle) + Perp * FMath::Sin(Angle)).GetSafeNormal();
		const float Length = FVector::Dist(P, End) * Rng.FRandRange(0.3f, 0.6f) * (Depth == 1 ? 1.f : 0.6f);
		if (Length < 15.f)
		{
			continue;
		}
		FBoltPath& Branch = Paths.AddDefaulted_GetRef();
		MidpointDisplace(Rng, P, P + Dir * Length, 3, Length * 0.18f, Branch.Points);
		Branch.Width = FMath::Pow(0.5f, static_cast<float>(Depth));
		Branch.Alpha = FMath::Pow(0.62f, static_cast<float>(Depth));
		const TArray<FVector> BranchPoints = Branch.Points; // Kopie: Paths kann beim Wachsen umziehen
		GrowBranches(BranchPoints, Depth + 1);
	}
}

void AEwigfelsLightningBolt::Place(float Brightness, bool bAfterglow)
{
	int32 Index = 0;
	for (const FBoltPath& Path : Paths)
	{
		if (bAfterglow && Path.Width < 0.5f)
		{
			continue; // Nachbild nur vom Hauptstrahl und dicken Ästen
		}
		for (int32 i = 0; i + 1 < Path.Points.Num() && Index < Pool.Num(); ++i, ++Index)
		{
			FBoltPiece& Piece = Pool[Index];
			const FVector A = Path.Points[i];
			const FVector B = Path.Points[i + 1];
			const FVector Delta = B - A;
			const float Len = Delta.Size();
			const FRotator Rot = FRotationMatrix::MakeFromZ(Delta / FMath::Max(Len, 0.01f)).Rotator();
			const FVector MidPoint = (A + B) * 0.5f;
			const float W = BaseWidth * Path.Width;
			// BasicShapes-Zylinder: 100 cm hoch (Z), 100 cm Durchmesser; Saum/Glow/Kern wie in der Vorschau 12/5/1,8
			auto Put = [&](UStaticMeshComponent* Comp, UMaterialInstanceDynamic* Mat, float WidthCm, float Opacity)
			{
				const bool bShow = Len > 1.f && Opacity > 0.005f;
				Comp->SetVisibility(bShow);
				if (!bShow)
				{
					return;
				}
				Comp->SetWorldLocationAndRotation(MidPoint, Rot);
				Comp->SetWorldScale3D(FVector(WidthCm / 100.f, WidthCm / 100.f, (Len + WidthCm * 0.5f) / 100.f));
				if (Mat)
				{
					Mat->SetScalarParameterValue(TEXT("Opacity"), Opacity);
				}
			};
			if (bAfterglow)
			{
				if (Piece.OuterMat)
				{
					Piece.OuterMat->SetVectorParameterValue(TEXT("Color"), AfterglowColor);
				}
				Put(Piece.Outer, Piece.OuterMat, 9.f * W, 0.3f * Brightness);
				Put(Piece.Mid, Piece.MidMat, 0.f, 0.f);
				Put(Piece.Core, Piece.CoreMat, 0.f, 0.f);
				continue;
			}
			Put(Piece.Outer, Piece.OuterMat, 26.f * W, 0.15f * Path.Alpha * Brightness);
			Put(Piece.Mid, Piece.MidMat, 9.f * W, 0.4f * Path.Alpha * Brightness);
			Put(Piece.Core, Piece.CoreMat, FMath::Max(1.2f, 3.2f * W), FMath::Min(1.f, 0.95f * Path.Alpha * Brightness + 0.05f));
		}
	}
	for (; Index < Pool.Num(); ++Index)
	{
		Pool[Index].Outer->SetVisibility(false);
		Pool[Index].Mid->SetVisibility(false);
		Pool[Index].Core->SetVisibility(false);
	}
}

void AEwigfelsLightningBolt::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Age += DeltaSeconds;
	if (Age < Life)
	{
		if (RegensLeft > 0 && Age >= NextRegen)
		{
			// gleiches Start/Ende, neuer Pfad
			Generate();
			--RegensLeft;
			NextRegen += Life / (Regens + 1);
		}
		// Helligkeit springt unregelmäßig zwischen 60 und 100 %, letztes Viertel blendet aus
		if (Rng.GetFraction() < 0.35f)
		{
			Bright = Rng.FRandRange(0.6f, 1.f);
		}
		const float Tail = Age > Life * 0.75f ? 1.f - (Age - Life * 0.75f) / (Life * 0.25f) : 1.f;
		Place(Bright * Tail, false);
		StartLight->SetIntensity((4000.f + 3000.f * BaseWidth) * Bright * Tail);
		EndLight->SetIntensity((9000.f + 7000.f * BaseWidth) * Bright * Tail);
		return;
	}
	const float T = (Age - Life) / AfterLife;
	if (T >= 1.f)
	{
		Destroy();
		return;
	}
	// Nachbild: schwach, bläulich, blendet aus
	Place(1.f - T, true);
	StartLight->SetIntensity(0.f);
	EndLight->SetIntensity(1500.f * (1.f - T));
}

// ---------------------------------------------------------------------------

AEwigfelsSparkBurst::AEwigfelsSparkBurst()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;
	SetCanBeDamaged(false);
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetMobility(EComponentMobility::Movable);
	RootComponent = Root;
}

void AEwigfelsSparkBurst::Setup(const FVector& Location, const FVector& Normal, const FLinearColor& Color, int32 Count, float Speed)
{
	FloorZ = Location.Z - 200.f;
	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(EwigfelsSparks), false);
	if (GetWorld() && GetWorld()->LineTraceSingleByChannel(Hit, Location + FVector(0.f, 0.f, 20.f), Location - FVector(0.f, 0.f, 600.f), ECC_Visibility, Params))
	{
		FloorZ = Hit.ImpactPoint.Z + 1.f;
	}
	const FLinearColor Hot = FLinearColor::LerpUsingHSV(Color, FLinearColor::White, 0.6f);
	const FVector Bias = Normal.IsNearlyZero() ? FVector::UpVector : Normal.GetSafeNormal();
	Count = FMath::Clamp(Count, 1, 60);
	Sparks.SetNum(Count);
	for (FSpark& S : Sparks)
	{
		S.Pos = Location;
		const FVector Dir = (FMath::VRand() + Bias * 0.6f + FVector(0.f, 0.f, 0.4f)).GetSafeNormal();
		S.Vel = Dir * Speed * FMath::FRandRange(0.35f, 1.f);
		S.Life = FMath::FRandRange(0.28f, 0.65f);
		S.Comp = NewObject<UStaticMeshComponent>(this);
		S.Comp->SetStaticMesh(EwigfelsSpellVisuals::GetShape(TEXT("cylinder")));
		S.Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		S.Comp->SetCastShadow(false);
		S.Comp->SetMobility(EComponentMobility::Movable);
		S.Comp->SetupAttachment(Root);
		S.Comp->RegisterComponent();
		S.Mat = EwigfelsSpellVisuals::ApplyBeam(S.Comp, Hot, 40.f);
		Keep.Add(S.Comp);
		if (S.Mat)
		{
			Keep.Add(S.Mat);
		}
	}
	SetLifeSpan(0.9f);
}

void AEwigfelsSparkBurst::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Age += DeltaSeconds;
	bool bAny = false;
	for (FSpark& S : Sparks)
	{
		const float K = 1.f - Age / S.Life;
		if (K <= 0.f)
		{
			S.Comp->SetVisibility(false);
			continue;
		}
		bAny = true;
		S.Vel.Z -= 1400.f * DeltaSeconds; // Schwerkraft
		S.Vel *= FMath::Pow(0.4f, DeltaSeconds);
		S.Pos += S.Vel * DeltaSeconds;
		if (S.Pos.Z < FloorZ && S.Vel.Z < 0.f)
		{
			S.Pos.Z = FloorZ;
			S.Vel.Z *= -0.35f; // abprallen
			S.Vel.X *= 0.7f;
			S.Vel.Y *= 0.7f;
		}
		// Leuchtspur: Streifen entlang der Flugrichtung, Länge = zurückgelegter Weg der letzten ~40 ms
		const float Speed = S.Vel.Size();
		const float TrailLen = FMath::Clamp(Speed * 0.04f, 3.f, 60.f);
		const FVector Dir = S.Vel / FMath::Max(Speed, 1.f);
		const float Width = 1.6f * K + 0.4f;
		S.Comp->SetWorldLocationAndRotation(S.Pos - Dir * TrailLen * 0.5f, FRotationMatrix::MakeFromZ(Dir).Rotator());
		S.Comp->SetWorldScale3D(FVector(Width / 100.f, Width / 100.f, TrailLen / 100.f));
		if (S.Mat)
		{
			S.Mat->SetScalarParameterValue(TEXT("Opacity"), K);
		}
	}
	if (!bAny)
	{
		Destroy();
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
	Light->SetMobility(EComponentMobility::Movable);
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
	// Halo: Endform x 1,6 (BasicShapes = 100 cm), nur bei kugeligen Blitzen, flache Ringe bleiben ohne.
	// Vorher x 2,4 mit Glühen 8: überstrahlte die ganze Figur beim Mündungsblitz.
	if (EndScale.Z > 0.1f)
	{
		HaloMaterial = EwigfelsSpellVisuals::AddHalo(Mesh, Color, EndScale.GetMax() * 160.f, 3.f);
	}
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
	if (HaloMaterial)
	{
		HaloMaterial->SetScalarParameterValue(TEXT("Opacity"), Fade * Fade);
	}
	if (T >= 1.f)
	{
		Destroy();
	}
}
