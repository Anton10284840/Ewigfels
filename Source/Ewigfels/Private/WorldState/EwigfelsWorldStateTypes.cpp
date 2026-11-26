// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "WorldState/EwigfelsWorldStateTypes.h"

namespace
{
	const TCHAR* DomainPrefix(EEwigfelsStateDomain Domain)
	{
		switch (Domain)
		{
		case EEwigfelsStateDomain::Relationship: return TEXT("rel");
		case EEwigfelsStateDomain::Reputation:   return TEXT("rep");
		default:                                 return TEXT("flag");
		}
	}

	const TCHAR* OpText(EEwigfelsCompareOp Op)
	{
		switch (Op)
		{
		case EEwigfelsCompareOp::Equal:        return TEXT("==");
		case EEwigfelsCompareOp::NotEqual:     return TEXT("!=");
		case EEwigfelsCompareOp::Less:         return TEXT("<");
		case EEwigfelsCompareOp::LessEqual:    return TEXT("<=");
		case EEwigfelsCompareOp::Greater:      return TEXT(">");
		default:                               return TEXT(">=");
		}
	}

	bool IsKeyChar(TCHAR C)
	{
		return FChar::IsAlnum(C) || C == TEXT('_');
	}

	/** Zerlegt "rel:Lio>=30" in Bereich, Schlüssel und Rest (">=30"). */
	bool SplitDomainKey(const FString& Text, EEwigfelsStateDomain& OutDomain, FName& OutKey, FString& OutRest, FString& OutError, bool* bOutQuest = nullptr)
	{
		bool bQuest = false;
		int32 Colon = INDEX_NONE;
		if (!Text.FindChar(TEXT(':'), Colon))
		{
			OutError = FString::Printf(TEXT("'%s': Bereich fehlt (flag:, rel: oder rep:)"), *Text);
			return false;
		}
		const FString Prefix = Text.Left(Colon).TrimStartAndEnd().ToLower();
		if (Prefix == TEXT("flag"))      { OutDomain = EEwigfelsStateDomain::Flag; }
		else if (Prefix == TEXT("rel"))  { OutDomain = EEwigfelsStateDomain::Relationship; }
		else if (Prefix == TEXT("rep"))  { OutDomain = EEwigfelsStateDomain::Reputation; }
		else if (Prefix == TEXT("quest")) { OutDomain = EEwigfelsStateDomain::Flag; bQuest = true; }
		else
		{
			OutError = FString::Printf(TEXT("'%s': unbekannter Bereich '%s'"), *Text, *Prefix);
			return false;
		}

		const FString Body = Text.Mid(Colon + 1).TrimStart();
		int32 End = 0;
		while (End < Body.Len() && IsKeyChar(Body[End]))
		{
			++End;
		}
		if (End == 0)
		{
			OutError = FString::Printf(TEXT("'%s': Schluessel fehlt"), *Text);
			return false;
		}
		// "quest:M101" liest das Status-Flag "Quest_M101" (siehe EwigfelsQuest::StatusFlag).
		OutKey = FName(*(bQuest ? FString(TEXT("Quest_")) + Body.Left(End) : Body.Left(End)));
		OutRest = Body.Mid(End).TrimStartAndEnd();
		if (bOutQuest)
		{
			*bOutQuest = bQuest;
		}
		return true;
	}

	bool ParseTier(const FString& In, EEwigfelsReputationTier& Out)
	{
		const FString S = In.ToLower().Replace(TEXT("\u00e4"), TEXT("ae")).Replace(TEXT("\u00fc"), TEXT("ue"));
		if (S == TEXT("feind"))        { Out = EEwigfelsReputationTier::Feind; return true; }
		if (S == TEXT("misstrauisch")) { Out = EEwigfelsReputationTier::Misstrauisch; return true; }
		if (S == TEXT("neutral"))      { Out = EEwigfelsReputationTier::Neutral; return true; }
		if (S == TEXT("geschaetzt"))   { Out = EEwigfelsReputationTier::Geschaetzt; return true; }
		if (S == TEXT("verbuendet"))   { Out = EEwigfelsReputationTier::Verbuendet; return true; }
		return false;
	}

	/** Quest-Status als Wort: notstarted 0, active 1, completed 2, failed 3. */
	bool ParseQuestStatus(const FString& In, int32& Out)
	{
		const FString S = In.ToLower();
		if (S == TEXT("notstarted")) { Out = 0; return true; }
		if (S == TEXT("active"))     { Out = 1; return true; }
		if (S == TEXT("completed"))  { Out = 2; return true; }
		if (S == TEXT("failed"))     { Out = 3; return true; }
		return false;
	}

	bool ParseInt(const FString& In, int32& Out)
	{
		const FString S = In.TrimStartAndEnd();
		if (S.IsEmpty() || !S.IsNumeric() || S.Contains(TEXT(".")))
		{
			return false;
		}
		Out = FCString::Atoi(*S);
		return true;
	}
}

