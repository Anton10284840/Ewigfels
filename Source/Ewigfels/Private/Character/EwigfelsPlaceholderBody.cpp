// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Character/EwigfelsPlaceholderBody.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Components/PointLightComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	// Grundformen sind 100 cm groß (Kugel Ø 100, Zylinder Ø 100 × 100 hoch, Kegel Ø 100 × 100). Ursprung = Mitte.
	constexpr float CapsuleHalf = 90.f; // Figur steht mit dem Fuß bei -90 (Kapselboden)
}

UEwigfelsPlaceholderBody::UEwigfelsPlaceholderBody()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(false);
}

UStaticMeshComponent* UEwigfelsPlaceholderBody::MakePart(FName Name, FName Shape, const FVector& Location, const FVector& Scale, USceneComponent* Parent)
{
	UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(GetOwner(), Name);
	Part->SetStaticMesh(EwigfelsSpellVisuals::GetShape(Shape));
	Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Part->SetGenerateOverlapEvents(false);
	Part->SetCanEverAffectNavigation(false);
	Part->SetMobility(EComponentMobility::Movable);
	Part->SetupAttachment(Parent);
	Part->SetRelativeLocation(Location);
	Part->SetRelativeScale3D(Scale);
	Part->RegisterComponent();
	return Part;
}

void UEwigfelsPlaceholderBody::BeginPlay()
{
	Super::BeginPlay();

	// Hat die Figur ein echtes Mesh, wird der Platzhalter nicht gebraucht
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (Character->GetMesh() && Character->GetMesh()->GetSkeletalMeshAsset())
		{
			SetComponentTickEnabled(false);
			return;
		}
	}
	bActive = true;
	bBuilt = true;

	const float H = Height;
	Pivot = NewObject<USceneComponent>(GetOwner(), TEXT("PlaceholderPivot"));
	Pivot->SetupAttachment(this);
	Pivot->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleHalf));
	Pivot->RegisterComponent();

	// Robe: Kegel (unten breit), Torso: Zylinder, Kopf: Kugel, Kapuze: Kegel über dem Kopf
	Robe  = MakePart(TEXT("PH_Robe"),  TEXT("cone"),     FVector(0.f, 0.f, 55.f * H),  FVector(0.62f, 0.62f, 1.1f * H), Pivot);
	Torso = MakePart(TEXT("PH_Torso"), TEXT("cylinder"), FVector(0.f, 0.f, 115.f * H), FVector(0.38f, 0.32f, 0.5f * H), Pivot);
	Head  = MakePart(TEXT("PH_Head"),  TEXT("sphere"),   FVector(0.f, 0.f, 160.f * H), FVector(0.3f, 0.28f, 0.33f), Pivot);
	Hood  = MakePart(TEXT("PH_Hood"),  TEXT("cone"),     FVector(-3.f, 0.f, 172.f * H), FVector(0.42f, 0.4f, 0.48f), Pivot);
	Hood->SetVisibility(bHood);
	EyeL  = MakePart(TEXT("PH_EyeL"),  TEXT("sphere"),   FVector(13.f, -6.f, 162.f * H), FVector(0.045f), Pivot);
	EyeR  = MakePart(TEXT("PH_EyeR"),  TEXT("sphere"),   FVector(13.f, 6.f, 162.f * H),  FVector(0.045f), Pivot);
	HandL = MakePart(TEXT("PH_HandL"), TEXT("sphere"),   FVector(8.f, -28.f, 95.f * H),  FVector(0.11f), Pivot);
	HandR = MakePart(TEXT("PH_HandR"), TEXT("sphere"),   FVector(8.f, 28.f, 95.f * H),   FVector(0.11f), Pivot);
	HandR->ComponentTags.Add(TEXT("CastOriginR"));
	HandL->ComponentTags.Add(TEXT("CastOriginL"));
	Shield = MakePart(TEXT("PH_Shield"), TEXT("sphere"), FVector(55.f, 0.f, 110.f * H), FVector(0.12f, 1.25f, 1.6f), Pivot);
	Shield->SetVisibility(false);

	HandLight = NewObject<UPointLightComponent>(GetOwner(), TEXT("PH_HandLight"));
	HandLight->SetupAttachment(HandR);
	HandLight->SetMobility(EComponentMobility::Movable);
	HandLight->SetIntensityUnits(ELightUnits::Lumens);
	HandLight->SetIntensity(0.f);
	HandLight->SetAttenuationRadius(300.f);
	HandLight->SetCastShadows(false);
	HandLight->RegisterComponent();

	RefreshMaterials();
}

void UEwigfelsPlaceholderBody::RefreshMaterials()
{
	if (!bBuilt)
	{
		return;
	}
	BodyMat = EwigfelsSpellVisuals::ApplySolid(Robe, BodyColor, 0.8f);
	Torso->SetMaterial(0, BodyMat);
	Hood->SetMaterial(0, BodyMat);
	const FLinearColor Skin(0.55f, 0.42f, 0.36f);
	EwigfelsSpellVisuals::ApplySolid(Head, Skin, 0.6f);
	EyeMat = EwigfelsSpellVisuals::ApplyGlow(EyeL, EyeColor, 4.f);
	EyeR->SetMaterial(0, EyeMat);
	HandMat = EwigfelsSpellVisuals::ApplyGlow(HandR, AccentColor, 2.f);
	HandL->SetMaterial(0, HandMat);
	ShieldMat = EwigfelsSpellVisuals::ApplyGlow(Shield, AccentColor, 6.f);
}

