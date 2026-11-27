// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "AI/EwigfelsCreatureTypes.h"
#include "Magic/EwigfelsSpellTypes.h"
#include "Ewigfels.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	FText ReadCreatureLocText(const TSharedPtr<FJsonObject>& Obj, const FString& Field)
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

	float CreatureNum(const TSharedPtr<FJsonObject>& Obj, const FString& Field, float Default)
	{
		double V = 0.0;
		return Obj->TryGetNumberField(Field, V) ? static_cast<float>(V) : Default;
	}

	void CreatureColor(const TSharedPtr<FJsonObject>& Obj, const FString& Field, FLinearColor& Out, const FString& Where, TArray<FString>& Errors)
	{
		FString Hex;
		if (Obj->TryGetStringField(Field, Hex) && !EwigfelsSpells::ParseHexColor(Hex, Out))
		{
			Errors.Add(FString::Printf(TEXT("%s.%s: '%s' ist kein #RRGGBB"), *Where, *Field, *Hex));
		}
	}

	FString CreatureDir()
	{
		return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), TEXT("Creatures"));
	}
}

bool EwigfelsCreatures::ParseCreatureFile(const FString& JsonText, const FString& SourceName, TArray<FEwigfelsCreatureDef>& Out, TArray<FString>& OutErrors)
{
	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		OutErrors.Add(FString::Printf(TEXT("%s: kein gueltiges JSON: %s"), *SourceName, *Reader->GetErrorMessage()));
		return false;
	}
	const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
	if (!Root->TryGetArrayField(TEXT("creatures"), Arr) || !Arr)
	{
		return true;
	}
	for (int32 i = 0; i < Arr->Num(); ++i)
	{
		const FString Where = FString::Printf(TEXT("%s: creatures[%d]"), *SourceName, i);
		const TSharedPtr<FJsonObject>* ObjPtr = nullptr;
		if (!(*Arr)[i]->TryGetObject(ObjPtr) || !ObjPtr)
		{
			OutErrors.Add(Where + TEXT(": muss ein Objekt sein"));
			continue;
		}
		const TSharedPtr<FJsonObject>& C = *ObjPtr;
		FEwigfelsCreatureDef Def;
		FString S;
		if (!C->TryGetStringField(TEXT("id"), S) || S.IsEmpty())
		{
			OutErrors.Add(Where + TEXT(": id fehlt"));
			continue;
		}
		Def.Id = FName(*S);
		Def.Name = ReadCreatureLocText(C, TEXT("name"));
		if (Def.Name.IsEmpty())
		{
			Def.Name = FText::FromName(Def.Id);
		}
		if (C->TryGetStringField(TEXT("faction"), S)) { Def.Faction = FName(*S); }
		if (C->TryGetStringField(TEXT("behavior"), S))
		{
			if (S == TEXT("melee"))       { Def.Behavior = EEwigfelsCreatureBehavior::Melee; }
			else if (S == TEXT("ranged")) { Def.Behavior = EEwigfelsCreatureBehavior::Ranged; }
			else if (S == TEXT("dummy"))  { Def.Behavior = EEwigfelsCreatureBehavior::Dummy; }
			else { OutErrors.Add(FString::Printf(TEXT("%s: unbekanntes behavior '%s'"), *Where, *S)); }
		}
		Def.Health = FMath::Max(1.f, CreatureNum(C, TEXT("health"), Def.Health));
		Def.WalkSpeed = CreatureNum(C, TEXT("walk_speed"), Def.WalkSpeed);
		Def.RunSpeed = CreatureNum(C, TEXT("run_speed"), Def.RunSpeed);
		Def.AggroRadius = CreatureNum(C, TEXT("aggro_radius"), Def.AggroRadius);
		Def.AttackRange = CreatureNum(C, TEXT("attack_range"), Def.AttackRange);
		Def.PreferredRange = CreatureNum(C, TEXT("preferred_range"), Def.PreferredRange);
		Def.FleeRange = CreatureNum(C, TEXT("flee_range"), Def.FleeRange);
		Def.StaggerThreshold = CreatureNum(C, TEXT("stagger_threshold"), Def.StaggerThreshold);
		TArray<FString> Spells;
		C->TryGetStringArrayField(TEXT("spells"), Spells);
		for (const FString& Sp : Spells)
		{
			Def.Spells.Add(FName(*Sp));
		}
		const TSharedPtr<FJsonObject>* Mult = nullptr;
		if (C->TryGetObjectField(TEXT("element_mult"), Mult) && Mult)
		{
			for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*Mult)->Values)
			{
				double V = 1.0;
				if (Pair.Value->TryGetNumber(V))
				{
					Def.ElementMultipliers.Add(FName(*Pair.Key), static_cast<float>(V));
				}
			}
		}
		const TSharedPtr<FJsonObject>* Look = nullptr;
		if (C->TryGetObjectField(TEXT("look"), Look) && Look)
		{
			CreatureColor(*Look, TEXT("body"), Def.Look.Body, Where, OutErrors);
			CreatureColor(*Look, TEXT("accent"), Def.Look.Accent, Where, OutErrors);
			CreatureColor(*Look, TEXT("eyes"), Def.Look.Eyes, Where, OutErrors);
			Def.Look.Height = FMath::Clamp(CreatureNum(*Look, TEXT("height"), 1.f), 0.4f, 3.f);
			bool bHood = true;
			if ((*Look)->TryGetBoolField(TEXT("hood"), bHood)) { Def.Look.bHood = bHood; }
			if ((*Look)->TryGetStringField(TEXT("mesh"), S)) { Def.Look.Mesh = FSoftObjectPath(S); }
			if ((*Look)->TryGetStringField(TEXT("anim_class"), S)) { Def.Look.AnimClass = FSoftClassPath(S); }
		}
		Out.Add(MoveTemp(Def));
	}
	return true;
}

void UEwigfelsCreatureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReloadDefinitions();
}

int32 UEwigfelsCreatureSubsystem::ReloadDefinitions()
{
	Creatures.Reset();
	LoadErrors.Reset();
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *CreatureDir(), TEXT("*.json"), true, false);
	Files.Sort();
	for (const FString& File : Files)
	{
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *File))
		{
			continue;
		}
		TArray<FEwigfelsCreatureDef> Defs;
		EwigfelsCreatures::ParseCreatureFile(Text, FPaths::GetCleanFilename(File), Defs, LoadErrors);
		for (FEwigfelsCreatureDef& Def : Defs)
		{
			Creatures.Add(Def.Id, MoveTemp(Def));
		}
	}
	for (const FString& Error : LoadErrors)
	{
		UE_LOG(LogEwigfels, Error, TEXT("Kreaturen: %s"), *Error);
	}
	UE_LOG(LogEwigfels, Log, TEXT("Kreaturen: %d Typen geladen"), Creatures.Num());
	return Creatures.Num();
}

TArray<FName> UEwigfelsCreatureSubsystem::GetAllCreatureIds() const
{
	TArray<FName> Ids;
	Creatures.GetKeys(Ids);
	Ids.Sort(FNameLexicalLess());
	return Ids;
}

namespace
{
	FAutoConsoleCommandWithWorld GCreaturesList(
		TEXT("Ewigfels.Creatures.List"),
		TEXT("Listet alle Gegnertypen."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
			if (const UEwigfelsCreatureSubsystem* Sub = GI ? GI->GetSubsystem<UEwigfelsCreatureSubsystem>() : nullptr)
			{
				for (const FName Id : Sub->GetAllCreatureIds())
				{
					const FEwigfelsCreatureDef* Def = Sub->FindCreature(Id);
					UE_LOG(LogEwigfels, Display, TEXT("%-16s HP %4.0f  %s"), *Id.ToString(), Def->Health, *Def->Name.ToString());
				}
			}
		}));
}
