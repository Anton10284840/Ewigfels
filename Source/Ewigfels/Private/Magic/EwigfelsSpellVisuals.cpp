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

	if (UNiagaraComponent* Beam = SpawnNiagaraAt(World, Spell.Visual.Niagara, Start, Dir.Rotation(), Spell.Visual.Color, Scale))
	{
		Beam->SetVariableVec3(TEXT("User.BeamEnd"), End);
	}
	else
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (AEwigfelsLightningBolt* Bolt = World->SpawnActor<AEwigfelsLightningBolt>(AEwigfelsLightningBolt::StaticClass(), FTransform::Identity, Params))
		{
			Bolt->Setup(Start, End, Spell.Visual.Color, 7.f * Scale, bStrong ? 0.2f : 0.13f, bStrong);
		}
	}

	// Mündungsblitz: kurzer, greller Funke an den Fingern
	SpawnFlash(World, TEXT("sphere"), Start, FRotator::ZeroRotator, FLinearColor::LerpUsingHSV(Spell.Visual.Color, FLinearColor::White, 0.6f),
		FVector(0.08f), FVector(0.35f * Scale), 0.09f, 6000.f, 380.f);

	// Einschlag: Niagara oder Lichtkugel; bei Treffer stärker, beim Kettenende mit Druckring
	const FRotator Rot = Normal.IsNearlyZero() ? FRotator::ZeroRotator : Normal.Rotation();
	if (!SpawnNiagaraAt(World, Spell.Visual.ImpactNiagara, End, Rot, Spell.Visual.Color, Scale))
	{
		SpawnFlash(World, TEXT("sphere"), End, Rot, Spell.Visual.Color, FVector(0.15f), FVector((bHitTarget ? 0.9f : 0.55f) * Scale),
			bStrong ? 0.3f : 0.2f, bHitTarget ? 14000.f : 6000.f, bStrong ? 900.f : 550.f);
		if (bStrong)
		{
			// flacher Ring senkrecht zur Schlagrichtung
			const FRotator RingRot = FRotationMatrix::MakeFromZ(Dir).Rotator();
			SpawnFlash(World, TEXT("cylinder"), End, RingRot, Spell.Visual.Color, FVector(0.2f, 0.2f, 0.01f), FVector(2.4f, 2.4f, 0.015f),
				0.28f, 0.f, 0.f);
		}
	}
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
		// Ausschlag in der Mitte am größten, an den Enden null (Blitz sitzt sauber an Hand und Ziel)
		const float Envelope = FMath::Sin(T * PI);
		const FVector Offset = (Side * InRng.FRandRange(-1.f, 1.f) + Up * InRng.FRandRange(-1.f, 1.f)) * Amplitude * Envelope;
		OutPoints.Add(FMath::Lerp(A, B, T) + Offset);
	}
	OutPoints.Add(B);
}

UStaticMeshComponent* AEwigfelsLightningBolt::MakePiece(const FLinearColor& Color, float Glow)
{
	UStaticMeshComponent* C = NewObject<UStaticMeshComponent>(this);
	C->SetStaticMesh(EwigfelsSpellVisuals::GetShape(TEXT("cylinder")));
	C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	C->SetCastShadow(false);
	C->SetMobility(EComponentMobility::Movable);
	C->SetupAttachment(Root);
	C->RegisterComponent();
	if (UMaterialInstanceDynamic* MID = EwigfelsSpellVisuals::ApplyGlow(C, Color, Glow))
	{
		Materials.Add(MID);
	}
	Pieces.Add(C);
	return C;
}

void AEwigfelsLightningBolt::Setup(const FVector& Start, const FVector& End, const FLinearColor& Color, float Thickness, float Duration, bool bStrong)
{
	From = Start;
	To = End;
	BoltColor = Color;
	BaseWidth = FMath::Max(1.f, Thickness);
	Length = FMath::Max(0.05f, Duration);
	Forks = bStrong ? 4 : 2;
	Rng.Initialize(FMath::Rand());

	const float Dist = FVector::Dist(From, To);
	const int32 MainSegments = FMath::Clamp(FMath::RoundToInt(Dist / 70.f), 6, 26);
	const int32 ForkSegments = 4;
	const int32 Total = MainSegments + Forks * ForkSegments;
	const FLinearColor CoreColor = FLinearColor::LerpUsingHSV(Color, FLinearColor::White, 0.8f);
	Segments.SetNum(Total);
	for (FBoltSegment& S : Segments)
	{
		S.Core = MakePiece(CoreColor, 60.f);
		S.Halo = MakePiece(Color, 18.f);
	}

	PeakStartLight = bStrong ? 9000.f : 5000.f;
	PeakEndLight = bStrong ? 22000.f : 12000.f;
	StartLight->SetLightColor(CoreColor);
	EndLight->SetLightColor(Color);
	StartLight->SetAttenuationRadius(450.f);
	EndLight->SetAttenuationRadius(bStrong ? 1100.f : 750.f);
	StartLight->SetWorldLocation(From);
	EndLight->SetWorldLocation(To);

	Rebuild();
	PlaceSegments(1.f);
	SetLifeSpan(Length + 0.15f);
}

