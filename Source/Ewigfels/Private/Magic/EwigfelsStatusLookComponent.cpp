// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsStatusLookComponent.h"
#include "Magic/EwigfelsSpellSubsystem.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"

UEwigfelsStatusLookComponent::UEwigfelsStatusLookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(false);
}

void UEwigfelsStatusLookComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(GetOwner()))
	{
		Combat->OnStatusesChanged.AddDynamic(this, &UEwigfelsStatusLookComponent::Refresh);
	}
	Refresh();
}

void UEwigfelsStatusLookComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (FLook& L : Looks)
	{
		DestroyLook(L);
	}
	Looks.Reset();
	Super::EndPlay(EndPlayReason);
}

void UEwigfelsStatusLookComponent::GetBodySize(float& OutRadius, float& OutHalfHeight) const
{
	OutRadius = 40.f;
	OutHalfHeight = 90.f;
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (const UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			OutRadius = Capsule->GetScaledCapsuleRadius();
			OutHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
		}
	}
}

void UEwigfelsStatusLookComponent::Refresh()
{
	const UEwigfelsCombatComponent* Combat = UEwigfelsCombatComponent::Find(GetOwner());
	const UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	if (!Combat || !Spells)
	{
		return;
	}

	// Gewünschte Looks aus den aktiven Status (ein Look pro Art, Farbe vom ersten Status)
	TMap<FName, FLinearColor> Wanted;
	if (!Combat->IsDead())
	{
		for (const FEwigfelsActiveStatus& S : Combat->GetActiveStatuses())
		{
			const FEwigfelsStatusDef* Def = Spells->FindStatus(S.Id);
			if (Def && !Def->Look.IsNone() && !Wanted.Contains(Def->Look))
			{
				Wanted.Add(Def->Look, Def->Color);
			}
		}
	}
	for (int32 i = Looks.Num() - 1; i >= 0; --i)
	{
		if (!Wanted.Contains(Looks[i].Look))
		{
			DestroyLook(Looks[i]);
			Looks.RemoveAt(i);
		}
	}
	for (const TPair<FName, FLinearColor>& W : Wanted)
	{
		if (!Looks.ContainsByPredicate([&W](const FLook& L) { return L.Look == W.Key; }))
		{
			FLook& L = Looks.AddDefaulted_GetRef();
			L.Look = W.Key;
			BuildLook(L, W.Value);
		}
	}
	SetAnimsPaused(Wanted.Contains(TEXT("ice")));
	SetComponentTickEnabled(Looks.Num() > 0);
}

