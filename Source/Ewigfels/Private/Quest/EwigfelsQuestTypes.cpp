// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Quest/EwigfelsQuestTypes.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"

FName EwigfelsQuest::StatusFlag(FName QuestId)
{
	return FName(*(FString(TEXT("Quest_")) + QuestId.ToString()));
}

namespace
{
	/** Sammelt Fehler mit Dateiname und Pfad im JSON, z. B. "M101.json: stages[0].objectives[1]: ...". */
	struct FParseContext
	{
		FString Source;
		TArray<FString>& Errors;

		void Error(const FString& Where, const FString& Message) const
		{
			Errors.Add(FString::Printf(TEXT("%s: %s: %s"), *Source, *Where, *Message));
		}
	};

	/** Text in der aktuellen Sprache. Erlaubt: "Text" oder {"de": "...", "en": "..."}; Rückfall Deutsch. */
	FText ReadLocText(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		const TSharedPtr<FJsonValue> Value = Obj->TryGetField(Field);
		if (!Value.IsValid())
		{
			return FText::GetEmpty();
		}
		FString Plain;
		if (Value->TryGetString(Plain))
		{
			return FText::FromString(Plain);
		}
		const TSharedPtr<FJsonObject>* LocObj = nullptr;
		if (Value->TryGetObject(LocObj) && LocObj)
		{
			const FString Lang = FInternationalization::Get().GetCurrentLanguage()->GetTwoLetterISOLanguageName();
			FString Out;
			if ((*LocObj)->TryGetStringField(Lang, Out) || (*LocObj)->TryGetStringField(TEXT("de"), Out))
			{
				return FText::FromString(Out);
			}
		}
		return FText::GetEmpty();
	}

	FName ReadName(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		FString S;
		return Obj->TryGetStringField(Field, S) ? FName(*S) : NAME_None;
	}

	TArray<FString> ReadStringArray(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		TArray<FString> Out;
		Obj->TryGetStringArrayField(Field, Out);
		return Out;
	}

	void ReadConditions(const TSharedPtr<FJsonObject>& Obj, const FString& Field, const FString& Where,
		const FParseContext& Ctx, TArray<FEwigfelsCondition>& Out)
	{
		for (const FString& Text : ReadStringArray(Obj, Field))
		{
			FEwigfelsCondition Condition;
			FString Error;
			if (EwigfelsWorldState::ParseCondition(Text, Condition, Error))
			{
				Out.Add(Condition);
			}
			else
			{
				Ctx.Error(Where + TEXT(".") + Field, Error);
			}
		}
	}

	void ReadEffects(const TSharedPtr<FJsonObject>& Obj, const FString& Field, const FString& Where,
		const FParseContext& Ctx, TArray<FEwigfelsEffect>& Out)
	{
		for (const FString& Text : ReadStringArray(Obj, Field))
		{
			FEwigfelsEffect Effect;
			FString Error;
			if (EwigfelsWorldState::ParseEffect(Text, Effect, Error))
			{
				Out.Add(Effect);
			}
			else
			{
				Ctx.Error(Where + TEXT(".") + Field, Error);
			}
		}
	}

	bool ParseType(const FString& S, EEwigfelsQuestType& Out)
	{
		if (S == TEXT("main"))    { Out = EEwigfelsQuestType::Main; return true; }
		if (S == TEXT("series"))  { Out = EEwigfelsQuestType::Series; return true; }
		if (S == TEXT("faction")) { Out = EEwigfelsQuestType::Faction; return true; }
		if (S == TEXT("side"))    { Out = EEwigfelsQuestType::Side; return true; }
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>& ReadArray(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		static const TArray<TSharedPtr<FJsonValue>> Empty;
		const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
		return Obj->TryGetArrayField(Field, Arr) && Arr ? *Arr : Empty;
	}
}

bool EwigfelsQuest::ParseQuestJson(const FString& JsonText, const FString& SourceName, FEwigfelsQuestDef& Out, TArray<FString>& OutErrors)
{
	const FParseContext Ctx{ SourceName, OutErrors };
	const int32 ErrorsBefore = OutErrors.Num();

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		Ctx.Error(TEXT("(Datei)"), FString::Printf(TEXT("kein gueltiges JSON: %s"), *Reader->GetErrorMessage()));
		return false;
	}