void AEwigfelsLightningBolt::Rebuild()
{
	const float Dist = FVector::Dist(From, To);
	const int32 ForkSegments = 4;
	const int32 MainSegments = Segments.Num() - Forks * ForkSegments;
	TArray<FVector> Main;
	BuildJaggedPath(Rng, From, To, MainSegments, FMath::Min(45.f, Dist * 0.05f), Main);
	int32 Index = 0;
	for (int32 i = 0; i + 1 < Main.Num() && Index < Segments.Num(); ++i, ++Index)
	{
		Segments[Index].A = Main[i];
		Segments[Index].B = Main[i + 1];
		Segments[Index].Width = 1.f;
	}
	// Äste: zweigen irgendwo im mittleren Teil ab, schräg nach vorn, dünner
	const FVector Dir = (To - From).GetSafeNormal();
	TArray<FVector> Fork;
	for (int32 f = 0; f < Forks; ++f)
	{
		const FVector Base = Main[Rng.RandRange(1, FMath::Max(1, Main.Num() - 2))];
		const FVector Bend = FMath::VRandCone(Dir, FMath::DegreesToRadians(55.f));
		const FVector Tip = Base + Bend * Dist * Rng.FRandRange(0.08f, 0.22f);
		BuildJaggedPath(Rng, Base, Tip, ForkSegments, FMath::Min(18.f, Dist * 0.03f), Fork);
		for (int32 i = 0; i + 1 < Fork.Num() && Index < Segments.Num(); ++i, ++Index)
		{
			Segments[Index].A = Fork[i];
			Segments[Index].B = Fork[i + 1];
			Segments[Index].Width = 0.45f * (1.f - static_cast<float>(i) / ForkSegments);
		}
	}
	for (; Index < Segments.Num(); ++Index)
	{
		Segments[Index].A = Segments[Index].B = From; // unbenutzt
		Segments[Index].Width = 0.f;
	}
}

void AEwigfelsLightningBolt::PlaceSegments(float WidthScale)
{
	for (FBoltSegment& S : Segments)
	{
		const FVector Delta = S.B - S.A;
		const float Len = Delta.Size();
		const bool bVisible = Len > 1.f && S.Width * WidthScale > 0.01f;
		S.Core->SetVisibility(bVisible);
		S.Halo->SetVisibility(bVisible);
		if (!bVisible)
		{
			continue;
		}
		// BasicShapes-Zylinder: 100 cm hoch (Z), 100 cm Durchmesser
		const FRotator Rot = FRotationMatrix::MakeFromZ(Delta / Len).Rotator();
		const FVector Mid = (S.A + S.B) * 0.5f;
		const float W = BaseWidth * S.Width * WidthScale;
		S.Core->SetWorldLocationAndRotation(Mid, Rot);
		S.Core->SetWorldScale3D(FVector(W * 0.35f / 100.f, W * 0.35f / 100.f, Len / 100.f));
		S.Halo->SetWorldLocationAndRotation(Mid, Rot);
		S.Halo->SetWorldScale3D(FVector(W / 100.f, W / 100.f, Len / 100.f));
	}
}

void AEwigfelsLightningBolt::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	Age += DeltaSeconds;
	const float T = FMath::Clamp(Age / Length, 0.f, 1.f);
	const float Fade = 1.f - T;
	if (Age >= NextJitter && T < 0.8f)
	{
		NextJitter = Age + 0.035f;
		Rebuild(); // Flackern: neue Zacken
	}
	// Flackern der Helligkeit zusätzlich zum Ausblenden
	const float Flicker = FMath::Lerp(0.65f, 1.f, Rng.GetFraction());
	PlaceSegments(FMath::Sqrt(Fade) * Flicker);
	for (UMaterialInstanceDynamic* MID : Materials)
	{
		MID->SetScalarParameterValue(TEXT("Opacity"), Fade);
	}
	StartLight->SetIntensity(PeakStartLight * Fade * Fade * Flicker);
	EndLight->SetIntensity(PeakEndLight * Fade * Flicker);
	if (T >= 1.f)
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
