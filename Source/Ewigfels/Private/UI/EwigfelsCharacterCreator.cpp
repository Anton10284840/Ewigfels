// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "UI/EwigfelsCharacterCreator.h"
#include "UI/SEwigfelsCreatorWidget.h"
#include "Appearance/EwigfelsAppearanceComponent.h"
#include "Appearance/EwigfelsAppearanceSubsystem.h"
#include "Character/EwigfelsCharacter.h"
#include "Ewigfels.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Engine/PointLight.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

#define LOCTEXT_NAMESPACE "EwigfelsCreator"

namespace
{
	constexpr int32 MaxUndo = 60;

	bool IsFaceTab(EEwigfelsCreatorTab Tab)
	{
		return Tab == EEwigfelsCreatorTab::Face || Tab == EEwigfelsCreatorTab::SkinEyes || Tab == EEwigfelsCreatorTab::Hair
			|| Tab == EEwigfelsCreatorTab::Marks;
	}
}

UEwigfelsAppearanceSubsystem* UEwigfelsCreatorSubsystem::GetLookSubsystem() const
{
	const ULocalPlayer* LP = GetLocalPlayer();
	UGameInstance* GI = LP ? LP->GetGameInstance() : nullptr;
	return GI ? GI->GetSubsystem<UEwigfelsAppearanceSubsystem>() : nullptr;
}

const FEwigfelsLookCatalog& UEwigfelsCreatorSubsystem::GetCatalog() const
{
	static const FEwigfelsLookCatalog Empty;
	const UEwigfelsAppearanceSubsystem* Sub = GetLookSubsystem();
	return Sub ? Sub->GetCatalog() : Empty;
}

const FEwigfelsLookShop* UEwigfelsCreatorSubsystem::GetShop() const
{
	return GetCatalog().Shops.Find(ShopId);
}

void UEwigfelsCreatorSubsystem::Deinitialize()
{
	if (IsOpen())
	{
		Close();
	}
	Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// Öffnen / Schließen

bool UEwigfelsCreatorSubsystem::Open(UEwigfelsAppearanceComponent* InTarget, FName InShopId)
{
	UEwigfelsAppearanceSubsystem* Sub = GetLookSubsystem();
	ULocalPlayer* LP = GetLocalPlayer();
	APlayerController* PC = LP ? LP->GetPlayerController(LP->GetWorld()) : nullptr;
	if (IsOpen() || !InTarget || !Sub || !PC || !LP->ViewportClient)
	{
		return false;
	}
	if (!InShopId.IsNone() && !Sub->GetCatalog().Shops.Contains(InShopId))
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Spiegel: Laden '%s' gibt es nicht (Shops.json)"), *InShopId.ToString());
		return false;
	}

	Target = InTarget;
	ShopId = InShopId;
	Context = Sub->MakeContext(ShopId);
	Original = InTarget->GetAppearance();
	Working = IsCreation() && !Original.bCreated ? Sub->GetStartingLook() : Original;
	EwigfelsLook::Sanitize(Working, Sub->GetCatalog());
	UndoStack.Reset();
	RedoStack.Reset();
	bWaiting = false;
	StatusText = FText::GetEmpty();
	OrbitYaw = 0.f;
	OrbitPitch = -5.f;
	ZoomAlpha = 0.f;
	CurrentTab = EEwigfelsCreatorTab::Identity;
	if (!IsCreation())
	{
		for (uint8 i = 0; i <= static_cast<uint8>(EEwigfelsCreatorTab::Ring); ++i)
		{
			if (IsTabVisible(static_cast<EEwigfelsCreatorTab>(i)))
			{
				CurrentTab = static_cast<EEwigfelsCreatorTab>(i);
				break;
			}
		}
	}

	InTarget->OnChangeResult.AddDynamic(this, &UEwigfelsCreatorSubsystem::HandleChangeResult);
	InTarget->OnAppearanceReplaced.AddDynamic(this, &UEwigfelsCreatorSubsystem::HandleAppearanceApplied);

	if (AEwigfelsCharacter* Character = Cast<AEwigfelsCharacter>(InTarget->GetOwner()))
	{
		Character->SetGameplayInputBlocked(true);
	}
	if (PC->GetHUD())
	{
		PC->GetHUD()->bShowHUD = false;
	}

	SpawnStage();
	ApplyPreview();

	Widget = SNew(SEwigfelsCreatorWidget).Creator(this);
	WidgetContainer = Widget;
	LP->ViewportClient->AddViewportWidgetForPlayer(LP, Widget.ToSharedRef(), 50);

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(Widget);
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(Mode);
	PC->SetShowMouseCursor(true);

	UE_LOG(LogEwigfels, Log, TEXT("Spiegel: %s geoeffnet"), IsCreation() ? TEXT("Charakter-Editor") : *ShopId.ToString());
	return true;
}