void UEwigfelsStatusLookComponent::BuildLook(FLook& Look, const FLinearColor& Color)
{
	AActor* Owner = GetOwner();
	USceneComponent* Root = Owner ? Owner->GetRootComponent() : nullptr;
	if (!Root)
	{
		return;
	}
	float Radius = 40.f;
	float Half = 90.f;
	GetBodySize(Radius, Half);

	auto Add = [&](FName Shape, const FVector& Offset, const FVector& Scale, const FRotator& Rot, float Glow, float Opacity, float Phase,
		int32 Kind = 0) // 0 = Leuchten, 1 = Eis (Lichtbrechung), 2 = Hitzeflimmern
	{
		UStaticMeshComponent* C = NewObject<UStaticMeshComponent>(Owner);
		C->SetStaticMesh(EwigfelsSpellVisuals::GetShape(Shape));
		C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		C->SetCastShadow(false);
		C->SetupAttachment(Root);
		C->RegisterComponent();
		C->SetRelativeLocationAndRotation(Offset, Rot);
		C->SetRelativeScale3D(Scale);
		UMaterialInstanceDynamic* M = Kind == 1 ? EwigfelsSpellVisuals::ApplyIce(C, Color)
			: Kind == 2 ? EwigfelsSpellVisuals::ApplyHeat(C, 0.08f)
			: EwigfelsSpellVisuals::ApplyGlow(C, Color, Glow);
		if (Kind == 2 && !M)
		{
			C->DestroyComponent(); // kein Hitze-Material da: lieber gar nichts als ein grauer Kegel
			return;
		}
		if (M && Kind == 0)
		{
			M->SetScalarParameterValue(TEXT("Opacity"), Opacity);
		}
		Keep.Add(C);
		if (M)
		{
			Keep.Add(M);
		}
		FLookPiece& P = Look.Pieces.AddDefaulted_GetRef();
		P.Comp = C;
		P.Mat = M;
		P.Offset = Offset;
		P.BaseScale = Scale;
		P.Phase = Phase;
	};

	const float D = Radius * 2.6f / 100.f; // BasicShapes = 100 cm
	if (Look.Look == TEXT("ice"))
	{
		// Eisblock um den Körper + zwei schräge Zacken
		Add(TEXT("cube"), FVector(0.f, 0.f, 0.f), FVector(D, D, Half * 2.2f / 100.f), FRotator(0.f, 20.f, 0.f), 1.6f, 0.38f, 0.f, 1);
		Add(TEXT("cone"), FVector(Radius * 0.9f, 0.f, Half * 0.7f), FVector(0.25f, 0.25f, 0.7f), FRotator(-35.f, 0.f, 0.f), 3.f, 0.5f, 0.f, 1);
		Add(TEXT("cone"), FVector(-Radius * 0.7f, Radius * 0.6f, Half * 0.4f), FVector(0.2f, 0.2f, 0.55f), FRotator(30.f, 60.f, 0.f), 3.f, 0.5f, 0.f, 1);
	}
	else if (Look.Look == TEXT("notes"))
	{
		// drei leuchtende Noten kreisen über dem Kopf
		for (int32 i = 0; i < 3; ++i)
		{
			Add(TEXT("sphere"), FVector(0.f, 0.f, Half + 30.f), FVector(0.14f), FRotator::ZeroRotator, 12.f, 1.f, i * 2.f * PI / 3.f);
		}
	}
	else if (Look.Look == TEXT("flames"))
	{
		for (int32 i = 0; i < 5; ++i)
		{
			const float A = i * 2.f * PI / 5.f;
			Add(TEXT("cone"), FVector(FMath::Cos(A) * Radius * 0.6f, FMath::Sin(A) * Radius * 0.6f, -Half * 0.3f + (i % 3) * Half * 0.35f),
				FVector(0.22f, 0.22f, 0.5f), FRotator::ZeroRotator, 14.f, 0.8f, i * 1.3f);
		}
		// Hitzeflimmern um den ganzen Körper
		Add(TEXT("cylinder"), FVector(0.f, 0.f, Half * 0.2f), FVector(D * 1.1f, D * 1.1f, Half * 2.6f / 100.f), FRotator::ZeroRotator, 0.f, 0.f, 0.f, 2);
	}
	else if (Look.Look == TEXT("rings"))
	{
		for (int32 i = 0; i < 2; ++i)
		{
			Add(TEXT("cylinder"), FVector(0.f, 0.f, -Half - 10.f - i * 25.f), FVector(D * (1.1f + i * 0.35f), D * (1.1f + i * 0.35f), 0.012f),
				FRotator::ZeroRotator, 10.f, 0.6f, i * PI);
		}
	}
}

void UEwigfelsStatusLookComponent::DestroyLook(FLook& Look)
{
	for (FLookPiece& P : Look.Pieces)
	{
		if (P.Comp)
		{
			Keep.Remove(P.Comp);
			Keep.Remove(P.Mat);
			P.Comp->DestroyComponent();
		}
	}
	Look.Pieces.Reset();
}

void UEwigfelsStatusLookComponent::SetAnimsPaused(bool bPaused)
{
	if (bPaused == bPausedAnims)
	{
		return;
	}
	bPausedAnims = bPaused;
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			Mesh->bPauseAnims = bPaused; // gefroren = Pose bleibt stehen
		}
	}
}

void UEwigfelsStatusLookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Age += DeltaTime;
	for (FLook& L : Looks)
	{
		for (FLookPiece& P : L.Pieces)
		{
			if (!P.Comp)
			{
				continue;
			}
			if (L.Look == TEXT("notes"))
			{
				const float A = Age * 3.4f + P.Phase;
				P.Comp->SetRelativeLocation(P.Offset + FVector(FMath::Cos(A) * 38.f, FMath::Sin(A) * 38.f, FMath::Sin(A * 2.f) * 8.f));
			}
			else if (L.Look == TEXT("flames"))
			{
				const float F = 0.75f + 0.35f * FMath::Abs(FMath::Sin(Age * 11.f + P.Phase * 3.f));
				P.Comp->SetRelativeScale3D(FVector(P.BaseScale.X, P.BaseScale.Y, P.BaseScale.Z * F));
				P.Comp->SetRelativeLocation(P.Offset + FVector(0.f, 0.f, FMath::Fmod(Age * 60.f + P.Phase * 20.f, 30.f)));
			}
			else if (L.Look == TEXT("rings"))
			{
				const float Pulse = 1.f + 0.08f * FMath::Sin(Age * 4.f + P.Phase);
				P.Comp->SetRelativeScale3D(FVector(P.BaseScale.X * Pulse, P.BaseScale.Y * Pulse, P.BaseScale.Z));
				P.Comp->SetRelativeRotation(FRotator(0.f, Age * 90.f * (P.Phase > 0.f ? -1.f : 1.f), 0.f));
			}
			else if (L.Look == TEXT("ice") && P.Mat)
			{
				P.Mat->SetScalarParameterValue(TEXT("Glow"), 1.6f + 0.4f * FMath::Sin(Age * 2.f));
			}
		}
	}
}
