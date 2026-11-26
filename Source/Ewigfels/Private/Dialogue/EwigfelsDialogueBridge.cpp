// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Dialogue/EwigfelsDialogueBridge.h"
#include "WorldState/EwigfelsWorldStateSubsystem.h"
#include "Quest/EwigfelsQuestSubsystem.h"
#include "Ewigfels.h"
#include "SUDSDialogue.h"
#include "SUDSLibrary.h"
#include "SUDSScript.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

USUDSDialogue* UEwigfelsDialogueBridge::StartDialogue(UObject* Owner, USUDSScript* Script, const TArray<UObject*>& ExtraParticipants, FName StartLabel)
{
	if (!Owner || !Script)
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Dialogue: StartDialogue ohne Owner oder Skript"));
		return nullptr;
	}
	const UWorld* World = Owner->GetWorld();
	UEwigfelsDialogueBridge* Bridge = NewObject<UEwigfelsDialogueBridge>(Owner);
	Bridge->Init(World ? World->GetGameInstance() : nullptr);

	TArray<UObject*> Participants = ExtraParticipants;
	Participants.Insert(Bridge, 0);
	return USUDSLibrary::CreateDialogueWithParticipants(Owner, Script, Participants, true, StartLabel);
}

void UEwigfelsDialogueBridge::Init(UGameInstance* GameInstance)
{
	if (GameInstance)
	{
		WorldState = GameInstance->GetSubsystem<UEwigfelsWorldStateSubsystem>();
		Quests = GameInstance->GetSubsystem<UEwigfelsQuestSubsystem>();
	}
}

bool UEwigfelsDialogueBridge::ParseVariableName(FName VariableName, bool& bOutIsQuest, EEwigfelsStateDomain& OutDomain, FName& OutKey)
{
	const FString Name = VariableName.ToString();
	FString Prefix;
	FString Key;
	if (!Name.Split(TEXT("."), &Prefix, &Key) || Key.IsEmpty())
	{
		return false;
	}
	bOutIsQuest = false;
	if (Prefix == TEXT("flag"))       { OutDomain = EEwigfelsStateDomain::Flag; }
	else if (Prefix == TEXT("rel"))   { OutDomain = EEwigfelsStateDomain::Relationship; }
	else if (Prefix == TEXT("rep"))   { OutDomain = EEwigfelsStateDomain::Reputation; }
	else if (Prefix == TEXT("quest")) { OutDomain = EEwigfelsStateDomain::Flag; bOutIsQuest = true; }
	else
	{
		return false; // z. B. global.* oder lokale Dialog-Variablen
	}
	OutKey = bOutIsQuest ? EwigfelsQuest::StatusFlag(FName(*Key)) : FName(*Key);
	return true;
}

FString UEwigfelsDialogueBridge::ValueToString(const FSUDSValue& Value)
{
	switch (Value.GetType())
	{
	case ESUDSValueType::Name:    return Value.GetNameValue().ToString();
	case ESUDSValueType::Text:    return Value.GetTextValue().ToString();
	case ESUDSValueType::Int:     return FString::FromInt(Value.GetIntValue());
	case ESUDSValueType::Float:   return FString::SanitizeFloat(Value.GetFloatValue());
	case ESUDSValueType::Boolean: return Value.GetBooleanValue() ? TEXT("true") : TEXT("false");
	default:                      return FString();
	}
}

void UEwigfelsDialogueBridge::OnDialogueStarting_Implementation(USUDSDialogue* Dialogue, FName AtLabel)
{
}

void UEwigfelsDialogueBridge::OnDialogueFinished_Implementation(USUDSDialogue* Dialogue)
{
	if (UEwigfelsQuestSubsystem* QuestSys = Quests.Get())
	{
		if (const USUDSScript* Script = Dialogue ? Dialogue->GetScript() : nullptr)
		{
			QuestSys->ReportEvent(FName(TEXT("dialogue")), Script->GetName());
		}
	}
}

