// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Magic/EwigfelsSpellSubsystem.h"
#include "Ewigfels.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

FString UEwigfelsSpellSubsystem::SpellDataDir()
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), TEXT("Spells"));
}

void UEwigfelsSpellSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ReloadDefinitions();
}

int32 UEwigfelsSpellSubsystem::ReloadDefinitions()
{
	Spells.Reset();
	Statuses.Reset();
	DefaultLoadout.Reset();
	LoadErrors.Reset();

	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *SpellDataDir(), TEXT("*.json"), true, false);
	// Katalog/ ist der Design-Katalog (Kern-, Alltags-, Koop-Weisen, Lehrer) ohne Kampfwerte – nicht laden.
	Files.RemoveAll([](const FString& F) { return F.Contains(TEXT("/Katalog/")) || F.Contains(TEXT("\\Katalog\\")); });
	Files.Sort();
	for (const FString& File : Files)
	{
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *File))
		{
			LoadErrors.Add(FString::Printf(TEXT("%s: nicht lesbar"), *File));
			continue;
		}
		AddDefinitionsFromJson(Text, FPaths::GetCleanFilename(File));
	}

	for (const FString& Error : LoadErrors)
	{
		UE_LOG(LogEwigfels, Error, TEXT("Weisen: %s"), *Error);
	}
	UE_LOG(LogEwigfels, Log, TEXT("Weisen: %d Weisen, %d Status-Effekte aus %d Dateien geladen"), Spells.Num(), Statuses.Num(), Files.Num());
	return Spells.Num();
}

int32 UEwigfelsSpellSubsystem::AddDefinitionsFromJson(const FString& JsonText, const FString& SourceName)
{
	TArray<FEwigfelsSpellDef> NewSpells;
	TArray<FEwigfelsStatusDef> NewStatuses;
	TArray<FName> Loadout;
	EwigfelsSpells::ParseSpellFile(JsonText, SourceName, NewSpells, NewStatuses, Loadout, LoadErrors);
	const int32 Count = NewSpells.Num();
	Ingest(NewSpells, NewStatuses, Loadout, SourceName);
	return Count;
}

void UEwigfelsSpellSubsystem::Ingest(TArray<FEwigfelsSpellDef>& NewSpells, TArray<FEwigfelsStatusDef>& NewStatuses,
	const TArray<FName>& Loadout, const FString& SourceName)
{
	for (FEwigfelsSpellDef& Def : NewSpells)
	{
		if (Spells.Contains(Def.Id))
		{
			LoadErrors.Add(FString::Printf(TEXT("%s: Weise '%s' doppelt, spaetere Definition gewinnt"), *SourceName, *Def.Id.ToString()));
		}
		Spells.Add(Def.Id, MoveTemp(Def));
	}
	for (FEwigfelsStatusDef& Def : NewStatuses)
	{
		Statuses.Add(Def.Id, MoveTemp(Def));
	}
	if (Loadout.Num() > 0)
	{
		DefaultLoadout = Loadout;
	}
}

bool UEwigfelsSpellSubsystem::GetSpell(FName SpellId, FEwigfelsSpellDef& OutSpell) const
{
	if (const FEwigfelsSpellDef* Found = Spells.Find(SpellId))
	{
		OutSpell = *Found;
		return true;
	}
	return false;
}

TArray<FName> UEwigfelsSpellSubsystem::GetAllSpellIds() const
{
	TArray<FName> Ids;
	Spells.GetKeys(Ids);
	Ids.Sort(FNameLexicalLess());
	return Ids;
}

// ---------------------------------------------------------------------------
// Konsole

namespace
{
	UEwigfelsSpellSubsystem* FindSpellSubsystem(UWorld* World)
	{
		UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
		return GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	}

	FAutoConsoleCommandWithWorld GSpellsList(
		TEXT("Ewigfels.Spells.List"),
		TEXT("Listet alle geladenen Weisen mit Kosten und Abklingzeit."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (UEwigfelsSpellSubsystem* Sub = FindSpellSubsystem(World))
			{
				for (const FName Id : Sub->GetAllSpellIds())
				{
					const FEwigfelsSpellDef* Def = Sub->FindSpell(Id);
					UE_LOG(LogEwigfels, Display, TEXT("%-20s %-10s Atem %3.0f  CD %4.1fs  %s"), *Id.ToString(),
						*Def->School.ToString(), Def->Cost, Def->Cooldown, *Def->Name.ToString());
				}
			}
		}));

	FAutoConsoleCommandWithWorld GSpellsReload(
		TEXT("Ewigfels.Spells.Reload"),
		TEXT("Lädt Content/Data/Spells neu (Werte live ausprobieren)."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (UEwigfelsSpellSubsystem* Sub = FindSpellSubsystem(World))
			{
				Sub->ReloadDefinitions();
			}
		}));
}
