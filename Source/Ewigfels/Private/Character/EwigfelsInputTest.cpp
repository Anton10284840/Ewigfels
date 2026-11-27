// Copyright Ewigfels. Alle Rechte vorbehalten.

// Test-Befehl "Ewigfels.Test.Steuerung [quit]": schickt simulierte Tasten direkt an den PlayerController (kein echtes
// Maus/Tastatur-Signal des Betriebssystems) und protokolliert, was passiert: Laufen, Sprinten, Springen, Zauberrad (RB),
// Weise wirken (Y) und ob Tasten des Game Animation Samples (Gehen auf RB, Ducken auf C, Ragdoll auf R) noch auslösen.
// Ergebnis im Log unter "[Steuerung-Test]". Nur Entwicklungs-Builds.

#include "Character/EwigfelsCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "InputKeyEventArgs.h"
#include "TimerManager.h"
#include "UI/EwigfelsCharacterCreator.h"

#if !UE_BUILD_SHIPPING

namespace EwigfelsInputTest
{
	FString GaitOf(AEwigfelsCharacter* Char)
	{
		FProperty* Prop = Char ? Char->GetClass()->FindPropertyByName(TEXT("Gait")) : nullptr;
		if (!Prop)
		{
			return TEXT("(kein Gait)");
		}
		FString Text;
		Prop->ExportTextItem_Direct(Text, Prop->ContainerPtrToValuePtr<void>(Char), nullptr, Char, PPF_None);
		return Text;
	}

	void Report(AEwigfelsCharacter* Char, const TCHAR* Label)
	{
		if (!Char)
		{
			return;
		}
		const UAnimInstance* Anim = Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr;
		const UAnimMontage* Montage = Anim ? Anim->GetCurrentActiveMontage() : nullptr;
		UE_LOG(LogTemp, Display, TEXT("[Steuerung-Test] %s: Tempo=%.0f Luft=%d Gait=%s Geduckt=%d Ragdoll=%d Rad=%d Montage=%s"),
			Label, Char->GetVelocity().Size2D(), Char->GetCharacterMovement()->IsFalling(), *GaitOf(Char),
			Char->bIsCrouched, Char->GetMesh() && Char->GetMesh()->IsSimulatingPhysics(), Char->IsSpellWheelOpen(),
			Montage ? *Montage->GetName() : TEXT("-"));
	}

	void Key(APlayerController* PC, const FKey& Key, EInputEvent Event)
	{
		if (PC)
		{
			PC->InputKey(FInputKeyEventArgs::CreateSimulated(Key, Event, Event == IE_Released ? 0.f : 1.f));
		}
	}