// ---------------------------------------------------------------------------

FString FEwigfelsCondition::ToString() const
{
	return FString::Printf(TEXT("%s:%s%s%d"), DomainPrefix(Domain), *Key.ToString(), OpText(Op), Value);
}

FString FEwigfelsEffect::ToString() const
{
	if (Op == EEwigfelsEffectOp::Set)
	{
		return FString::Printf(TEXT("%s:%s=%d"), DomainPrefix(Domain), *Key.ToString(), Value);
	}
	return FString::Printf(TEXT("%s:%s%+d"), DomainPrefix(Domain), *Key.ToString(), Value);
}

// ---------------------------------------------------------------------------

TMap<FName, int32>& FEwigfelsWorldStateData::MapFor(EEwigfelsStateDomain Domain)
{
	switch (Domain)
	{
	case EEwigfelsStateDomain::Relationship: return Relationships;
	case EEwigfelsStateDomain::Reputation:   return Reputation;
	default:                                 return Flags;
	}
}

const TMap<FName, int32>& FEwigfelsWorldStateData::MapFor(EEwigfelsStateDomain Domain) const
{
	return const_cast<FEwigfelsWorldStateData*>(this)->MapFor(Domain);
}

int32 FEwigfelsWorldStateData::GetValue(EEwigfelsStateDomain Domain, FName Key) const
{
	const int32* Found = MapFor(Domain).Find(Key);
	return Found ? *Found : 0;
}

int32 FEwigfelsWorldStateData::SetValue(EEwigfelsStateDomain Domain, FName Key, int32 NewValue)
{
	if (Domain != EEwigfelsStateDomain::Flag)
	{
		NewValue = FMath::Clamp(NewValue, EwigfelsWorldState::MinValue, EwigfelsWorldState::MaxValue);
	}
	TMap<FName, int32>& Map = MapFor(Domain);
	const int32 Old = GetValue(Domain, Key);
	if (NewValue == 0)
	{
		Map.Remove(Key); // 0 = Standard, spart Platz im Spielstand
	}
	else
	{
		Map.Add(Key, NewValue);
	}
	return Old;
}

bool FEwigfelsWorldStateData::Check(const FEwigfelsCondition& Condition) const
{
	return EwigfelsWorldState::Compare(GetValue(Condition.Domain, Condition.Key), Condition.Op, Condition.Value);
}

bool FEwigfelsWorldStateData::CheckAll(const TArray<FEwigfelsCondition>& Conditions) const
{
	for (const FEwigfelsCondition& Condition : Conditions)
	{
		if (!Check(Condition))
		{
			return false;
		}
	}
	return true;
}

void FEwigfelsWorldStateData::Apply(const FEwigfelsEffect& Effect, int32& OutOld, int32& OutNew)
{
	const int32 Current = GetValue(Effect.Domain, Effect.Key);
	const int32 Target = Effect.Op == EEwigfelsEffectOp::Set ? Effect.Value : Current + Effect.Value;
	OutOld = SetValue(Effect.Domain, Effect.Key, Target);
	OutNew = GetValue(Effect.Domain, Effect.Key);
}

void FEwigfelsWorldStateData::Reset()
{
	Flags.Reset();
	Relationships.Reset();
	Reputation.Reset();
}

// ---------------------------------------------------------------------------

EEwigfelsReputationTier EwigfelsWorldState::TierFromValue(int32 Value)
{
	if (Value <= -60) { return EEwigfelsReputationTier::Feind; }
	if (Value <= -20) { return EEwigfelsReputationTier::Misstrauisch; }
	if (Value < 20)   { return EEwigfelsReputationTier::Neutral; }
	if (Value < 60)   { return EEwigfelsReputationTier::Geschaetzt; }
	return EEwigfelsReputationTier::Verbuendet;
}

int32 EwigfelsWorldState::TierMinValue(EEwigfelsReputationTier Tier)
{
	switch (Tier)
	{
	case EEwigfelsReputationTier::Feind:        return MinValue;
	case EEwigfelsReputationTier::Misstrauisch: return -59;
	case EEwigfelsReputationTier::Neutral:      return -19;
	case EEwigfelsReputationTier::Geschaetzt:   return 20;
	default:                                    return 60;
	}
}

bool EwigfelsWorldState::Compare(int32 Lhs, EEwigfelsCompareOp Op, int32 Rhs)
{
	switch (Op)
	{
	case EEwigfelsCompareOp::Equal:        return Lhs == Rhs;
	case EEwigfelsCompareOp::NotEqual:     return Lhs != Rhs;
	case EEwigfelsCompareOp::Less:         return Lhs < Rhs;
	case EEwigfelsCompareOp::LessEqual:    return Lhs <= Rhs;
	case EEwigfelsCompareOp::Greater:      return Lhs > Rhs;
	default:                               return Lhs >= Rhs;
	}
}

