// Copyright Ewigfels. Alle Rechte vorbehalten.

// Test-Befehl "Ewigfels.Test.VFX <id> [quit]": spielt NS_<id> und NS_<id>_Impact mehrmals vor der Figur ab (Strahlen
// 7 m nach vorn, User.BeamEnd gesetzt) in den Farben der Weise und macht Screenshots in kurzen Abständen
// (Saved/Screenshots/VFX/<id>_<n>). Unabhängig von Zielen und Wirkzeiten – zum Ansehen neuer Effekte. Nur Entwicklungs-Builds.

#include "Character/EwigfelsCharacter.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Magic/EwigfelsSpellTypes.h"
#include "Misc/Paths.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"
#include "Containers/Ticker.h"
#include "UI/EwigfelsCharacterCreator.h"
#include "UnrealClient.h"

#if !UE_BUILD_SHIPPING

namespace EwigfelsVfxTest
{
	void VfxShot(const FString& Name)
	{
		FScreenshotRequest::RequestScreenshot(FPaths::ProjectSavedDir() / TEXT("Screenshots/VFX") / (Name + TEXT(".png")), false, false);
	}

	struct FColors { FLinearColor Color = FLinearColor::White, Glow = FLinearColor::White, Spark = FLinearColor::White; };

	UNiagaraComponent* Play(UWorld* World, const FString& Path, const FVector& At, const FRotator& Rot, const FColors& C, const FVector& BeamEnd)
	{
		UNiagaraSystem* Sys = LoadObject<UNiagaraSystem>(nullptr, *Path, nullptr, LOAD_NoWarn | LOAD_Quiet);
		if (!Sys)
		{
			UE_LOG(LogTemp, Warning, TEXT("[VFX-Test] %s fehlt"), *Path);
			return nullptr;
		}
		// Erst Parameter setzen, dann starten (sonst liest der erste Spawn noch die Standardwerte)
		UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, Sys, At, Rot, FVector(1.f), true, false);
		if (Comp)
		{
			Comp->SetVariableLinearColor(TEXT("User.Color"), C.Color);
			Comp->SetVariableLinearColor(TEXT("User.GlowColor"), C.Glow);
			Comp->SetVariableLinearColor(TEXT("User.SparkColor"), C.Spark);
			Comp->SetVariableVec3(TEXT("User.BeamEnd"), BeamEnd);
			Comp->Activate(true);
		}
		return Comp;
	}

	void RunVfx(UWorld* World, FName Id, bool bQuit, int32 Tries = 0)
	{
		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		AEwigfelsCharacter* Char = PC ? Cast<AEwigfelsCharacter>(PC->GetPawn()) : nullptr;
		if (!Char || World->GetTimeSeconds() < 4.f)
		{
			if (World && Tries < 60)
			{
				FTimerHandle Retry;
				TWeakObjectPtr<UWorld> WeakWorld(World);
				World->GetTimerManager().SetTimer(Retry, FTimerDelegate::CreateLambda([WeakWorld, Id, bQuit, Tries]()
				{
					if (WeakWorld.IsValid())
					{
						RunVfx(WeakWorld.Get(), Id, bQuit, Tries + 1);
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
		// Farben kopieren (kein Zeiger in die Weisen-Tabelle über Timer hinweg)
		FColors Colors;
		bool bProjectile = false;
		bool bGround = false;  // Kegel, Fläche, Selbst, Schild: am Boden vor der Figur statt quer
		bool bCone = false;
		if (const FEwigfelsSpellDef* Spell = Char->GetCaster()->FindSpellDef(Id))
		{
			bProjectile = Spell->Delivery.Type == EEwigfelsSpellDelivery::Projectile;
			bCone = Spell->Delivery.Type == EEwigfelsSpellDelivery::Cone;
			bGround = bCone || Spell->Delivery.Type == EEwigfelsSpellDelivery::Area
				|| Spell->Delivery.Type == EEwigfelsSpellDelivery::Self || Spell->Delivery.Type == EEwigfelsSpellDelivery::Shield;
			Colors.Color = Spell->Visual.Color;
			Colors.Glow = Spell->Visual.GlowColor.A > 0.f ? Spell->Visual.GlowColor : Spell->Visual.Color;
			Colors.Spark = Spell->Visual.SparkColor.A > 0.f ? Spell->Visual.SparkColor : Spell->Visual.Color;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[VFX-Test] Weise %s unbekannt, weiße Farben"), *Id.ToString());
		}
		const FVector Forward = Char->GetActorForwardVector();
		const FVector Right = Char->GetActorRightVector();
		// Quer vor der Figur (von links nach rechts), damit die Kamera hinter ihr Strahlen in voller Länge sieht
		FVector Start = Char->GetActorLocation() + Forward * 550.f - Right * 380.f + FVector(0, 0, 60.f);
		FVector End = Start + Right * 760.f + Forward * 120.f;
		if (bGround)
		{
			// Fläche/Selbst: mittig 6 m vor der Figur am Boden. Kegel: von links quer nach rechts, damit man die Breite sieht.
			const FVector Feet = Char->GetActorLocation() - FVector(0, 0, Char->GetSimpleCollisionHalfHeight());
			Start = bCone ? Feet + Forward * 600.f - Right * 450.f : Feet + Forward * 650.f;
			End = bCone ? Start + Right * 900.f : Start + Forward * 100.f;
		}
		const FString Base = FString::Printf(TEXT("/Game/VFX/Spells/NS_%s.NS_%s"), *Id.ToString(), *Id.ToString());
		const FString Impact = FString::Printf(TEXT("/Game/VFX/Spells/NS_%s_Impact.NS_%s_Impact"), *Id.ToString(), *Id.ToString());
		TWeakObjectPtr<UWorld> WeakWorld(World);
		FTimerManager& Timers = World->GetTimerManager();
		auto At = [&Timers](float Seconds, TFunction<void()> Fn)
		{
			FTimerHandle Handle;
			Timers.SetTimer(Handle, FTimerDelegate::CreateLambda(MoveTemp(Fn)), Seconds, false);
		};
		const FString IdText = Id.ToString();
		// Drei Durchgänge, Screenshots zu verschiedenen Zeitpunkten nach dem Start
		const float BeamDelays[] = { 0.03f, 0.08f, 0.15f, 0.3f, 0.6f };
		const float ProjectileDelays[] = { 0.15f, 0.35f, 0.55f, 0.7f, 0.95f };
		const float* Delays = bProjectile ? ProjectileDelays : BeamDelays;
		for (int32 Pass = 0; Pass < 5; ++Pass)
		{
			const float T = 1.f + Pass * 1.5f;
			At(T, [WeakWorld, Base, Impact, Start, End, Forward, Colors, bProjectile]()
			{
				UWorld* W = WeakWorld.Get();
				if (!W)
				{
					return;
				}
				UNiagaraComponent* Comp = Play(W, Base, Start, (End - Start).Rotation(), Colors, End);
				if (!bProjectile)
				{
					Play(W, Impact, End, (Start - End).Rotation(), Colors, End);
					return;
				}
				// Geschoss: Effekt 0,6 s von Start nach End fliegen lassen, dann Einschlag
				const double T0 = FPlatformTime::Seconds();
				TWeakObjectPtr<UNiagaraComponent> WeakComp(Comp);
				FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([WeakComp, WeakWorld, Impact, Start, End, Colors, T0](float)
				{
					const float Alpha = FMath::Clamp(float(FPlatformTime::Seconds() - T0) / 0.6f, 0.f, 1.f);
					if (WeakComp.IsValid())
					{
						WeakComp->SetWorldLocation(FMath::Lerp(Start, End, Alpha));
					}
					if (Alpha >= 1.f)
					{
						if (WeakComp.IsValid())
						{
							WeakComp->Deactivate();
						}
						if (UWorld* W2 = WeakWorld.Get())
						{
							Play(W2, Impact, End, (Start - End).Rotation(), Colors, End);
						}
						return false;
					}
					return true;
				}));
			});
			const float Delay = Delays[Pass];
			At(T + Delay, [IdText, Pass, Delay]() { VfxShot(FString::Printf(TEXT("%s_%d_%03dms"), *IdText, Pass, FMath::RoundToInt(Delay * 1000.f))); });
		}
		At(1.f + 5 * 1.5f + 1.f, [bQuit]()
		{
			UE_LOG(LogTemp, Display, TEXT("[VFX-Test] fertig"));
			if (bQuit)
			{
				FPlatformMisc::RequestExit(false);
			}
		});
	}

	FAutoConsoleCommandWithWorldAndArgs GTestVfx(
		TEXT("Ewigfels.Test.VFX"),
		TEXT("NS_<id> und NS_<id>_Impact vor der Figur abspielen, Screenshots nach Saved/Screenshots/VFX (quit = danach beenden)"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			if (Args.Num() > 0)
			{
				RunVfx(World, FName(*Args[0]), Args.Contains(TEXT("quit")));
			}
		}));
}

#endif
