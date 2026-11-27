// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Camera/EwigfelsDialogueCameraComponent.h"
#include "Character/EwigfelsCharacter.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

UEwigfelsDialogueCameraComponent::UEwigfelsDialogueCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

APlayerController* UEwigfelsDialogueCameraComponent::GetPC() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	APlayerController* PC = Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
	return PC && PC->IsLocalController() ? PC : nullptr;
}

FVector UEwigfelsDialogueCameraComponent::GetEyeLocation(const AActor* Actor)
{
	if (!Actor)
	{
		return FVector::ZeroVector;
	}
	if (const ACharacter* Character = Cast<ACharacter>(Actor))
	{
		const USkeletalMeshComponent* Mesh = Character->GetMesh();
		if (Mesh && Mesh->GetSkeletalMeshAsset())
		{
			for (const TCHAR* Socket : { TEXT("FACIAL_L_Eye"), TEXT("head") })
			{
				if (Mesh->DoesSocketExist(Socket))
				{
					return Mesh->GetSocketLocation(Socket);
				}
			}
		}
	}
	// Platzhalter-Kopf sitzt ca. 72 cm über der Kapselmitte
	return Actor->GetActorLocation() + FVector(0.f, 0.f, 72.f);
}

EEwigfelsDialogueShot UEwigfelsDialogueCameraComponent::ChooseShot(const FEwigfelsDialogueLine& Line, bool bFirstLine)
{
	if (Line.Camera == TEXT("wide"))    { return EEwigfelsDialogueShot::Wide; }
	if (Line.Camera == TEXT("closeup")) { return EEwigfelsDialogueShot::CloseUp; }
	if (Line.Camera == TEXT("ots"))     { return EEwigfelsDialogueShot::OverShoulder; }
	if (bFirstLine)
	{
		return EEwigfelsDialogueShot::Wide;
	}
	return Line.Intensity >= 0.7f ? EEwigfelsDialogueShot::CloseUp : EEwigfelsDialogueShot::OverShoulder;
}

FTransform UEwigfelsDialogueCameraComponent::ComputeShot(EEwigfelsDialogueShot Shot, const FVector& ListenerEye, const FVector& SpeakerEye,
	float SideSign, float& OutFocal)
{
	FVector Axis = SpeakerEye - ListenerEye;
	const float Dist = FMath::Max(Axis.Size2D(), 60.f);
	Axis.Z = 0.f;
	Axis = Axis.GetSafeNormal();
	if (Axis.IsNearlyZero())
	{
		Axis = FVector::ForwardVector;
	}
	// Seiten-Normale: alle Kameras auf derselben Seite der Achse (180°-Regel)
	const FVector SideDir = FVector::CrossProduct(FVector::UpVector, Axis) * (SideSign >= 0.f ? 1.f : -1.f);

	FVector CamLoc;
	FVector LookAt;
	switch (Shot)
	{
	case EEwigfelsDialogueShot::Wide:
	{
		const FVector Mid = (ListenerEye + SpeakerEye) * 0.5f;
		CamLoc = Mid + SideDir * (Dist * 1.1f + 180.f) - Axis * (Dist * 0.15f) + FVector(0.f, 0.f, 25.f);
		LookAt = Mid - FVector(0.f, 0.f, 15.f);
		OutFocal = 24.f;
		break;
	}
	case EEwigfelsDialogueShot::OverShoulder:
		// Hinter dem Zuhörer, leicht zur Seite, Schulter im Vordergrund
		CamLoc = ListenerEye - Axis * 95.f + SideDir * 42.f + FVector(0.f, 0.f, 6.f);
		LookAt = SpeakerEye;
		OutFocal = 40.f;
		break;
	case EEwigfelsDialogueShot::CloseUp:
	default:
		// Zwischen den beiden, auf der Seite der Achse, nah am Sprecher
		CamLoc = SpeakerEye - Axis * FMath::Min(Dist * 0.55f, 160.f) + SideDir * 38.f + FVector(0.f, 0.f, -2.f);
		LookAt = SpeakerEye - FVector(0.f, 0.f, 4.f);
		OutFocal = 85.f;
		break;
	}
	return FTransform(UKismetMathLibrary::FindLookAtRotation(CamLoc, LookAt), CamLoc);
}

ACineCameraActor* UEwigfelsDialogueCameraComponent::EnsureCamera()
{
	if (!CameraActor)
	{
		FActorSpawnParameters Params;
		Params.Owner = GetOwner();
		Params.ObjectFlags |= RF_Transient;
		CameraActor = GetWorld()->SpawnActor<ACineCameraActor>(ACineCameraActor::StaticClass(), FTransform::Identity, Params);
		if (CameraActor)
		{
			CameraActor->SetReplicates(false);
		}
	}
	return CameraActor;
}

