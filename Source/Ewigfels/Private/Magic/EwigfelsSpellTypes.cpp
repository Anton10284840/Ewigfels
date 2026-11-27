// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsSpellTypes.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"

namespace
{
	struct FSpellParseContext
	{
		FString Source;
		TArray<FString>& Errors;

		void Error(const FString& Where, const FString& Message) const
		{
			Errors.Add(FString::Printf(TEXT("%s: %s: %s"), *Source, *Where, *Message));
		}
	};

	/** "Text" oder {"de": "...", "en": "..."}; Rückfall Deutsch (gleiche Regel wie Quests). */
	FText ReadSpellLocText(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
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

	FName ReadSpellName(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		FString S;
		return Obj->TryGetStringField(Field, S) && !S.IsEmpty() ? FName(*S) : NAME_None;
	}

	float ReadFloat(const TSharedPtr<FJsonObject>& Obj, const FString& Field, float Default)
	{
		double V = 0.0;
		return Obj->TryGetNumberField(Field, V) ? static_cast<float>(V) : Default;
	}

	int32 ReadInt(const TSharedPtr<FJsonObject>& Obj, const FString& Field, int32 Default)
	{
		int32 V = 0;
		return Obj->TryGetNumberField(Field, V) ? V : Default;
	}

	bool ReadBool(const TSharedPtr<FJsonObject>& Obj, const FString& Field, bool Default)
	{
		bool V = false;
		return Obj->TryGetBoolField(Field, V) ? V : Default;
	}

	FSoftObjectPath ReadPath(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		FString S;
		return Obj->TryGetStringField(Field, S) && !S.IsEmpty() ? FSoftObjectPath(S) : FSoftObjectPath();
	}

	TArray<FName> ReadNameArray(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		TArray<FString> Strings;
		Obj->TryGetStringArrayField(Field, Strings);
		TArray<FName> Out;
		for (const FString& S : Strings)
		{
			Out.Add(FName(*S));
		}
		return Out;
	}

	const TArray<TSharedPtr<FJsonValue>>& ReadSpellArray(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		static const TArray<TSharedPtr<FJsonValue>> Empty;
		const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
		return Obj->TryGetArrayField(Field, Arr) && Arr ? *Arr : Empty;
	}

	TSharedPtr<FJsonObject> ReadObject(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
	{
		const TSharedPtr<FJsonObject>* Out = nullptr;
		return Obj->TryGetObjectField(Field, Out) && Out ? *Out : nullptr;
	}

	FString ValueToString(const TSharedPtr<FJsonValue>& Value)
	{
		FString S;
		if (Value->TryGetString(S))
		{
			return S;
		}
		FString Out;
		const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Out);
		FJsonSerializer::Serialize(Value, FString(), Writer); // schließt den Writer selbst
		return Out;
	}

	bool ParseDeliveryType(const FString& S, EEwigfelsSpellDelivery& Out)
	{
		if (S == TEXT("projectile")) { Out = EEwigfelsSpellDelivery::Projectile; return true; }
		if (S == TEXT("cone"))       { Out = EEwigfelsSpellDelivery::Cone; return true; }
		if (S == TEXT("shield"))     { Out = EEwigfelsSpellDelivery::Shield; return true; }
		if (S == TEXT("area"))       { Out = EEwigfelsSpellDelivery::Area; return true; }
		if (S == TEXT("self"))       { Out = EEwigfelsSpellDelivery::Self; return true; }
		if (S == TEXT("beam"))       { Out = EEwigfelsSpellDelivery::Beam; return true; }
		return false;
	}

	bool ParseEffectType(const FString& S, EEwigfelsSpellEffectType& Out)
	{
		if (S == TEXT("damage"))           { Out = EEwigfelsSpellEffectType::Damage; return true; }
		if (S == TEXT("status"))           { Out = EEwigfelsSpellEffectType::Status; return true; }
		if (S == TEXT("knockback"))        { Out = EEwigfelsSpellEffectType::Knockback; return true; }
		if (S == TEXT("pull"))             { Out = EEwigfelsSpellEffectType::Pull; return true; }
		if (S == TEXT("restore_atem"))     { Out = EEwigfelsSpellEffectType::RestoreAtem; return true; }
		if (S == TEXT("heal"))             { Out = EEwigfelsSpellEffectType::Heal; return true; }
		if (S == TEXT("stagger_attacker")) { Out = EEwigfelsSpellEffectType::StaggerAttacker; return true; }
		if (S == TEXT("lift"))             { Out = EEwigfelsSpellEffectType::Lift; return true; }
		if (S == TEXT("slam"))             { Out = EEwigfelsSpellEffectType::Slam; return true; }
		return false;
	}

	void ReadSpellEffects(const TSharedPtr<FJsonObject>& Obj, const FString& Field, const FString& Where,
		const FSpellParseContext& Ctx, TArray<FEwigfelsSpellEffect>& Out)
	{
		const TArray<TSharedPtr<FJsonValue>>& Arr = ReadSpellArray(Obj, Field);
		for (int32 i = 0; i < Arr.Num(); ++i)
		{
			const FString At = FString::Printf(TEXT("%s.%s[%d]"), *Where, *Field, i);
			const TSharedPtr<FJsonObject>* EffObj = nullptr;
			if (!Arr[i]->TryGetObject(EffObj) || !EffObj)
			{
				Ctx.Error(At, TEXT("muss ein Objekt sein"));
				continue;
			}
			const TSharedPtr<FJsonObject>& E = *EffObj;
			FEwigfelsSpellEffect Effect;
			FString TypeStr;
			if (!E->TryGetStringField(TEXT("type"), TypeStr) || !ParseEffectType(TypeStr, Effect.Type))
			{
				Ctx.Error(At, FString::Printf(TEXT("unbekannter type '%s'"), *TypeStr));
				continue;
			}
			Effect.Amount = ReadFloat(E, TEXT("amount"), 0.f);
			Effect.Element = ReadSpellName(E, TEXT("element"));
			Effect.Status = ReadSpellName(E, TEXT("status"));
			Effect.Duration = ReadFloat(E, TEXT("duration"), 0.f);
			Effect.Up = ReadFloat(E, TEXT("up"), 0.f);
			Effect.bChainEndOnly = ReadBool(E, TEXT("chain_end_only"), false);
			if (const TSharedPtr<FJsonObject> Bonus = ReadObject(E, TEXT("bonus")))
			{
				Effect.Bonus.IfStatus = ReadSpellName(Bonus, TEXT("if_status"));
				Effect.Bonus.Multiplier = ReadFloat(Bonus, TEXT("mult"), 1.f);
				Effect.Bonus.bConsume = ReadBool(Bonus, TEXT("consume"), false);
			}
			if (Effect.Type == EEwigfelsSpellEffectType::Status && Effect.Status.IsNone())
			{
				Ctx.Error(At, TEXT("type status braucht 'status'"));
				continue;
			}
			Out.Add(Effect);
		}
	}

	/** Felder, die ParseSpell selbst liest. Alles andere landet in FEwigfelsSpellDef::Extra. */
	const TSet<FString>& KnownSpellFields()
	{
		static const TSet<FString> Known = {
			TEXT("id"), TEXT("name"), TEXT("description"), TEXT("kind"), TEXT("catalog_id"), TEXT("school"), TEXT("chord"), TEXT("unlock"), TEXT("cost"), TEXT("cooldown"),
			TEXT("cast_time"), TEXT("recovery"), TEXT("hands"), TEXT("delivery"), TEXT("effects"), TEXT("on_parry"),
			TEXT("counter_spell"), TEXT("vfx"), TEXT("anim"), TEXT("sfx"), TEXT("incantation"), TEXT("recast"), TEXT("on_land"),
			TEXT("land_radius")
		};
		return Known;
	}

	bool ParseSpell(const TSharedPtr<FJsonObject>& S, const FString& Where, const FSpellParseContext& Ctx, FEwigfelsSpellDef& Out)
	{
		const int32 ErrorsBefore = Ctx.Errors.Num();
		Out = FEwigfelsSpellDef();
		Out.Id = ReadSpellName(S, TEXT("id"));
		if (Out.Id.IsNone())
		{
			Ctx.Error(Where, TEXT("id fehlt"));
			return false;
		}
		Out.Name = ReadSpellLocText(S, TEXT("name"));
		if (Out.Name.IsEmpty())
		{
			Out.Name = FText::FromName(Out.Id);
		}
		Out.Description = ReadSpellLocText(S, TEXT("description"));
		Out.Incantation = ReadSpellLocText(S, TEXT("incantation"));
		Out.Kind = ReadSpellName(S, TEXT("kind"));
		if (Out.Kind.IsNone())
		{
			Out.Kind = TEXT("kern");
		}
		Out.CatalogId = ReadSpellName(S, TEXT("catalog_id"));
		Out.School = ReadSpellName(S, TEXT("school"));
		Out.Chord = ReadNameArray(S, TEXT("chord"));
		Out.Unlock = ReadSpellName(S, TEXT("unlock"));
		Out.Cost = FMath::Max(0.f, ReadFloat(S, TEXT("cost"), 0.f));
		Out.Cooldown = FMath::Max(0.f, ReadFloat(S, TEXT("cooldown"), 0.f));
		Out.CastTime = FMath::Max(0.f, ReadFloat(S, TEXT("cast_time"), 0.f));
		Out.Recovery = FMath::Max(0.f, ReadFloat(S, TEXT("recovery"), 0.2f));
		FString Hands;
		Out.bTwoHanded = S->TryGetStringField(TEXT("hands"), Hands) && Hands == TEXT("two");
		Out.CounterSpell = ReadSpellName(S, TEXT("counter_spell"));

		const TSharedPtr<FJsonObject> D = ReadObject(S, TEXT("delivery"));
		if (!D)
		{
			Ctx.Error(Where, TEXT("delivery fehlt"));
			return false;
		}
		FString DeliveryType;
		if (!D->TryGetStringField(TEXT("type"), DeliveryType) || !ParseDeliveryType(DeliveryType, Out.Delivery.Type))
		{
			Ctx.Error(Where + TEXT(".delivery"), FString::Printf(TEXT("unbekannter type '%s'"), *DeliveryType));
			return false;
		}
		FEwigfelsSpellDeliveryDef& Del = Out.Delivery;
		Del.Speed = ReadFloat(D, TEXT("speed"), Del.Speed);
		Del.Radius = ReadFloat(D, TEXT("radius"), Del.Radius);
		Del.Range = ReadFloat(D, TEXT("range"), Del.Range);
		Del.Angle = FMath::Clamp(ReadFloat(D, TEXT("angle"), Del.Angle), 1.f, 360.f);
		Del.Homing = FMath::Clamp(ReadFloat(D, TEXT("homing"), Del.Homing), 0.f, 1.f);
		Del.Gravity = ReadFloat(D, TEXT("gravity"), Del.Gravity);
		Del.Chain = FMath::Max(1, ReadInt(D, TEXT("chain"), Del.Chain));
		Del.ChainWindow = ReadFloat(D, TEXT("chain_window"), Del.ChainWindow);
		Del.Pierce = FMath::Max(0, ReadInt(D, TEXT("pierce"), Del.Pierce));
		Del.ExplodeRadius = FMath::Max(0.f, ReadFloat(D, TEXT("explode_radius"), Del.ExplodeRadius));
		Del.ParryWindow = FMath::Max(0.f, ReadFloat(D, TEXT("parry_window"), Del.ParryWindow));
		Del.BlockAtemPerDamage = FMath::Max(0.f, ReadFloat(D, TEXT("block_atem_per_damage"), Del.BlockAtemPerDamage));
		Del.BlockDamageMultiplier = FMath::Clamp(ReadFloat(D, TEXT("block_damage_mult"), Del.BlockDamageMultiplier), 0.f, 1.f);
		Del.Bounces = FMath::Clamp(ReadInt(D, TEXT("bounces"), Del.Bounces), 0, 8);
		Del.BounceRange = FMath::Max(0.f, ReadFloat(D, TEXT("bounce_range"), Del.BounceRange));

		ReadSpellEffects(S, TEXT("effects"), Where, Ctx, Out.Effects);
		ReadSpellEffects(S, TEXT("on_parry"), Where, Ctx, Out.OnParry);
		ReadSpellEffects(S, TEXT("on_land"), Where, Ctx, Out.OnLand);
		Out.LandRadius = FMath::Max(0.f, ReadFloat(S, TEXT("land_radius"), 0.f));
		if (const TSharedPtr<FJsonObject> R = ReadObject(S, TEXT("recast")))
		{
			Out.RecastWindow = FMath::Max(0.f, ReadFloat(R, TEXT("window"), 0.f));
			ReadSpellEffects(R, TEXT("effects"), Where + TEXT(".recast"), Ctx, Out.RecastEffects);
		}

		// Farben: Standard aus dem Akkord, vfx.color / glow_color / spark_color überschreiben einzeln
		EwigfelsSpells::ChordPalette(Out.Chord, Out.Visual.Color, Out.Visual.GlowColor, Out.Visual.SparkColor);
		bool bGlowSet = false;
		bool bSparkSet = false;
		if (const TSharedPtr<FJsonObject> V = ReadObject(S, TEXT("vfx")))
		{
			auto ReadColor = [&](const TCHAR* FieldName, FLinearColor& Target) -> bool
			{
				FString Hex;
				if (!V->TryGetStringField(FieldName, Hex))
				{
					return false;
				}
				if (!EwigfelsSpells::ParseHexColor(Hex, Target))
				{
					Ctx.Error(Where + TEXT(".vfx.") + FieldName, FString::Printf(TEXT("'%s' ist kein #RRGGBB"), *Hex));
					return false;
				}
				return true;
			};
			ReadColor(TEXT("color"), Out.Visual.Color);
			bGlowSet = ReadColor(TEXT("glow_color"), Out.Visual.GlowColor);
			bSparkSet = ReadColor(TEXT("spark_color"), Out.Visual.SparkColor);
			const FName BeamStyle = ReadSpellName(V, TEXT("beam_style"));
			if (!BeamStyle.IsNone())
			{
				Out.Visual.BeamStyle = BeamStyle;
			}
			Out.Visual.Scale = ReadFloat(V, TEXT("scale"), 1.f);
			const FName Shape = ReadSpellName(V, TEXT("shape"));
			if (!Shape.IsNone())
			{
				Out.Visual.Shape = Shape;
			}
			Out.Visual.Niagara = ReadPath(V, TEXT("niagara"));
			Out.Visual.ImpactNiagara = ReadPath(V, TEXT("impact_niagara"));
		}
		if (Out.Chord.Num() == 0)
		{
			// Ohne Akkord (Gegnerweisen): alles in der Grundfarbe
			if (!bGlowSet) { Out.Visual.GlowColor = Out.Visual.Color; }
			if (!bSparkSet) { Out.Visual.SparkColor = Out.Visual.Color; }
		}
		if (const TSharedPtr<FJsonObject> A = ReadObject(S, TEXT("anim")))
		{
			Out.Visual.Montage = ReadPath(A, TEXT("montage"));
			Out.Visual.Gesture = ReadSpellName(A, TEXT("gesture"));
		}
		if (const TSharedPtr<FJsonObject> Sfx = ReadObject(S, TEXT("sfx")))
		{
			Out.Visual.CastSound = ReadPath(Sfx, TEXT("cast"));
			Out.Visual.ImpactSound = ReadPath(Sfx, TEXT("impact"));
		}

		for (const TPair<FString, TSharedPtr<FJsonValue>>& Field : S->Values)
		{
			if (!KnownSpellFields().Contains(Field.Key) && !Field.Key.StartsWith(TEXT("_")))
			{
				Out.Extra.Add(FName(*Field.Key), ValueToString(Field.Value));
			}
		}
		return Ctx.Errors.Num() == ErrorsBefore;
	}

	bool ParseStatus(const TSharedPtr<FJsonObject>& S, const FString& Where, const FSpellParseContext& Ctx, FEwigfelsStatusDef& Out)
	{
		Out = FEwigfelsStatusDef();
		Out.Id = ReadSpellName(S, TEXT("id"));
		if (Out.Id.IsNone())
		{
			Ctx.Error(Where, TEXT("id fehlt"));
			return false;
		}
		Out.Name = ReadSpellLocText(S, TEXT("name"));
		Out.Duration = FMath::Max(0.05f, ReadFloat(S, TEXT("duration"), Out.Duration));
		Out.DamagePerSecond = ReadFloat(S, TEXT("damage_per_second"), 0.f);
		Out.MoveSpeedMultiplier = FMath::Max(0.f, ReadFloat(S, TEXT("move_speed_mult"), 1.f));
		Out.ActionSpeedMultiplier = FMath::Max(0.05f, ReadFloat(S, TEXT("action_speed_mult"), 1.f));
		Out.AtemRegenMultiplier = FMath::Max(0.f, ReadFloat(S, TEXT("atem_regen_mult"), 1.f));
		Out.bStopsActions = ReadBool(S, TEXT("stops_actions"), false);
		Out.Cancels = ReadNameArray(S, TEXT("cancels"));
		Out.EscalateAt = FMath::Max(0, ReadInt(S, TEXT("escalate_at"), 0));
		Out.EscalateTo = ReadSpellName(S, TEXT("escalate_to"));
		Out.Look = ReadSpellName(S, TEXT("look"));
		FString Hex;
		if (S->TryGetStringField(TEXT("color"), Hex) && !EwigfelsSpells::ParseHexColor(Hex, Out.Color))
		{
			Ctx.Error(Where + TEXT(".color"), FString::Printf(TEXT("'%s' ist kein #RRGGBB"), *Hex));
		}
		return true;
	}
}

bool EwigfelsSpells::ParseHexColor(const FString& Hex, FLinearColor& Out)
{
	FString H = Hex.TrimStartAndEnd();
	H.RemoveFromStart(TEXT("#"));
	if (H.Len() != 6)
	{
		return false;
	}
	for (const TCHAR C : H)
	{
		if (!FChar::IsHexDigit(C))
		{
			return false;
		}
	}
	// FColor::FromHex liest sRGB; FLinearColor wandelt korrekt nach linear.
	Out = FLinearColor(FColor::FromHex(H));
	return true;
}

FLinearColor EwigfelsSpells::ChordNoteColor(FName Note)
{
	static const TMap<FName, FColor> Colors = {
		{ TEXT("Glut"),   FColor(255, 154, 60) },
		{ TEXT("Firn"),   FColor(143, 212, 255) },
		{ TEXT("Strom"),  FColor(255, 240, 160) },
		{ TEXT("Quell"),  FColor(60, 224, 208) },
		{ TEXT("Grund"),  FColor(204, 150, 80) },
		{ TEXT("Wachs"),  FColor(120, 190, 90) },
		{ TEXT("Hall"),   FColor(232, 238, 255) },
		{ TEXT("Urton"),  FColor(200, 170, 255) },
	};
	const FColor* Found = Colors.Find(Note);
	return Found ? FLinearColor(*Found) : FLinearColor::White;
}

void EwigfelsSpells::ChordPalette(const TArray<FName>& Chord, FLinearColor& OutCore, FLinearColor& OutGlow, FLinearColor& OutSpark)
{
	if (Chord.Num() == 0)
	{
		return; // Standardfarben behalten
	}
	OutCore = ChordNoteColor(Chord[0]);
	OutGlow = Chord.Num() > 1 ? ChordNoteColor(Chord[1]) : OutCore;
	OutSpark = Chord.Num() > 2 ? ChordNoteColor(Chord[2]) : OutGlow;
}

FSoftObjectPath EwigfelsSpells::GestureMontagePath(FName Gesture)
{
	if (Gesture.IsNone())
	{
		return FSoftObjectPath();
	}
	const FString Name = FString::Printf(TEXT("AM_Geste_%s"), *Gesture.ToString());
	return FSoftObjectPath(FString::Printf(TEXT("/Game/Animations/Magic/Gesten/%s.%s"), *Name, *Name));
}

bool EwigfelsSpells::ParseSpellFile(const FString& JsonText, const FString& SourceName,
	TArray<FEwigfelsSpellDef>& OutSpells, TArray<FEwigfelsStatusDef>& OutStatuses, TArray<FName>& OutLoadout,
	TArray<FString>& OutErrors)
{
	const FSpellParseContext Ctx{ SourceName, OutErrors };
	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		Ctx.Error(TEXT("(Datei)"), FString::Printf(TEXT("kein gueltiges JSON: %s"), *Reader->GetErrorMessage()));
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>& Spells = ReadSpellArray(Root, TEXT("spells"));
	for (int32 i = 0; i < Spells.Num(); ++i)
	{
		const FString Where = FString::Printf(TEXT("spells[%d]"), i);
		const TSharedPtr<FJsonObject>* Obj = nullptr;
		if (!Spells[i]->TryGetObject(Obj) || !Obj)
		{
			Ctx.Error(Where, TEXT("muss ein Objekt sein"));
			continue;
		}
		FEwigfelsSpellDef Def;
		if (ParseSpell(*Obj, Where, Ctx, Def))
		{
			OutSpells.Add(MoveTemp(Def));
		}
	}

	const TArray<TSharedPtr<FJsonValue>>& Statuses = ReadSpellArray(Root, TEXT("statuses"));
	for (int32 i = 0; i < Statuses.Num(); ++i)
	{
		const FString Where = FString::Printf(TEXT("statuses[%d]"), i);
		const TSharedPtr<FJsonObject>* Obj = nullptr;
		if (!Statuses[i]->TryGetObject(Obj) || !Obj)
		{
			Ctx.Error(Where, TEXT("muss ein Objekt sein"));
			continue;
		}
		FEwigfelsStatusDef Def;
		if (ParseStatus(*Obj, Where, Ctx, Def))
		{
			OutStatuses.Add(MoveTemp(Def));
		}
	}

	OutLoadout.Append(ReadNameArray(Root, TEXT("loadout")));
	return true;
}
