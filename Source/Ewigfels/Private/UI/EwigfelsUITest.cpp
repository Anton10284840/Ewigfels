// Copyright Ewigfels. Alle Rechte vorbehalten.

// Test-Befehl "Ewigfels.Test.UI [quit]": zeigt nacheinander Kampf-Anzeige, Zauberrad (Maus und Controller) und Zauberbuch
// (Maus und Controller) und macht Screenshots nach Saved/Screenshots/UI. Ohne Maus/Tastatur, damit er per -ExecCmds
// im Hintergrund laufen kann. Nur Entwicklungs-Builds.

#include "Character/EwigfelsCharacter.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Misc/Paths.h"
#include "TimerManager.h"
#include "UI/EwigfelsCharacterCreator.h"
#include "UnrealClient.h"

#if !UE_BUILD_SHIPPING

namespace EwigfelsUITest
{
	void UIShot(const FString& Name)
	{
		const FString File = FPaths::ProjectSavedDir() / TEXT("Screenshots/UI") / (Name + TEXT(".png"));
		FScreenshotRequest::RequestScreenshot(File, true, false);
		UE_LOG(LogTemp, Display, TEXT("[UI-Test] Screenshot %s"), *Name);
	}

	void RunUI(UWorld* World, bool bQuit, int32 Tries = 0)
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
						RunUI(WeakWorld.Get(), bQuit, Tries + 1);
					}
				}), 1.f, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[UI-Test] keine Spielfigur"));
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
		FTimerManager& Timers = World->GetTimerManager();
		auto At = [&Timers](float Seconds, TFunction<void()> Fn)
		{
			FTimerHandle Handle;
			Timers.SetTimer(Handle, FTimerDelegate::CreateLambda(MoveTemp(Fn)), Seconds, false);
		};

		// 1) Kampf-Anzeige, einmal mit Abklingzeit (Platz 2 wirken)
		At(3.0f, []() { UIShot(TEXT("1_HUD")); });
		At(3.5f, [Weak]() { if (Weak.IsValid()) { Weak->TestCastSlot(2); } });
		At(4.3f, []() { UIShot(TEXT("2_HUD_Abklingzeit")); });
		// 2) Zauberrad mit Maus (Zeiger nach rechts oben), dann Controller (nach links)
		At(6.0f, [Weak]() { if (Weak.IsValid()) { Weak->TestOpenWheel(FVector2D(0.7f, -0.7f), false); } });
		At(6.6f, [Weak]() { UE_LOG(LogTemp, Display, TEXT("[UI-Test] Rad offen=%d Hover=%d"), Weak.IsValid() && Weak->IsSpellWheelOpen(), Weak.IsValid() ? Weak->GetWheelHover() : -1); UIShot(TEXT("3_Zauberrad_Maus")); });
		At(7.2f, [Weak]() { if (Weak.IsValid()) { Weak->TestCloseWheel(); } });
		At(8.0f, [Weak]() { if (Weak.IsValid()) { Weak->TestOpenWheel(FVector2D(-1.f, 0.f), true); } });
		At(8.6f, [Weak]() { UE_LOG(LogTemp, Display, TEXT("[UI-Test] Rad offen=%d Hover=%d"), Weak.IsValid() && Weak->IsSpellWheelOpen(), Weak.IsValid() ? Weak->GetWheelHover() : -1); UIShot(TEXT("4_Zauberrad_Controller")); });
		At(9.2f, [Weak]() { if (Weak.IsValid()) { Weak->TestCloseWheel(); UE_LOG(LogTemp, Display, TEXT("[UI-Test] gewaehlt=%d"), Weak->GetSelectedSlot()); } });
		At(9.8f, []() { UIShot(TEXT("5_nach_Radwahl")); });
		// 3) Zauberbuch mit Maus/Tastatur, dann Controller
		At(11.0f, [Weak]() { if (Weak.IsValid()) { Weak->TestToggleSpellbook(false); } });
		At(11.8f, []() { UIShot(TEXT("6_Zauberbuch_Maus")); });
		At(12.4f, [Weak]() { if (Weak.IsValid()) { Weak->TestToggleSpellbook(false); } });
		At(13.2f, [Weak]() { if (Weak.IsValid()) { Weak->TestToggleSpellbook(true); } });
		At(14.0f, []() { UIShot(TEXT("7_Zauberbuch_Controller")); });
		At(14.6f, [Weak]() { if (Weak.IsValid()) { Weak->TestToggleSpellbook(true); } });
		At(16.0f, [bQuit]()
		{
			UE_LOG(LogTemp, Display, TEXT("[UI-Test] fertig"));
			if (bQuit)
			{
				FPlatformMisc::RequestExit(false);
			}
		});
	}

	FAutoConsoleCommandWithWorldAndArgs GTestUI(
		TEXT("Ewigfels.Test.UI"),
		TEXT("Kampf-Anzeige, Zauberrad und Zauberbuch zeigen, Screenshots nach Saved/Screenshots/UI (Argument quit = danach beenden)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			RunUI(World, Args.Contains(TEXT("quit")));
		}));
}

#endif