	Out = FEwigfelsQuestDef();
	Out.Id = ReadName(Root, TEXT("id"));
	if (Out.Id.IsNone())
	{
		Ctx.Error(TEXT("id"), TEXT("fehlt"));
		return false;
	}

	FString TypeText;
	if (Root->TryGetStringField(TEXT("type"), TypeText) && !ParseType(TypeText, Out.Type))
	{
		Ctx.Error(TEXT("type"), FString::Printf(TEXT("'%s' unbekannt (main, series, faction, side)"), *TypeText));
	}
	Out.Title = ReadLocText(Root, TEXT("title"));
	Out.Summary = ReadLocText(Root, TEXT("summary"));
	Out.Giver = ReadName(Root, TEXT("giver"));
	Out.Region = ReadName(Root, TEXT("region"));
	Root->TryGetNumberField(TEXT("act"), Out.Act);
	Root->TryGetNumberField(TEXT("duration_min"), Out.DurationMinutes);
	Root->TryGetBoolField(TEXT("auto_start"), Out.bAutoStart);
	ReadConditions(Root, TEXT("requires"), TEXT("quest"), Ctx, Out.Requires);

	const TArray<TSharedPtr<FJsonValue>>& Stages = ReadArray(Root, TEXT("stages"));
	for (int32 S = 0; S < Stages.Num(); ++S)
	{
		const FString Where = FString::Printf(TEXT("stages[%d]"), S);
		const TSharedPtr<FJsonObject>* StageObjPtr = nullptr;
		if (!Stages[S]->TryGetObject(StageObjPtr) || !StageObjPtr)
		{
			Ctx.Error(Where, TEXT("muss ein Objekt sein"));
			continue;
		}
		const TSharedPtr<FJsonObject>& StageObj = *StageObjPtr;

		FEwigfelsStageDef& Stage = Out.Stages.AddDefaulted_GetRef();
		Stage.Id = ReadName(StageObj, TEXT("id"));
		Stage.Text = ReadLocText(StageObj, TEXT("text"));
		if (Stage.Id.IsNone())
		{
			Ctx.Error(Where, TEXT("id fehlt"));
		}

		const TArray<TSharedPtr<FJsonValue>>& Objectives = ReadArray(StageObj, TEXT("objectives"));
		for (int32 O = 0; O < Objectives.Num(); ++O)
		{
			const FString OWhere = FString::Printf(TEXT("%s.objectives[%d]"), *Where, O);
			const TSharedPtr<FJsonObject>* ObjPtr = nullptr;
			if (!Objectives[O]->TryGetObject(ObjPtr) || !ObjPtr)
			{
				Ctx.Error(OWhere, TEXT("muss ein Objekt sein"));
				continue;
			}
			FEwigfelsObjectiveDef& Objective = Stage.Objectives.AddDefaulted_GetRef();
			Objective.Id = ReadName(*ObjPtr, TEXT("id"));
			Objective.Text = ReadLocText(*ObjPtr, TEXT("text"));
			Objective.Kind = ReadName(*ObjPtr, TEXT("kind"));
			(*ObjPtr)->TryGetStringField(TEXT("target"), Objective.Target);
			(*ObjPtr)->TryGetNumberField(TEXT("count"), Objective.Count);
			Objective.Count = FMath::Max(1, Objective.Count);
			Objective.Marker = ReadName(*ObjPtr, TEXT("marker"));
			(*ObjPtr)->TryGetBoolField(TEXT("optional"), Objective.bOptional);
			if (Objective.Id.IsNone())
			{
				Ctx.Error(OWhere, TEXT("id fehlt"));
			}
			if (Objective.Kind == FName(TEXT("condition")))
			{
				FString Error;
				if (!EwigfelsWorldState::ParseCondition(Objective.Target, Objective.Condition, Error))
				{
					Ctx.Error(OWhere + TEXT(".target"), Error);
				}
			}
		}

		ReadEffects(StageObj, TEXT("on_complete"), Where, Ctx, Stage.OnComplete);

		const TArray<TSharedPtr<FJsonValue>>& Next = ReadArray(StageObj, TEXT("next"));
		for (int32 N = 0; N < Next.Num(); ++N)
		{
			const FString NWhere = FString::Printf(TEXT("%s.next[%d]"), *Where, N);
			const TSharedPtr<FJsonObject>* NPtr = nullptr;
			if (!Next[N]->TryGetObject(NPtr) || !NPtr)
			{
				Ctx.Error(NWhere, TEXT("muss ein Objekt sein"));
				continue;
			}
			FEwigfelsStageTransition& T = Stage.Next.AddDefaulted_GetRef();
			ReadConditions(*NPtr, TEXT("if"), NWhere, Ctx, T.Conditions);
			T.NextStage = ReadName(*NPtr, TEXT("stage"));
			FString End;
			if ((*NPtr)->TryGetStringField(TEXT("end"), End))
			{
				if (End == TEXT("completed"))   { T.EndStatus = EEwigfelsQuestStatus::Completed; }
				else if (End == TEXT("failed")) { T.EndStatus = EEwigfelsQuestStatus::Failed; }
				else { Ctx.Error(NWhere + TEXT(".end"), TEXT("nur 'completed' oder 'failed'")); }
			}
			if (T.NextStage.IsNone() == (T.EndStatus == EEwigfelsQuestStatus::NotStarted))
			{
				Ctx.Error(NWhere, TEXT("braucht genau eins von 'stage' oder 'end'"));
			}
		}
		// Ohne "next" endet die Quest nach dieser Etappe erfolgreich.
		if (Stage.Next.Num() == 0)
		{
			Stage.Next.AddDefaulted_GetRef().EndStatus = EEwigfelsQuestStatus::Completed;
		}
	}