void UEwigfelsCreatorSubsystem::Cancel()
{
	if (!IsOpen() || bWaiting)
	{
		return;
	}
	if (IsCreation() && !Target->IsCreated())
	{
		StatusText = LOCTEXT("MustCreate", "Erst „Fertig“ drücken – ohne Figur geht es nicht los.");
		return;
	}
	Target->ClearPreview();
	Close();
}

void UEwigfelsCreatorSubsystem::Close()
{
	ULocalPlayer* LP = GetLocalPlayer();
	APlayerController* PC = LP ? LP->GetPlayerController(LP->GetWorld()) : nullptr;

	if (UEwigfelsAppearanceComponent* T = Target.Get())
	{
		T->OnChangeResult.RemoveDynamic(this, &UEwigfelsCreatorSubsystem::HandleChangeResult);
		T->OnAppearanceReplaced.RemoveDynamic(this, &UEwigfelsCreatorSubsystem::HandleAppearanceApplied);
		if (AEwigfelsCharacter* Character = Cast<AEwigfelsCharacter>(T->GetOwner()))
		{
			Character->SetGameplayInputBlocked(false);
		}
	}
	if (LP && LP->ViewportClient && Widget.IsValid())
	{
		LP->ViewportClient->RemoveViewportWidgetForPlayer(LP, Widget.ToSharedRef());
	}
	Widget.Reset();
	WidgetContainer.Reset();

	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
		if (PC->GetHUD())
		{
			PC->GetHUD()->bShowHUD = true;
		}
		if (APawn* Pawn = PC->GetPawn())
		{
			PC->SetViewTargetWithBlend(Pawn, 0.5f, VTBlend_Cubic);
		}
	}
	DestroyStage();
	Target.Reset();
	bWaiting = false;
}

// ---------------------------------------------------------------------------
// Bearbeiten

void UEwigfelsCreatorSubsystem::Edit(TFunctionRef<void(FEwigfelsAppearance&)> Change)
{
	if (!IsOpen() || bWaiting)
	{
		return;
	}
	UndoStack.Push(Working);
	if (UndoStack.Num() > MaxUndo)
	{
		UndoStack.RemoveAt(0);
	}
	RedoStack.Reset();
	Change(Working);
	EwigfelsLook::Sanitize(Working, GetCatalog());
	ApplyPreview();
}

void UEwigfelsCreatorSubsystem::EditLive(TFunctionRef<void(FEwigfelsAppearance&)> Change)
{
	if (!IsOpen() || bWaiting)
	{
		return;
	}
	Change(Working);
	ApplyPreview();
}

void UEwigfelsCreatorSubsystem::BeginDrag()
{
	if (!IsOpen() || bWaiting)
	{
		return;
	}
	UndoStack.Push(Working);
	if (UndoStack.Num() > MaxUndo)
	{
		UndoStack.RemoveAt(0);
	}
	RedoStack.Reset();
}

void UEwigfelsCreatorSubsystem::Undo()
{
	if (UndoStack.Num() == 0 || bWaiting)
	{
		return;
	}
	RedoStack.Push(Working);
	Working = UndoStack.Pop();
	ApplyPreview();
}

void UEwigfelsCreatorSubsystem::Redo()
{
	if (RedoStack.Num() == 0 || bWaiting)
	{
		return;
	}
	UndoStack.Push(Working);
	Working = RedoStack.Pop();
	ApplyPreview();
}

