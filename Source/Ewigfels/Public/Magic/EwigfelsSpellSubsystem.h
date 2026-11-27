// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Magic/EwigfelsSpellTypes.h"
#include "EwigfelsSpellSubsystem.generated.h"

/**
 * Katalog aller Weisen und Status-Effekte. Lädt beim Start alle Content/Data/Spells/**.json
 * außer dem Design-Katalog in Content/Data/Spells/Katalog/.
 * Läuft auf Server und Clients gleich (gleiche Daten im Paket), darum reicht es, Weisen per Id zu replizieren.
 *
 * Konsole (~): Ewigfels.Spells.List · Ewigfels.Spells.Reload
 */
UCLASS()
class EWIGFELS_API UEwigfelsSpellSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Lädt alle Dateien neu. Rückgabe: Anzahl Weisen. Fehler stehen danach in GetLoadErrors(). */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Magic")
	int32 ReloadDefinitions();

	/** Lädt Weisen aus einem JSON-Text dazu (für Tests und Mods). */
	int32 AddDefinitionsFromJson(const FString& JsonText, const FString& SourceName);

	const FEwigfelsSpellDef* FindSpell(FName SpellId) const { return Spells.Find(SpellId); }
	const FEwigfelsStatusDef* FindStatus(FName StatusId) const { return Statuses.Find(StatusId); }

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Magic")
	bool GetSpell(FName SpellId, FEwigfelsSpellDef& OutSpell) const;

	/** Start-Belegung der Schnellwahl ("loadout" in den JSON-Dateien). */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	TArray<FName> GetDefaultLoadout() const { return DefaultLoadout; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	TArray<FName> GetAllSpellIds() const;

	const TArray<FString>& GetLoadErrors() const { return LoadErrors; }

	/** Spiel-Ordner mit den Zauberdateien. */
	static FString SpellDataDir();

private:
	void Ingest(TArray<FEwigfelsSpellDef>& NewSpells, TArray<FEwigfelsStatusDef>& NewStatuses, const TArray<FName>& Loadout, const FString& SourceName);

	TMap<FName, FEwigfelsSpellDef> Spells;
	TMap<FName, FEwigfelsStatusDef> Statuses;
	TArray<FName> DefaultLoadout;
	TArray<FString> LoadErrors;
};