void UEwigfelsDialogueBridge::OnDialogueVariableRequested_Implementation(USUDSDialogue* Dialogue, FName VariableName)
{
	bool bQuest = false;
	EEwigfelsStateDomain Domain = EEwigfelsStateDomain::Flag;
	FName Key;
	UEwigfelsWorldStateSubsystem* State = WorldState.Get();
	if (!Dialogue || !State || !ParseVariableName(VariableName, bQuest, Domain, Key))
	{
		return;
	}
	TGuardValue<bool> Guard(bPushingToDialogue, true);
	Dialogue->SetVariableInt(VariableName, State->GetValue(Domain, Key));
}

void UEwigfelsDialogueBridge::OnDialogueVariableChanged_Implementation(USUDSDialogue* Dialogue, FName VariableName, const FSUDSValue& Value, bool bFromScript)
{
	bool bQuest = false;
	EEwigfelsStateDomain Domain = EEwigfelsStateDomain::Flag;
	FName Key;
	UEwigfelsWorldStateSubsystem* State = WorldState.Get();
	if (bPushingToDialogue || !bFromScript || !State || !ParseVariableName(VariableName, bQuest, Domain, Key))
	{
		return;
	}
	if (bQuest)
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Dialogue: '%s' ist nur lesbar. Quests ueber [event quest.*] steuern."), *VariableName.ToString());
		return;
	}
	int32 NewValue = 0;
	switch (Value.GetType())
	{
	case ESUDSValueType::Boolean: NewValue = Value.GetBooleanValue() ? 1 : 0; break;
	case ESUDSValueType::Int:     NewValue = Value.GetIntValue(); break;
	case ESUDSValueType::Float:   NewValue = FMath::RoundToInt(Value.GetFloatValue()); break;
	default:
		UE_LOG(LogEwigfels, Warning, TEXT("Dialogue: '%s' braucht eine Zahl oder true/false"), *VariableName.ToString());
		return;
	}
	State->SetValue(Domain, Key, NewValue);
}

void UEwigfelsDialogueBridge::OnDialogueEvent_Implementation(USUDSDialogue* Dialogue, FName EventName, const TArray<FSUDSValue>& Arguments)
{
	TArray<FString> Args;
	for (const FSUDSValue& Arg : Arguments)
	{
		Args.Add(ValueToString(Arg));
	}
	auto Arg = [&Args](int32 Index) { return Args.IsValidIndex(Index) ? Args[Index] : FString(); };

	UEwigfelsWorldStateSubsystem* State = WorldState.Get();
	UEwigfelsQuestSubsystem* QuestSys = Quests.Get();
	const FString Event = EventName.ToString();

	if (Event == TEXT("state"))
	{
		for (const FString& EffectText : Args)
		{
			if (State)
			{
				State->ApplyEffectText(EffectText);
			}
		}
	}
	else if (Event == TEXT("quest.start") && QuestSys)
	{
		QuestSys->StartQuest(FName(*Arg(0)));
	}
	else if (Event == TEXT("quest.report") && QuestSys)
	{
		const int32 Amount = Args.Num() >= 3 ? FCString::Atoi(*Arg(2)) : 1;
		QuestSys->ReportEvent(FName(*Arg(0)), Arg(1), Amount);
	}
	else if (Event == TEXT("quest.objective") && QuestSys)
	{
		QuestSys->CompleteObjective(FName(*Arg(0)), FName(*Arg(1)));
	}
	else if (Event == TEXT("quest.stage") && QuestSys)
	{
		QuestSys->SetStage(FName(*Arg(0)), FName(*Arg(1)));
	}
	else if (Event == TEXT("quest.fail") && QuestSys)
	{
		QuestSys->FailQuest(FName(*Arg(0)));
	}
	else
	{
		OnCue.Broadcast(Dialogue, EventName, Args); // emotion, camera, eigene Events
	}
}