void UEwigfelsCreatorSubsystem::Randomize()
{
	if (!IsCreation())
	{
		return; // im Laden würde Zufall Dinge ändern, die der Laden nicht anbietet
	}
	FRandomStream Random(FPlatformTime::Cycles());
	const FEwigfelsAppearance Rolled = EwigfelsLook::MakeRandom(GetCatalog(), Random);
	Edit([&Rolled](FEwigfelsAppearance& L)
	{
		const FString Name = L.Name;
		const EEwigfelsPronouns Pronouns = L.Pronouns;
		L = Rolled;
		L.Name = Name;          // Name und Anrede bleiben
		L.Pronouns = Pronouns;
	});
}

void UEwigfelsCreatorSubsystem::ResetToOriginal()
{
	const UEwigfelsAppearanceSubsystem* Sub = GetLookSubsystem();
	const FEwigfelsAppearance Start = IsCreation() && !Original.bCreated && Sub ? Sub->GetStartingLook() : Original;
	Edit([&Start](FEwigfelsAppearance& L) { L = Start; });
}

void UEwigfelsCreatorSubsystem::ApplyPreview()
{
	if (UEwigfelsAppearanceComponent* T = Target.Get())
	{
		T->SetPreview(Working);
	}
	StatusText = FText::GetEmpty();
}

bool UEwigfelsCreatorSubsystem::IsAreaEditable(FName Area) const
{
	if (IsCreation())
	{
		return true;
	}
	const FEwigfelsLookShop* Shop = GetShop();
	return Shop && EwigfelsLook::ShopAllows(*Shop, Area);
}

bool UEwigfelsCreatorSubsystem::IsTabVisible(EEwigfelsCreatorTab Tab) const
{
	if (IsCreation())
	{
		return true;
	}
	using namespace EwigfelsLook;
	switch (Tab)
	{
	case EEwigfelsCreatorTab::Identity: return IsAreaEditable(Change::Name);
	case EEwigfelsCreatorTab::Body: return IsAreaEditable(Change::Body);
	case EEwigfelsCreatorTab::Face: return IsAreaEditable(Change::Face);
	case EEwigfelsCreatorTab::SkinEyes: return IsAreaEditable(Change::Skin) || IsAreaEditable(Change::Eyes);
	case EEwigfelsCreatorTab::Hair: return IsAreaEditable(Change::Hair) || IsAreaEditable(Change::Beard) || IsAreaEditable(Change::Brows);
	case EEwigfelsCreatorTab::Marks: return IsAreaEditable(Change::Marks);
	case EEwigfelsCreatorTab::Voice: return IsAreaEditable(Change::Voice);
	case EEwigfelsCreatorTab::Ring: return IsAreaEditable(Change::Rings);
	case EEwigfelsCreatorTab::Outfit:
		for (const FName Slot : GetCatalog().OutfitSlots)
		{
			if (IsAreaEditable(FName(*(TEXT("outfit:") + Slot.ToString()))) || IsAreaEditable(FName(*(TEXT("outfit_colors:") + Slot.ToString()))))
			{
				return true;
			}
		}
		return false;
	}
	return false;
}

bool UEwigfelsCreatorSubsystem::IsOptionAvailable(const FEwigfelsLookOption& Option) const
{
	return EwigfelsLook::IsUnlocked(Option, Context);
}

bool UEwigfelsCreatorSubsystem::PreviewCheck(int32& OutPrice, FText& OutReason) const
{
	const UEwigfelsAppearanceComponent* T = Target.Get();
	if (!T)
	{
		OutPrice = 0;
		return false;
	}
	FEwigfelsAppearance Clean = Working;
	Clean.bCreated = T->GetAppearance().bCreated;
	EwigfelsLook::Sanitize(Clean, GetCatalog());
	return EwigfelsLook::CheckChange(T->GetAppearance(), Clean, GetCatalog(), Context, OutPrice, OutReason);
}

int32 UEwigfelsCreatorSubsystem::GetMoney() const
{
	const UEwigfelsAppearanceSubsystem* Sub = GetLookSubsystem();
	const UEwigfelsAppearanceComponent* T = Target.Get();
	return Sub && Sub->MoneyQuery && T ? Sub->MoneyQuery(Cast<APawn>(T->GetOwner())) : -1;
}