	if (Out.Stages.Num() == 0)
	{
		Ctx.Error(TEXT("stages"), TEXT("mindestens eine Etappe noetig"));
	}
	else
	{
		Out.StartStage = ReadName(Root, TEXT("start_stage"));
		if (Out.StartStage.IsNone())
		{
			Out.StartStage = Out.Stages[0].Id;
		}
		if (!Out.FindStage(Out.StartStage))
		{
			Ctx.Error(TEXT("start_stage"), FString::Printf(TEXT("Etappe '%s' gibt es nicht"), *Out.StartStage.ToString()));
		}
		for (const FEwigfelsStageDef& Stage : Out.Stages)
		{
			for (const FEwigfelsStageTransition& T : Stage.Next)
			{
				if (!T.NextStage.IsNone() && !Out.FindStage(T.NextStage))
				{
					Ctx.Error(Stage.Id.ToString(), FString::Printf(TEXT("next zeigt auf unbekannte Etappe '%s'"), *T.NextStage.ToString()));
				}
			}
		}
	}

	const TSharedPtr<FJsonObject>* RewardsPtr = nullptr;
	if (Root->TryGetObjectField(TEXT("rewards"), RewardsPtr) && RewardsPtr)
	{
		(*RewardsPtr)->TryGetNumberField(TEXT("xp"), Out.Rewards.Xp);
		(*RewardsPtr)->TryGetNumberField(TEXT("gold"), Out.Rewards.Gold);
		for (const FString& Item : ReadStringArray(*RewardsPtr, TEXT("items")))
		{
			Out.Rewards.Items.Add(FName(*Item));
		}
		ReadEffects(*RewardsPtr, TEXT("effects"), TEXT("rewards"), Ctx, Out.Rewards.Effects);
	}

	return OutErrors.Num() == ErrorsBefore;
}
