// Copyright Ewigfels. Alle Rechte vorbehalten.

// Test-Befehl "Ewigfels.Test.Gesten [quit]": löst linke Maus und Plätze 1–7 nacheinander aus, danach zwei Zauber im Laufen,
// protokolliert Montage + Oberkörper-Gewicht und macht Screenshots (Saved/Screenshots/...). Ohne Maus/Tastatur,
// damit man ihn per -ExecCmds im Hintergrund laufen lassen kann. Nur Entwicklungs-Builds.

#include "Character/EwigfelsCharacter.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
#include "UI/EwigfelsCharacterCreator.h"
#include "UnrealClient.h"

#if !UE_BUILD_SHIPPING

namespace EwigfelsGestureTest
{
	void LogState(AEwigfelsCharacter* Char, const TCHAR* Label)
	{
		UAnimInstance* Anim = Char && Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr;
		if (!Anim)
		{
			return;
		}
		const UAnimMontage* Montage = Anim->GetCurrentActiveMontage();
		UE_LOG(LogTemp, Display, TEXT("[Gesten-Test] %s: Montage=%s UpperBody=%.2f DefaultSlot=%.2f Tempo=%.0f"),
			Label, Montage ? *Montage->GetName() : TEXT("-"),
			Anim->GetSlotMontageGlobalWeight(FName(TEXT("UpperBody"))),
			Anim->GetSlotMontageGlobalWeight(FName(TEXT("DefaultSlot"))),
			Char->GetVelocity().Size2D());
	}

	void Shot(const FString& Name)
	{
		const FString File = FPaths::ProjectSavedDir() / TEXT("Screenshots/Gesten") / (Name + TEXT(".png"));
		FScreenshotRequest::RequestScreenshot(File, false, false);
	}

	void Run(UWorld* World, bool bQuit, int32 Tries = 0)
	{
		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		AEwigfelsCharacter* Char = PC ? Cast<AEwigfelsCharacter>(PC->GetPawn()) : nullptr;
		if (!Char || World->GetTimeSeconds() < 4.f)
		{
			// Beim Start per -ExecCmds gibt es die Figur noch nicht: kurz warten
			if (World && Tries < 60)
			{
				FTimerHandle Retry;
				TWeakObjectPtr<UWorld> WeakWorld(World);
				World->GetTimerManager().SetTimer(Retry, FTimerDelegate::CreateLambda([WeakWorld, bQuit, Tries]()
				{
					if (WeakWorld.IsValid())
					{
						Run(WeakWorld.Get(), bQuit, Tries + 1);
					}
				}), 1.f, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[Gesten-Test] keine Spielfigur"));
			}
			return;
		}
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEwigfelsCreatorSubsystem* Creator = LP->GetSubsystem<UEwigfelsCreatorSubsystem>(); Creator && Creator->IsOpen())
			{
				// Erstellung lässt sich nicht abbrechen: Testnamen setzen und bestätigen
				Creator->Edit([](FEwigfelsAppearance& Look) { Look.Name = TEXT("Testfigur"); });
				Creator->Confirm();
			}
		}
		TWeakObjectPtr<AEwigfelsCharacter> Weak(Char);
		FTimerManager& Timers = World->GetTimerManager();
		auto At = [&Timers](float Seconds, TFunction<void()> Fn)
		{
			FTimerHandle Handle;
			Timers.SetTimer(Handle, FTimerDelegate::CreateLambda(MoveTemp(Fn)), Seconds, false);
		};

		float T = 6.0f;
		for (int32 Slot = 0; Slot <= 7; ++Slot)
		{
			const FString Name = Slot == 0 ? FString(TEXT("0_linke_Maus")) : FString::Printf(TEXT("%d_Taste"), Slot);
			At(T, [Weak, Slot]() { if (Weak.IsValid()) { Weak->TestCastSlot(Slot); } });
			At(T + 0.22f, [Weak, Name]() { LogState(Weak.Get(), *Name); Shot(Name + TEXT("_a")); });
			At(T + 0.45f, [Weak, Name]() { LogState(Weak.Get(), *(Name + TEXT(" spaeter"))); Shot(Name + TEXT("_b")); });
			T += 2.6f;
		}

		// Im Laufen: vorwärts laufen lassen, dabei Glutweise (6) und linke Maus
		const float WalkStart = T;
		At(WalkStart, [Weak]()
		{
			// jedes Bild Vorwärts-Eingabe, 3,2 s lang (Timer wären zu selten)
			const double End = FPlatformTime::Seconds() + 3.2;
			FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Weak, End](float)
			{
				if (!Weak.IsValid() || FPlatformTime::Seconds() > End)
				{
					return false;
				}
				Weak->AddMovementInput(Weak->GetActorForwardVector(), 1.f);
				return true;
			}));
		});
		At(WalkStart + 0.9f, [Weak]() { if (Weak.IsValid()) { Weak->TestCastSlot(6); } });
		At(WalkStart + 1.12f, [Weak]() { LogState(Weak.Get(), TEXT("Laufen + 6")); Shot(TEXT("9_Laufen_Glut")); });
		At(WalkStart + 2.0f, [Weak]() { if (Weak.IsValid()) { Weak->TestCastSlot(0); } });
		At(WalkStart + 2.22f, [Weak]() { LogState(Weak.Get(), TEXT("Laufen + LMB")); Shot(TEXT("9_Laufen_LMB")); });
		At(WalkStart + 4.0f, [bQuit]()
		{
			UE_LOG(LogTemp, Display, TEXT("[Gesten-Test] fertig"));
			if (bQuit)
			{
				FPlatformMisc::RequestExit(false);
			}
		});
	}

	FAutoConsoleCommandWithWorldAndArgs GTestGestures(
		TEXT("Ewigfels.Test.Gesten"),
		TEXT("Alle Zauber-Gesten nacheinander auslösen, Screenshots nach Saved/Screenshots/Gesten (Argument quit = danach beenden)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			Run(World, Args.Contains(TEXT("quit")));
		}));
}

#endif