void UEwigfelsCreatorSubsystem::Confirm()
{
	UEwigfelsAppearanceComponent* T = Target.Get();
	if (!T || bWaiting)
	{
		return;
	}
	int32 Price = 0;
	FText Reason;
	if (!PreviewCheck(Price, Reason))
	{
		StatusText = Reason;
		return;
	}
	if (!IsCreation() && EwigfelsLook::GetChangedAreas(T->GetAppearance(), Working).Num() == 0)
	{
		Cancel(); // nichts geändert
		return;
	}
	bWaiting = true;
	StatusText = LOCTEXT("Waiting", "Einen Moment …");
	T->RequestChange(Working, ShopId);
}

void UEwigfelsCreatorSubsystem::HandleChangeResult(bool bAccepted, int32 Price, const FText& Reason)
{
	if (!IsOpen())
	{
		return;
	}
	bWaiting = false;
	if (!bAccepted)
	{
		StatusText = Reason;
		return;
	}
	UEwigfelsAppearanceComponent* T = Target.Get();
	const bool bWasCreation = IsCreation();
	const FString Name = Working.Name;
	Close();
	if (T)
	{
		T->ClearPreview(true); // Optik zeigt schon das Ergebnis, kein Flackern
	}
	if (AEwigfelsCharacter* Character = T ? Cast<AEwigfelsCharacter>(T->GetOwner()) : nullptr)
	{
		const FText Message = bWasCreation
			? FText::Format(LOCTEXT("Welcome", "Willkommen in Nocturna, {0}."), FText::FromString(Name))
			: Price > 0 ? FText::Format(LOCTEXT("Paid", "Steht dir gut! ({0} Kronen)"), Price) : LOCTEXT("Saved", "Steht dir gut!");
		Character->ShowMessage(Message, FLinearColor(1.f, 0.85f, 0.55f), 3.f);
	}
}

void UEwigfelsCreatorSubsystem::HandleAppearanceApplied(const FEwigfelsAppearance& Look)
{
	// Spielstand wurde geladen, während der Editor offen war → Figur existiert, Editor zu
	const UEwigfelsAppearanceComponent* T = Target.Get();
	if (IsCreation() && T && T->IsCreated() && !bWaiting && !Original.bCreated)
	{
		Target->ClearPreview();
		Close();
	}
}

// ---------------------------------------------------------------------------
// Reiter, Kamera, Licht

void UEwigfelsCreatorSubsystem::SetTab(EEwigfelsCreatorTab Tab)
{
	if (!IsTabVisible(Tab))
	{
		return;
	}
	const bool bFaceChange = IsFaceTab(Tab) != IsFaceTab(CurrentTab);
	CurrentTab = Tab;
	if (bFaceChange)
	{
		ZoomAlpha = 0.f;
	}
	++Revision;
	UpdateCamera(false);
}

void UEwigfelsCreatorSubsystem::Orbit(float DeltaYaw, float DeltaPitch)
{
	OrbitYaw = FMath::Fmod(OrbitYaw + DeltaYaw + 360.f, 360.f);
	OrbitPitch = FMath::Clamp(OrbitPitch + DeltaPitch, -35.f, 25.f);
	UpdateCamera(true);
}

void UEwigfelsCreatorSubsystem::Zoom(float Delta)
{
	ZoomAlpha = FMath::Clamp(ZoomAlpha + Delta, 0.f, 1.f);
	UpdateCamera(true);
}

