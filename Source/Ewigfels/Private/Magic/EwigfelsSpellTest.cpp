// Copyright Ewigfels. Alle Rechte vorbehalten.

// Test-Befehl "Ewigfels.Test.Weisen [id ...] [quit]": legt jede Weise auf Platz 1, wirkt sie und macht zwei Screenshots
// (Saved/Screenshots/Weisen/<id>_a/_b). Ohne ids: alle Weisen aus Kampfweisen2.json. Atem wird vorher aufgefüllt.
// Ohne Maus/Tastatur, damit er per -ExecCmds im Hintergrund laufen kann. Nur Entwicklungs-Builds.

#include "Character/EwigfelsCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Misc/Paths.h"
#include "TimerManager.h"
#include "UI/EwigfelsCharacterCreator.h"
#include "UnrealClient.h"

#if !UE_BUILD_SHIPPING

namespace EwigfelsSpellTest
{
	void SpellShot(const FString& Name)
	{
		const FString File = FPaths::ProjectSavedDir() / TEXT("Screenshots/Weisen") / (Name + TEXT(".png"));
		FScreenshotRequest::RequestScreenshot(File, true, false);
	}

	void RunSpells(UWorld* World, TArray<FName> Ids, bool bQuit, int32 Tries = 0)
	{
		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		AEwigfelsCharacter* Char = PC ? Cast<AEwigfelsCharacter>(PC->GetPawn()) : nullptr;
		if (!Char || World->GetTimeSeconds() < 4.f)
		{
			if (World && Tries < 60)
			{
				FTimerHandle Retry;
				TWeakObjectPtr<UWorld> WeakWorld(World);
				World->GetTimerManager().SetTimer(Retry, FTimerDelegate::CreateLambda([WeakWorld, Ids, bQuit, Tries]()
				{
					if (WeakWorld.IsValid())
					{
						RunSpells(WeakWorld.Get(), Ids, bQuit, Tries + 1);
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
		FTimerManager& Timers = World->GetTimerManager();
		auto At = [&Timers](float Seconds, TFunction<void()> Fn)
		{
			FTimerHandle Handle;
			Timers.SetTimer(Handle, FTimerDelegate::CreateLambda(MoveTemp(Fn)), Seconds, false);
		};

		float T = 2.f;
		for (const FName Id : Ids)
		{
			At(T, [Weak, Id]()
			{
				if (!Weak.IsValid())
				{
					return;
				}
				UEwigfelsSpellCasterComponent* Caster = Weak->GetCaster();
				Caster->LearnSpell(Id);
				Caster->RequestSetWheelSlot(1, Id);
				Caster->RestoreAtem(1000.f);
			});
			At(T + 0.3f, [Weak, Id]()
			{
				if (!Weak.IsValid())
				{
					return;
				}
				const FName Slot = Weak->GetCaster()->GetSlotSpell(1);
				Weak->TestCastSlot(1);
				UE_LOG(LogTemp, Display, TEXT("[Weisen-Test] %s: Platz1=%s"), *Id.ToString(), *Slot.ToString());
			});
			At(T + 0.4f, [Weak, Id]()
			{
				const UAnimInstance* Anim = Weak.IsValid() && Weak->GetMesh() ? Weak->GetMesh()->GetAnimInstance() : nullptr;
				const UAnimMontage* Montage = Anim ? Anim->GetCurrentActiveMontage() : nullptr;
				UE_LOG(LogTemp, Display, TEXT("[Weisen-Test] %s: nach 0,1 s Montage=%s"), *Id.ToString(), Montage ? *Montage->GetName() : TEXT("-"));
			});
			At(T + 0.6f, [Weak, Id]()
			{
				const UAnimInstance* Anim = Weak.IsValid() && Weak->GetMesh() ? Weak->GetMesh()->GetAnimInstance() : nullptr;
				const UAnimMontage* Montage = Anim ? Anim->GetCurrentActiveMontage() : nullptr;
				FText Msg; FLinearColor Color; float Alpha = 0.f;
				const bool bMsg = Weak.IsValid() && Weak->GetMessage(Msg, Color, Alpha);
				UE_LOG(LogTemp, Display, TEXT("[Weisen-Test] %s: Abklingzeit=%.1f Atem=%.0f Montage=%s Hinweis=%s"), *Id.ToString(),
					Weak.IsValid() ? Weak->GetCaster()->GetCooldownRemaining(Id) : -1.f, Weak.IsValid() ? Weak->GetCaster()->GetAtem() : -1.f,
					Montage ? *Montage->GetName() : TEXT("-"), bMsg ? *Msg.ToString() : TEXT("-"));
				SpellShot(Id.ToString() + TEXT("_a"));
			});
			At(T + 1.2f, [Id]() { SpellShot(Id.ToString() + TEXT("_b")); });
			T += 2.6f;
		}
		At(T + 0.5f, [bQuit]()
		{
			UE_LOG(LogTemp, Display, TEXT("[Weisen-Test] fertig"));
			if (bQuit)
			{
				FPlatformMisc::RequestExit(false);
			}
		});
	}

	FAutoConsoleCommandWithWorldAndArgs GTestSpells(
		TEXT("Ewigfels.Test.Weisen"),
		TEXT("Weisen nacheinander auf Platz 1 legen und wirken, Screenshots nach Saved/Screenshots/Weisen (ids als Argumente, quit = danach beenden)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			TArray<FName> Ids;
			for (const FString& A : Args)
			{
				if (A != TEXT("quit"))
				{
					Ids.Add(FName(*A));
				}
			}
			if (Ids.Num() == 0)
			{
				for (const TCHAR* Id : { TEXT("splitterchor"), TEXT("bassbrecher"), TEXT("wirbelschlag"), TEXT("wiegenlied"), TEXT("atemwende"),
					TEXT("windstoss"), TEXT("flutweise"), TEXT("hagelchor"), TEXT("donnerkeil"), TEXT("wirbelflut"), TEXT("felsfaust"), TEXT("glutpeitsche") })
				{
					Ids.Add(FName(Id));
				}
			}
			RunSpells(World, Ids, Args.Contains(TEXT("quit")));
		}));
}

#endif