void UEwigfelsPlaceholderBody::SetLook(const FLinearColor& InBody, const FLinearColor& InAccent, const FLinearColor& InEyes, float InHeight, bool bInHood)
{
	BodyColor = InBody;
	AccentColor = InAccent;
	EyeColor = InEyes;
	CastColor = InAccent;
	bHood = bInHood;
	if (!bBuilt)
	{
		Height = InHeight; // Höhe wirkt nur vor dem Aufbau (BeginPlay)
		return;
	}
	Hood->SetVisibility(bHood);
	RefreshMaterials();
}

void UEwigfelsPlaceholderBody::SetCastPose(float Alpha, const FLinearColor& Color, bool bTwoHanded)
{
	CastTarget = FMath::Clamp(Alpha, 0.f, 1.f);
	CastColor = Color;
	bCastTwoHanded = bTwoHanded;
}

void UEwigfelsPlaceholderBody::SetTelegraph(float Alpha)
{
	Telegraph = FMath::Clamp(Alpha, 0.f, 1.f);
}

void UEwigfelsPlaceholderBody::PlayHit()
{
	HitKick = 1.f;
}

void UEwigfelsPlaceholderBody::PlayDeath()
{
	if (DeathTime < 0.f)
	{
		DeathTime = 0.f;
	}
}

void UEwigfelsPlaceholderBody::Revive()
{
	DeathTime = -1.f;
	if (bBuilt)
	{
		Pivot->SetRelativeRotation(FRotator::ZeroRotator);
		Pivot->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleHalf));
	}
}

void UEwigfelsPlaceholderBody::SetShieldVisible(bool bVisible, const FLinearColor& Color)
{
	if (!bBuilt)
	{
		return;
	}
	Shield->SetVisibility(bVisible);
	if (ShieldMat)
	{
		ShieldMat->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void UEwigfelsPlaceholderBody::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bBuilt)
	{
		return;
	}
	Clock += DeltaTime;
	const AActor* Owner = GetOwner();
	const float Speed = Owner ? Owner->GetVelocity().Size2D() : 0.f;
	const float H = Height;

	// Tod: nach hinten kippen, einsinken
	if (DeathTime >= 0.f)
	{
		DeathTime += DeltaTime;
		const float T = FMath::Clamp(DeathTime / 0.9f, 0.f, 1.f);
		Pivot->SetRelativeRotation(FRotator(-85.f * FMath::Sin(T * HALF_PI), 0.f, 0.f));
		Pivot->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleHalf - 40.f * FMath::Max(0.f, DeathTime - 1.f)));
		if (EyeMat)
		{
			EyeMat->SetScalarParameterValue(TEXT("Glow"), 4.f * (1.f - T));
		}
		HandLight->SetIntensity(0.f);
		return;
	}

	// Atmen + Schritt-Wippen
	const float Breath = FMath::Sin(Clock * 2.2f) * 1.2f;
	const float StepFreq = 2.f + Speed / 140.f;
	const float Bob = Speed > 20.f ? FMath::Abs(FMath::Sin(Clock * StepFreq * PI)) * FMath::Min(Speed / 120.f, 4.f) : 0.f;
	HitKick = FMath::FInterpTo(HitKick, 0.f, DeltaTime, 9.f);
	const float Lean = FMath::Min(Speed / 600.f, 1.f) * 8.f - HitKick * 14.f;
	Pivot->SetRelativeLocation(FVector(-HitKick * 12.f, 0.f, -CapsuleHalf + Bob + Breath * 0.3f));
	Pivot->SetRelativeRotation(FRotator(-Lean, 0.f, 0.f));
	Torso->SetRelativeLocation(FVector(0.f, 0.f, 115.f * H + Breath));

	// Hände: Ruhe seitlich, beim Zaubern nach vorne (eine oder beide), Gegner-Vorwarnung hebt beide
	CastAlpha = FMath::FInterpTo(CastAlpha, FMath::Max(CastTarget, Telegraph), DeltaTime, 16.f);
	const float SwingR = Speed > 20.f ? FMath::Sin(Clock * StepFreq * PI) * 10.f : 0.f;
	const FVector RestR(8.f + SwingR, 28.f, 95.f * H);
	const FVector RestL(8.f - SwingR, -28.f, 95.f * H);
	const FVector CastR(48.f, 12.f, 128.f * H);
	const FVector CastL = bCastTwoHanded || Telegraph > 0.f ? FVector(48.f, -12.f, 128.f * H) : RestL;
	HandR->SetRelativeLocation(FMath::Lerp(RestR, CastR, CastAlpha));
	HandL->SetRelativeLocation(FMath::Lerp(RestL, CastL, CastAlpha));

	const FLinearColor HandColor = FMath::Lerp(AccentColor, CastColor, CastAlpha);
	const float Glow = 2.f + CastAlpha * 30.f + Telegraph * 20.f;
	if (HandMat)
	{
		HandMat->SetVectorParameterValue(TEXT("Color"), HandColor);
		HandMat->SetScalarParameterValue(TEXT("Glow"), Glow);
	}
	if (EyeMat)
	{
		EyeMat->SetScalarParameterValue(TEXT("Glow"), 4.f + Telegraph * 40.f + HitKick * 20.f);
	}
	HandLight->SetLightColor(HandColor);
	HandLight->SetIntensity(CastAlpha * 2500.f + Telegraph * 1500.f + 150.f);
}