void UEwigfelsDialogueCameraComponent::ApplyCamera(const FTransform& Transform, float Focal, const FVector& FocusPoint, float Blend)
{
	APlayerController* PC = GetPC();
	ACineCameraActor* Cam = EnsureCamera();
	if (!PC || !Cam)
	{
		return;
	}
	Cam->SetActorTransform(Transform);
	if (UCineCameraComponent* Cine = Cam->GetCineCameraComponent())
	{
		Cine->SetCurrentFocalLength(Focal);
		Cine->SetCurrentAperture(Aperture);
		Cine->FocusSettings.FocusMethod = ECameraFocusMethod::Manual;
		Cine->FocusSettings.ManualFocusDistance = FVector::Dist(Transform.GetLocation(), FocusPoint);
		Cine->FocusSettings.bSmoothFocusChanges = false;
	}
	if (PC->GetViewTarget() != Cam)
	{
		PC->SetViewTargetWithBlend(Cam, Blend, VTBlend_EaseInOut, 2.f);
	}
}

void UEwigfelsDialogueCameraComponent::BeginDialogue(AActor* Partner, const TArray<FEwigfelsDialogueLine>& Lines)
{
	APlayerController* PC = GetPC();
	if (!Partner || !PC)
	{
		return;
	}
	PartnerActor = Partner;
	Script = Lines;
	bActive = true;
	bFirstCut = true;

	// Seite der Achse = wo die Spielkamera gerade steht (fühlt sich wie ein natürlicher Übergang an)
	FVector ViewLoc;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewLoc, ViewRot);
	const FVector Axis = (Partner->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal2D();
	const FVector Right = FVector::CrossProduct(FVector::UpVector, Axis);
	Side = FVector::DotProduct(ViewLoc - GetOwner()->GetActorLocation(), Right) >= 0.f ? 1.f : -1.f;

	if (AEwigfelsCharacter* Character = Cast<AEwigfelsCharacter>(GetOwner()))
	{
		Character->SetGameplayInputBlocked(true);
		// Zum Gegenüber drehen
		Character->SetActorRotation(FRotator(0.f, Axis.Rotation().Yaw, 0.f));
	}
	OnDialogueCameraChanged.Broadcast(true);

	if (Script.Num() > 0)
	{
		ShowLine(0);
	}
	else
	{
		SetShot(EEwigfelsDialogueShot::Wide, true);
	}
}

void UEwigfelsDialogueCameraComponent::ShowLine(int32 Index)
{
	LineIndex = Index;
	const FEwigfelsDialogueLine& Line = Script[Index];
	SetShot(ChooseShot(Line, Index == 0), Line.bPartnerSpeaks);
}

void UEwigfelsDialogueCameraComponent::SetShot(EEwigfelsDialogueShot Shot, bool bOnPartner)
{
	const AActor* Partner = PartnerActor.Get();
	const AActor* Self = GetOwner();
	if (!bActive || !Partner || !Self)
	{
		return;
	}
	const FVector Speaker = GetEyeLocation(bOnPartner ? Partner : Self);
	const FVector Listener = GetEyeLocation(bOnPartner ? Self : Partner);
	// Seite bezogen auf die Achse Spieler→Partner; dreht sich die Blickrichtung um, Seite spiegeln
	const float ShotSide = bOnPartner ? Side : -Side;
	float Focal = 35.f;
	const FTransform Transform = ComputeShot(Shot, Listener, Speaker, ShotSide, Focal);
	ApplyCamera(Transform, Focal, Speaker, bFirstCut ? EnterBlendTime : 0.f);
	bFirstCut = false;
}

void UEwigfelsDialogueCameraComponent::Advance()
{
	if (!bActive)
	{
		return;
	}
	if (LineIndex + 1 < Script.Num())
	{
		ShowLine(LineIndex + 1);
	}
	else
	{
		EndDialogue();
	}
}

void UEwigfelsDialogueCameraComponent::EndDialogue()
{
	if (!bActive)
	{
		return;
	}
	bActive = false;
	LineIndex = INDEX_NONE;
	Script.Reset();
	if (APlayerController* PC = GetPC())
	{
		PC->SetViewTargetWithBlend(GetOwner(), EnterBlendTime, VTBlend_EaseInOut, 2.f);
	}
	if (AEwigfelsCharacter* Character = Cast<AEwigfelsCharacter>(GetOwner()))
	{
		Character->SetGameplayInputBlocked(false);
	}
	OnDialogueCameraChanged.Broadcast(false);
}

void UEwigfelsDialogueCameraComponent::HandleCue(FName Cue, const TArray<FString>& Arguments)
{
	if (!bActive || Arguments.Num() == 0)
	{
		return;
	}
	if (Cue == TEXT("camera"))
	{
		FEwigfelsDialogueLine Line;
		Line.Camera = FName(*Arguments[0]);
		const bool bOnPartner = Arguments.Num() < 2 || Arguments[1] != TEXT("player");
		SetShot(ChooseShot(Line, false), bOnPartner);
	}
}

bool UEwigfelsDialogueCameraComponent::GetCurrentLine(FEwigfelsDialogueLine& OutLine) const
{
	if (!bActive || !Script.IsValidIndex(LineIndex))
	{
		return false;
	}
	OutLine = Script[LineIndex];
	return true;
}
