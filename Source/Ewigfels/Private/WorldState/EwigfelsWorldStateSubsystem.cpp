// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "WorldState/EwigfelsWorldStateSubsystem.h"
#include "Ewigfels.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"

void UEwigfelsWorldStateSubsystem::SetValue(EEwigfelsStateDomain Domain, FName Key, int32 Value)
{
	const int32 Old = Data.SetValue(Domain, Key, Value);
	Broadcast(Domain, Key, Old, Data.GetValue(Domain, Key));
}

void UEwigfelsWorldStateSubsystem::AddValue(EEwigfelsStateDomain Domain, FName Key, int32 Delta)
{
	SetValue(Domain, Key, Data.GetValue(Domain, Key) + Delta);
}

EEwigfelsReputationTier UEwigfelsWorldStateSubsystem::GetReputationTier(FName Faction) const
{
	return EwigfelsWorldState::TierFromValue(Data.GetValue(EEwigfelsStateDomain::Reputation, Faction));
}

void UEwigfelsWorldStateSubsystem::ApplyEffect(const FEwigfelsEffect& Effect)
{
	int32 Old = 0;
	int32 New = 0;
	Data.Apply(Effect, Old, New);
	Broadcast(Effect.Domain, Effect.Key, Old, New);
}

void UEwigfelsWorldStateSubsystem::ApplyEffects(const TArray<FEwigfelsEffect>& Effects)
{
	for (const FEwigfelsEffect& Effect : Effects)
	{
		ApplyEffect(Effect);
	}
}

bool UEwigfelsWorldStateSubsystem::CheckConditionText(const FString& ConditionText) const
{
	FEwigfelsCondition Condition;
	FString Error;
	if (!EwigfelsWorldState::ParseCondition(ConditionText, Condition, Error))
	{
		UE_LOG(LogEwigfels, Warning, TEXT("WorldState: %s"), *Error);
		return false;
	}
	return Data.Check(Condition);
}

bool UEwigfelsWorldStateSubsystem::ApplyEffectText(const FString& EffectText)
{
	FEwigfelsEffect Effect;
	FString Error;
	if (!EwigfelsWorldState::ParseEffect(EffectText, Effect, Error))
	{
		UE_LOG(LogEwigfels, Warning, TEXT("WorldState: %s"), *Error);
		return false;
	}
	ApplyEffect(Effect);
	return true;
}

void UEwigfelsWorldStateSubsystem::ReplaceData(const FEwigfelsWorldStateData& NewData)
{
	Data = NewData;
	OnStateReplaced.Broadcast();
}

FString UEwigfelsWorldStateSubsystem::DescribeState() const
{
	FString Out;
	auto AppendMap = [&Out](const TCHAR* Title, const TMap<FName, int32>& Map)
	{
		Out += FString::Printf(TEXT("%s (%d):\n"), Title, Map.Num());
		TArray<FName> Keys;
		Map.GetKeys(Keys);
		Keys.Sort(FNameLexicalLess());
		for (const FName& Key : Keys)
		{
			Out += FString::Printf(TEXT("  %s = %d\n"), *Key.ToString(), Map.FindChecked(Key));
		}
	};
	AppendMap(TEXT("Flags"), Data.Flags);
	AppendMap(TEXT("Beziehungen"), Data.Relationships);
	AppendMap(TEXT("Ruf"), Data.Reputation);
	return Out;
}

void UEwigfelsWorldStateSubsystem::Broadcast(EEwigfelsStateDomain Domain, FName Key, int32 Old, int32 New)
{
	if (Old == New)
	{
		return;
	}
	UE_LOG(LogEwigfels, Verbose, TEXT("WorldState: %s %d -> %d"), *Key.ToString(), Old, New);
	OnStateChanged.Broadcast(Domain, Key, Old, New);
}

// ---------------------------------------------------------------------------
// Konsolenbefehle zum Testen im laufenden Spiel (PIE oder Build mit Konsole)

namespace
{
	UEwigfelsWorldStateSubsystem* FindSubsystem(UWorld* World)
	{
		UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		return GameInstance ? GameInstance->GetSubsystem<UEwigfelsWorldStateSubsystem>() : nullptr;
	}

	FAutoConsoleCommandWithWorldAndArgs GDumpCommand(
		TEXT("Ewigfels.State.Dump"),
		TEXT("Zeigt alle Flags, Beziehungen und Ruf-Werte im Log."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (UEwigfelsWorldStateSubsystem* State = FindSubsystem(World))
			{
				UE_LOG(LogEwigfels, Display, TEXT("Weltstatus:\n%s"), *State->DescribeState());
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GApplyCommand(
		TEXT("Ewigfels.State.Apply"),
		TEXT("Aendert einen Wert, z. B. Ewigfels.State.Apply rel:Lio+10"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			UEwigfelsWorldStateSubsystem* State = FindSubsystem(World);
			if (State && Args.Num() > 0)
			{
				State->ApplyEffectText(FString::Join(Args, TEXT("")));
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GCheckCommand(
		TEXT("Ewigfels.State.Check"),
		TEXT("Prueft eine Bedingung, z. B. Ewigfels.State.Check rep:Kollegium>=Geschaetzt"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			UEwigfelsWorldStateSubsystem* State = FindSubsystem(World);
			if (State && Args.Num() > 0)
			{
				const FString Text = FString::Join(Args, TEXT(""));
				UE_LOG(LogEwigfels, Display, TEXT("%s -> %s"), *Text, State->CheckConditionText(Text) ? TEXT("wahr") : TEXT("falsch"));
			}
		}));
}