bool EwigfelsWorldState::ParseCondition(const FString& InText, FEwigfelsCondition& Out, FString& OutError)
{
	FString Text = InText.TrimStartAndEnd();
	const bool bNegated = Text.StartsWith(TEXT("!"));
	if (bNegated)
	{
		Text.RightChopInline(1);
	}

	FString Rest;
	bool bQuest = false;
	if (!SplitDomainKey(Text, Out.Domain, Out.Key, Rest, OutError, &bQuest))
	{
		return false;
	}

	if (Rest.IsEmpty() && bQuest)
	{
		// "quest:M101" = abgeschlossen, "!quest:M101" = nicht abgeschlossen
		Out.Op = bNegated ? EEwigfelsCompareOp::NotEqual : EEwigfelsCompareOp::Equal;
		Out.Value = 2;
		return true;
	}
	if (Rest.IsEmpty())
	{
		if (Out.Domain != EEwigfelsStateDomain::Flag)
		{
			OutError = FString::Printf(TEXT("'%s': Vergleich fehlt (z. B. >=30)"), *InText);
			return false;
		}
		Out.Op = bNegated ? EEwigfelsCompareOp::Equal : EEwigfelsCompareOp::NotEqual;
		Out.Value = 0;
		return true;
	}
	if (bNegated)
	{
		OutError = FString::Printf(TEXT("'%s': '!' nur ohne Vergleich erlaubt"), *InText);
		return false;
	}

	// Längere Operatoren zuerst prüfen.
	static const TPair<const TCHAR*, EEwigfelsCompareOp> Ops[] = {
		{ TEXT(">="), EEwigfelsCompareOp::GreaterEqual },
		{ TEXT("<="), EEwigfelsCompareOp::LessEqual },
		{ TEXT("=="), EEwigfelsCompareOp::Equal },
		{ TEXT("!="), EEwigfelsCompareOp::NotEqual },
		{ TEXT(">"),  EEwigfelsCompareOp::Greater },
		{ TEXT("<"),  EEwigfelsCompareOp::Less },
		{ TEXT("="),  EEwigfelsCompareOp::Equal },
	};
	FString ValueText;
	bool bFoundOp = false;
	for (const TPair<const TCHAR*, EEwigfelsCompareOp>& Pair : Ops)
	{
		if (Rest.StartsWith(Pair.Key))
		{
			Out.Op = Pair.Value;
			ValueText = Rest.Mid(FCString::Strlen(Pair.Key));
			bFoundOp = true;
			break;
		}
	}
	if (!bFoundOp)
	{
		OutError = FString::Printf(TEXT("'%s': unbekannter Vergleich '%s'"), *InText, *Rest);
		return false;
	}

	if (ParseInt(ValueText, Out.Value))
	{
		return true;
	}
	if (bQuest && ParseQuestStatus(ValueText.TrimStartAndEnd(), Out.Value))
	{
		return true;
	}
	EEwigfelsReputationTier Tier;
	if (Out.Domain == EEwigfelsStateDomain::Reputation && ParseTier(ValueText.TrimStartAndEnd(), Tier))
	{
		Out.Value = TierMinValue(Tier);
		return true;
	}
	OutError = FString::Printf(TEXT("'%s': Wert '%s' ist keine Zahl%s"), *InText, *ValueText,
		Out.Domain == EEwigfelsStateDomain::Reputation ? TEXT(" und keine Ruf-Stufe") : TEXT(""));
	return false;
}

bool EwigfelsWorldState::ParseEffect(const FString& InText, FEwigfelsEffect& Out, FString& OutError)
{
	const FString Text = InText.TrimStartAndEnd();
	FString Rest;
	bool bQuest = false;
	if (!SplitDomainKey(Text, Out.Domain, Out.Key, Rest, OutError, &bQuest))
	{
		return false;
	}
	if (bQuest)
	{
		OutError = FString::Printf(TEXT("'%s': Quest-Status setzt nur das Questsystem"), *InText);
		return false;
	}
	if (Rest.IsEmpty())
	{
		OutError = FString::Printf(TEXT("'%s': Aenderung fehlt (=1, +10 oder -5)"), *InText);
		return false;
	}

	const TCHAR OpChar = Rest[0];
	const FString ValueText = Rest.Mid(1);
	int32 Number = 0;
	if (!ParseInt(ValueText, Number))
	{
		OutError = FString::Printf(TEXT("'%s': Wert '%s' ist keine Zahl"), *InText, *ValueText);
		return false;
	}
	switch (OpChar)
	{
	case TEXT('='): Out.Op = EEwigfelsEffectOp::Set; Out.Value = Number;  return true;
	case TEXT('+'): Out.Op = EEwigfelsEffectOp::Add; Out.Value = Number;  return true;
	case TEXT('-'): Out.Op = EEwigfelsEffectOp::Add; Out.Value = -Number; return true;
	default:
		OutError = FString::Printf(TEXT("'%s': unbekannte Aenderung '%c'"), *InText, OpChar);
		return false;
	}
}