void UEwigfelsCreatorSubsystem::SpawnStage()
{
	const UEwigfelsAppearanceComponent* T = Target.Get();
	UWorld* World = T ? T->GetWorld() : nullptr;
	const AActor* Owner = T ? T->GetOwner() : nullptr;
	ULocalPlayer* LP = GetLocalPlayer();
	APlayerController* PC = LP ? LP->GetPlayerController(LP->GetWorld()) : nullptr;
	if (!World || !Owner || !PC)
	{
		return;
	}
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.ObjectFlags |= RF_Transient;

	Camera = World->SpawnActor<ACameraActor>(Owner->GetActorLocation(), FRotator::ZeroRotator, Params);
	if (Camera)
	{
		Camera->GetCameraComponent()->SetFieldOfView(32.f);
		Camera->GetCameraComponent()->bConstrainAspectRatio = false;
	}

	// Porträt-Licht: warmes Führungslicht vorne links, kühles Kantenlicht hinten rechts
	const FVector Base = Owner->GetActorLocation();
	const FVector Fwd = Owner->GetActorForwardVector();
	const FVector Right = Owner->GetActorRightVector();
	auto MakeLight = [&](const FVector& Offset, const FLinearColor& Color, float Intensity) -> APointLight*
	{
		APointLight* Light = World->SpawnActor<APointLight>(Base + Offset, FRotator::ZeroRotator, Params);
		if (Light)
		{
			Light->SetMobility(EComponentMobility::Movable);
			Light->PointLight->SetIntensity(Intensity);
			Light->PointLight->SetAttenuationRadius(700.f);
			Light->PointLight->SetLightColor(Color);
			Light->PointLight->SetCastShadows(false);
		}
		return Light;
	};
	KeyLight = MakeLight(Fwd * 170.f - Right * 120.f + FVector(0.f, 0.f, 90.f), FLinearColor(1.f, 0.88f, 0.72f), 9000.f);
	RimLight = MakeLight(-Fwd * 140.f + Right * 110.f + FVector(0.f, 0.f, 120.f), FLinearColor(0.6f, 0.75f, 1.f), 7000.f);

	UpdateCamera(true);
	if (Camera)
	{
		PC->SetViewTargetWithBlend(Camera, 0.7f, VTBlend_Cubic);
	}
}

void UEwigfelsCreatorSubsystem::DestroyStage()
{
	for (AActor* Actor : TArray<AActor*>{ Camera.Get(), KeyLight.Get(), RimLight.Get() })
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	Camera = nullptr;
	KeyLight = nullptr;
	RimLight = nullptr;
}

void UEwigfelsCreatorSubsystem::UpdateCamera(bool bInstant)
{
	const UEwigfelsAppearanceComponent* T = Target.Get();
	const ACharacter* Owner = T ? Cast<ACharacter>(T->GetOwner()) : nullptr;
	if (!Camera || !Owner)
	{
		return;
	}
	const float HalfHeight = Owner->GetCapsuleComponent() ? Owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() : 88.f;
	const bool bFace = IsFaceTab(CurrentTab);
	// Kapselmitte = Hüfte; Kopf ≈ 80 % der halben Höhe darüber
	const FVector Focus = Owner->GetActorLocation() + FVector(0.f, 0.f, bFace ? HalfHeight * 0.8f * Working.Height : HalfHeight * 0.05f);
	const float Distance = FMath::Lerp(bFace ? 95.f : 330.f, bFace ? 55.f : 170.f, ZoomAlpha);
	const FRotator View(OrbitPitch, Owner->GetActorRotation().Yaw + 180.f + OrbitYaw, 0.f);
	// Leicht nach rechts versetzt, damit die Figur neben dem Bedienfeld steht
	const FVector Side = FRotationMatrix(View).GetUnitAxis(EAxis::Y) * (bFace ? 14.f : 55.f);
	Camera->SetActorLocationAndRotation(Focus - View.Vector() * Distance + Side, View);
}

void UEwigfelsCreatorSubsystem::PlayVoiceSample() const
{
	const FEwigfelsLookOption* Voice = GetCatalog().Find(EwigfelsLook::Cat::Voice, Working.Voice);
	const UEwigfelsAppearanceComponent* T = Target.Get();
	USoundBase* Sound = Voice ? Cast<USoundBase>(EwigfelsSpellVisuals::LoadIfExists(Voice->Asset)) : nullptr;
	if (Sound && T)
	{
		UGameplayStatics::PlaySound2D(T, Sound, 1.f, 1.f + Working.VoicePitch * 0.15f);
	}
	else
	{
		UE_LOG(LogEwigfels, Log, TEXT("Spiegel: Stimmprobe fuer '%s' fehlt noch (%s)"), *Working.Voice.ToString(), Voice ? *Voice->Asset.ToString() : TEXT("-"));
	}
}

#undef LOCTEXT_NAMESPACE
