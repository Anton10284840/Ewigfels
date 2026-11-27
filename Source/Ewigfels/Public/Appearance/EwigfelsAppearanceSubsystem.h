// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "EwigfelsAppearanceSubsystem.generated.h"

class APawn;

/** Zuletzt erstellte Figur dieses Rechners (Datei "EwigfelsLook"). Damit bringt man seine Figur in fremde Koop-Spiele mit. */
UCLASS()
class EWIGFELS_API UEwigfelsLookProfile : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	FEwigfelsAppearance Look;
};

/**
 * Katalog fürs Aussehen (Content/Data/Appearance/*.json), lokales Figur-Profil und Bezahl-Anschluss für Läden.
 * Läuft auf Server und Clients gleich (gleiche Daten im Paket).
 *
 * Konsole (~): Ewigfels.Look.Editor [Laden] · Ewigfels.Look.Random · Ewigfels.Look.Code · Ewigfels.Look.Import NOC1:…
 *              Ewigfels.Look.Reload · Ewigfels.Look.Reset (Figur vergessen, Editor öffnet sich neu)
 */
UCLASS()
class EWIGFELS_API UEwigfelsAppearanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Lädt den Katalog neu. Rückgabe: Anzahl Optionen. Fehler danach in GetLoadErrors(). */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Look")
	int32 ReloadCatalog();

	/** Fügt Katalog-Daten aus einem JSON-Text hinzu (Tests, Mods). */
	bool AddCatalogFromJson(const FString& JsonText, const FString& SourceName);

	const FEwigfelsLookCatalog& GetCatalog() const { return Catalog; }
	const TArray<FString>& GetLoadErrors() const { return LoadErrors; }

	/** Kontext mit Weltstatus-Flags (für "flag:"-Freischaltungen). */
	FEwigfelsLookContext MakeContext(FName ShopId) const;

	/** Startaussehen für eine neue Figur: lokales Profil, sonst Standard aus dem Katalog. bCreated ist immer false. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Look")
	FEwigfelsAppearance GetStartingLook() const;

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Look")
	bool LoadProfile(FEwigfelsAppearance& OutLook) const;

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Look")
	void SaveProfile(const FEwigfelsAppearance& Look);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Look")
	void DeleteProfile();

	/** Name und Anrede als globale Dialog-Variablen: {global.SpielerName}, Geschlecht für Textformen. Nur lokal. */
	void PushDialogueVariables(const FEwigfelsAppearance& Look);

	/**
	 * Bezahlen im Laden (nur Server). Das Inventar (Phase 5) hängt sich hier an: true = bezahlt.
	 * Nicht gesetzt = alles gratis (Phase-3/4-Tests), mit Log-Hinweis.
	 */
	TFunction<bool(APawn* Payer, const FEwigfelsLookShop& Shop, int32 Price)> PaymentHandler;

	/** Verfügbarer Betrag für die Anzeige im Spiegel (-1 = unbekannt/gratis). Inventar setzt das später. */
	TFunction<int32(APawn* Payer)> MoneyQuery;

	static FString AppearanceDataDir();
	static const TCHAR* ProfileSlotName;

private:
	FEwigfelsLookCatalog Catalog;
	TArray<FString> LoadErrors;
};