	void RunInput(UWorld* World, bool bQuit, int32 Tries = 0)
	{
		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		AEwigfelsCharacter* Char = PC ? Cast<AEwigfelsCharacter>(PC->GetPawn()) : nullptr;
		if (!Char || World->GetTimeSeconds() < 4.f)
		{
			if (World && Tries < 60)
			{
				FTimerHandle Retry;
				TWeakObjectPtr<UWorld> WeakWorld(World);
				World->GetTimerManager().SetTimer(Retry, FTimerDelegate::CreateLambda([WeakWorld, bQuit, Tries]()
				{
					if (WeakWorld.IsValid())
					{
						RunInput(WeakWorld.Get(), bQuit, Tries + 1);
					}
				}), 1.f, false);
			}
			return;
		}
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEwigfelsCreatorSubsystem* Creator = LP->GetSubsystem<UEwigfelsCreatorSubsystem>(); Creator && Creator->IsOpen())
			{
				Creator->Edit([](FEwigfelsAppearance& Look) { Look.Name = TEXT("Testfigur"); });
				Creator->Confirm();
			}
		}
		TWeakObjectPtr<AEwigfelsCharacter> Weak(Char);
		TWeakObjectPtr<APlayerController> WeakPC(PC);
		FTimerManager& Timers = World->GetTimerManager();
		auto At = [&Timers](float Seconds, TFunction<void()> Fn)
		{
			FTimerHandle Handle;
			Timers.SetTimer(Handle, FTimerDelegate::CreateLambda(MoveTemp(Fn)), Seconds, false);
		};
		auto Press = [WeakPC](const FKey& K) { Key(WeakPC.Get(), K, IE_Pressed); };
		auto Release = [WeakPC](const FKey& K) { Key(WeakPC.Get(), K, IE_Released); };
		auto Log = [Weak](const TCHAR* L) { Report(Weak.Get(), L); };

		At(2.0f, [Log]() { Log(TEXT("Start")); });
		// Laufen (W), dann Sprinten (Shift), dann Controller-Sprint (L3)
		At(2.5f, [Press]() { Press(EKeys::W); });
		At(3.5f, [Log]() { Log(TEXT("W gehalten")); });
		At(3.6f, [Press]() { Press(EKeys::LeftShift); });
		At(4.6f, [Log]() { Log(TEXT("W + Shift")); });
		At(4.7f, [Release]() { Release(EKeys::LeftShift); });
		At(4.8f, [Press]() { Press(EKeys::Gamepad_LeftThumbstick); });
		At(5.8f, [Log]() { Log(TEXT("W + L3")); });
		At(5.9f, [Release]() { Release(EKeys::Gamepad_LeftThumbstick); });
		At(6.0f, [Press]() { Press(EKeys::Gamepad_LeftShoulder); });
		At(7.0f, [Log]() { Log(TEXT("W + LB (Sample-Sprint, sollte geschluckt sein)")); });
		At(7.1f, [Release]() { Release(EKeys::Gamepad_LeftShoulder); });
		At(7.2f, [Release]() { Release(EKeys::W); });
		// Springen
		At(8.5f, [Press]() { Press(EKeys::SpaceBar); });
		At(8.7f, [Log]() { Log(TEXT("Leertaste")); });
		At(8.8f, [Release]() { Release(EKeys::SpaceBar); });
		// RB halten: Rad offen, kein Gehen
		At(10.0f, [Press]() { Press(EKeys::Gamepad_RightShoulder); });
		At(10.6f, [Log]() { Log(TEXT("RB gehalten")); });
		At(10.7f, [Press]() { Press(EKeys::W); });
		At(11.5f, [Log]() { Log(TEXT("RB + W (Gait darf nicht Walk sein)")); });
		At(11.6f, [Release]() { Release(EKeys::W); Release(EKeys::Gamepad_RightShoulder); });
		// Y: gewählte Weise wirken
		At(13.0f, [Press]() { Press(EKeys::Gamepad_FaceButton_Top); });
		At(13.25f, [Log]() { Log(TEXT("Y")); });
		At(13.3f, [Release]() { Release(EKeys::Gamepad_FaceButton_Top); });
		// C (Sample: Ducken, bei uns Ausweichen), R (Sample: Ragdoll)
		At(15.0f, [Press]() { Press(EKeys::C); });
		At(15.4f, [Log]() { Log(TEXT("C")); });
		At(15.5f, [Release]() { Release(EKeys::C); });
		At(16.5f, [Press]() { Press(EKeys::R); });
		At(17.0f, [Log]() { Log(TEXT("R")); });
		At(17.1f, [Release]() { Release(EKeys::R); });
		At(18.0f, [bQuit]()
		{
			UE_LOG(LogTemp, Display, TEXT("[Steuerung-Test] fertig"));
			if (bQuit)
			{
				FPlatformMisc::RequestExit(false);
			}
		});
	}

	FAutoConsoleCommandWithWorldAndArgs GTestInput(
		TEXT("Ewigfels.Test.Steuerung"),
		TEXT("Simulierte Tasten an den PlayerController: Laufen/Sprinten/Springen/Rad/Weise + Sample-Tasten prüfen, Log [Steuerung-Test] (Argument quit = danach beenden)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			RunInput(World, Args.Contains(TEXT("quit")));
		}));
}

#endif
