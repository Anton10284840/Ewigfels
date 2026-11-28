// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "EwigfelsAppearanceTypes.generated.h"

/**
 * Aussehen der Spielfigur (Charakter-Editor + Läden). Format und Regeln: docs/CHARACTER_CREATOR.md.
 *
 * FEwigfelsAppearance ist reine Daten (Ids + Zahlen + Farben). Damit ist es klein genug zum Replizieren, landet
 * unverändert im Spielstand und passt als „Look-Code“ in eine Textzeile. Welche Ids es gibt, steht im Katalog
 * Content/Data/Appearance/*.json (FEwigfelsLookCatalog), nicht im Code.
 */

/** Wie NPCs und Texte die Figur ansprechen. Unabhängig von Körper und Stimme. */
UENUM(BlueprintType)
enum class EEwigfelsPronouns : uint8
{
	Neutral UMETA(DisplayName = "Nur Name"),
	Sie UMETA(DisplayName = "sie/ihr"),
	Er UMETA(DisplayName = "er/ihm")
};

/** Ein Gesichts- oder Körperregler. -1 … 1, 0 = Grundform. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSliderValue
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Id;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	float Value = 0.f;

	FEwigfelsSliderValue() = default;
	FEwigfelsSliderValue(FName InId, float InValue) : Id(InId), Value(InValue) {}

	bool operator==(const FEwigfelsSliderValue& O) const { return Id == O.Id && Value == O.Value; }
};

/** Narbe, Sommersprossen, Tattoo, Schminke, Bemalung oder Rune auf der Haut. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsMarkLook
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Id;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor Color = FColor(26, 20, 20);

	/** 0 … 1 */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	float Opacity = 1.f;

	bool operator==(const FEwigfelsMarkLook& O) const { return Id == O.Id && Color == O.Color && Opacity == O.Opacity; }
};

/** Ein Kleidungsstück in einem der 10 Slots, mit 3 Farbzonen, Muster und Stoff. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsOutfitPiece
{
	GENERATED_BODY()

	/** kopf, gesicht, hals, oberkoerper, umhang, guertel, handschuhe, beine, schuhe, tasche */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Slot;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Item;

	/** Farbzonen R/G/B der Material-Maske. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor Color1 = FColor(46, 58, 92);

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor Color2 = FColor(232, 225, 208);

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor Color3 = FColor(201, 180, 88);

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Pattern;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Fabric;

	bool operator==(const FEwigfelsOutfitPiece& O) const
	{
		return Slot == O.Slot && Item == O.Item && Color1 == O.Color1 && Color2 == O.Color2 && Color3 == O.Color3
			&& Pattern == O.Pattern && Fabric == O.Fabric;
	}
	bool SameColors(const FEwigfelsOutfitPiece& O) const
	{
		return Color1 == O.Color1 && Color2 == O.Color2 && Color3 == O.Color3 && Pattern == O.Pattern && Fabric == O.Fabric;
	}
};

/** Aussehen eines Stimmrings. Die Quarzfarbe ist die Farbe der eigenen Magie. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsRingLook
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Metal;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor Quartz = FColor(255, 210, 122);

	bool operator==(const FEwigfelsRingLook& O) const { return Metal == O.Metal && Quartz == O.Quartz; }
};

/** Das komplette Aussehen einer Spielfigur. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsAppearance
{
	GENERATED_BODY()

	/** false, solange die Figur noch nie im Editor bestätigt wurde (dann öffnet sich der Editor). */
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Ewigfels|Look")
	bool bCreated = false;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FString Name;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	EEwigfelsPronouns Pronouns = EEwigfelsPronouns::Neutral;

	// --- Körper ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName BodyType;

	/** 1 = Grundgröße des Körpertyps, Grenzen im Katalog (±5 %). */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	float Height = 1.f;

	// --- Kopf: fertiger Kopf (FaceA). FaceB/FaceBlend/Sliders bleiben für ältere Spielstände und optionale Morphs. ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName FaceA;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName FaceB;

	/** 0 = nur FaceA, 1 = nur FaceB. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	float FaceBlend = 0.f;

	/** Nur Regler ungleich 0 stehen hier drin. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	TArray<FEwigfelsSliderValue> Sliders;

	// --- Haut und Augen ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor SkinColor = FColor(192, 133, 96);

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor EyeColorL = FColor(92, 59, 34);

	/** Ungleich EyeColorL = zweifarbige Augen. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor EyeColorR = FColor(92, 59, 34);

	// --- Haare ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName HairStyle;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor HairColor = FColor(74, 48, 32);

	/** Farbe der Spitzen (Verlauf). Gleich HairColor = einfarbig. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FColor HairTipColor = FColor(74, 48, 32);

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Beard;

	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Brows;

	// --- Haut-Details (max. Anzahl im Katalog) ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	TArray<FEwigfelsMarkLook> Marks;

	// --- Stimme (Lautgeber in Kampf/Emotes, Dialoge sind Textwahl) ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	FName Voice;

	/** -1 … 1, verschiebt die Tonhöhe leicht. */
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	float VoicePitch = 0.f;

	// --- Kleidung (ein Eintrag pro Slot) ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	TArray<FEwigfelsOutfitPiece> Outfit;

	// --- Stimmringe (Index 0 = Ring von zu Hause, bis 4) ---
	UPROPERTY(SaveGame, EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Look")
	TArray<FEwigfelsRingLook> Rings;

	float GetSlider(FName Id) const;
	/** Setzt einen Regler, 0 entfernt ihn. */
	void SetSlider(FName Id, float Value);

	const FEwigfelsOutfitPiece* FindPiece(FName Slot) const;
	FEwigfelsOutfitPiece& FindOrAddPiece(FName Slot);

	const FEwigfelsMarkLook* FindMark(FName Id) const;

	/** Farbe der Magie (Quarz des ersten Rings). */
	FLinearColor GetMagicColor() const;

	bool operator==(const FEwigfelsAppearance& O) const;
	bool operator!=(const FEwigfelsAppearance& O) const { return !(*this == O); }
};

// ---------------------------------------------------------------------------
// Katalog (aus Content/Data/Appearance/*.json)

/** Eine wählbare Sache: Körpertyp, Kopf, Frisur, Bart, Brauen, Mal, Stimme, Kleidungsstück, Muster, Stoff, Metall. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsLookOption
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Id;
	/** Siehe EwigfelsLook::Cat. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Category;
	/** Nur Kleidung: Slot. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Slot;
	/** Nur Male: freckles, scar, makeup, tattoo, paint, rune. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Kind;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FText DisplayName;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FSoftObjectPath Asset;
	/** Nur Köpfe (alt): Morph-Target im Gesichts-Mesh statt eigenem Kopf-Mesh. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Morph;
	/** Bild für die Kachel im Editor (leer = /Game/UI/Creator/Thumbs/T_<id>, falls vorhanden). */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FSoftObjectPath Thumb;
	/** Köpfe: passender Hautton ("skin"), sonst Kachelfarbe ("color"). */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FColor Tint = FColor::White;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") bool bHasTint = false;
	/** "start" (Editor + Läden), "shop" (nur Läden), "flag:<Flag>" (sobald der Weltstatus-Flag gesetzt ist). */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FString Unlock = TEXT("start");
	/** Preis in Kronen im Laden (zusätzlich zum Grundpreis des Ladens). */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") int32 Price = 0;
};

/** Ein Regler im Editor. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSliderDef
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Id;
	/** kopf, brauen, augen, nase, wangen, mund, kiefer, ohren, koerper */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Group;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FText GroupName;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FText DisplayName;
	/** Morph-Target für Werte > 0 bzw. < 0. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName MorphPos;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName MorphNeg;
};

/** Ein Laden (oder Spiegel), der das Aussehen ändern darf. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsLookShop
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Id;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FText DisplayName;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName Npc;
	/** Änderbare Bereiche (EwigfelsLook::Change), "outfit:<slot>" = nur dieser Kleidungs-Slot. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") TArray<FName> Categories;
	/** Kronen pro geändertem Bereich. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") int32 BasePrice = 0;
	/** Statt Kronen: Gegenstand (z. B. Kräuter bei der Spiegelhexe). */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") FName PriceItem;
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Look") int32 PriceItemCount = 0;
};

/** Alles, was der Editor anbietet. */
USTRUCT()
struct EWIGFELS_API FEwigfelsLookCatalog
{
	GENERATED_BODY()

	/** Kategorie (EwigfelsLook::Cat) → Optionen in Datei-Reihenfolge. */
	TMap<FName, TArray<FEwigfelsLookOption>> Options;
	TArray<FEwigfelsSliderDef> Sliders;
	/** skin, hair, eyes, dye, quartz, makeup */
	TMap<FName, TArray<FColor>> Palettes;
	TArray<FName> OutfitSlots;
	/** Vorschläge für „Namen würfeln“ (wichtig am Controller, da Tippen mühsam ist). */
	TArray<FString> RandomNames;
	TMap<FName, FEwigfelsLookShop> Shops;
	FEwigfelsAppearance Defaults;
	float HeightMin = 0.95f;
	float HeightMax = 1.05f;
	int32 MaxMarks = 8;
	int32 NameMin = 2;
	int32 NameMax = 20;

	const TArray<FEwigfelsLookOption>& GetOptions(FName Category) const;
	const FEwigfelsLookOption* Find(FName Category, FName Id) const;
	const FEwigfelsSliderDef* FindSlider(FName Id) const;
	/** Kleidungsstücke eines Slots. */
	TArray<const FEwigfelsLookOption*> GetOutfitOptions(FName Slot) const;
	bool IsEmpty() const { return Options.Num() == 0; }
};

/** Wo eine Änderung passiert: Charakter-Editor am Start oder ein Laden. */
struct EWIGFELS_API FEwigfelsLookContext
{
	/** None = Charakter-Editor (Spielbeginn). */
	FName ShopId;
	/** Weltstatus-Flags für "flag:"-Freischaltungen. Leer = keine Flags gesetzt. */
	TFunction<bool(FName)> HasFlag;

	bool IsCreation() const { return ShopId.IsNone(); }
};

namespace EwigfelsLook
{
	/** Katalog-Kategorien. */
	namespace Cat
	{
		extern EWIGFELS_API const FName Body;
		extern EWIGFELS_API const FName Face;
		extern EWIGFELS_API const FName Hair;
		extern EWIGFELS_API const FName Beard;
		extern EWIGFELS_API const FName Brows;
		extern EWIGFELS_API const FName Marks;
		extern EWIGFELS_API const FName Voice;
		extern EWIGFELS_API const FName Outfit;
		extern EWIGFELS_API const FName Pattern;
		extern EWIGFELS_API const FName Fabric;
		extern EWIGFELS_API const FName Metal;
	}

	/** Änderungs-Bereiche (für Läden und Preise). */
	namespace Change
	{
		extern EWIGFELS_API const FName Name;
		extern EWIGFELS_API const FName Body;
		extern EWIGFELS_API const FName Face;
		extern EWIGFELS_API const FName Skin;
		extern EWIGFELS_API const FName Eyes;
		extern EWIGFELS_API const FName Hair;
		extern EWIGFELS_API const FName Beard;
		extern EWIGFELS_API const FName Brows;
		extern EWIGFELS_API const FName Marks;
		extern EWIGFELS_API const FName Voice;
		/** Neues Kleidungsstück (Bereich "outfit:<slot>"). */
		extern EWIGFELS_API const FName Outfit;
		/** Nur Farben/Muster/Stoff vorhandener Stücke. */
		extern EWIGFELS_API const FName OutfitColors;
		extern EWIGFELS_API const FName Rings;
	}

	/** Liest eine Katalog-Datei (Appearance.json oder Shops.json) und fügt sie dem Katalog hinzu. */
	EWIGFELS_API bool ParseCatalogJson(const FString& JsonText, const FString& SourceName, FEwigfelsLookCatalog& Catalog, TArray<FString>& OutErrors);

	/** Repariert ein Aussehen gegen den Katalog: Zahlen begrenzen, unbekannte Ids durch Standard ersetzen, Slots auffüllen.
	 *  Rückgabe: Anzahl Korrekturen (0 = war schon sauber). OutFixes beschreibt jede Korrektur. */
	EWIGFELS_API int32 Sanitize(FEwigfelsAppearance& Look, const FEwigfelsLookCatalog& Catalog, TArray<FString>* OutFixes = nullptr);

	/** Ist die Option im Kontext wählbar? (Editor: "start" + gesetzte Flags, Laden zusätzlich "shop"). */
	EWIGFELS_API bool IsUnlocked(const FEwigfelsLookOption& Option, const FEwigfelsLookContext& Context);

	/** Welche Bereiche (Change::…) unterscheiden sich? Kleidung als "outfit:<slot>" bzw. "outfit_colors". */
	EWIGFELS_API TArray<FName> GetChangedAreas(const FEwigfelsAppearance& Old, const FEwigfelsAppearance& New);

	/** Darf Old → New im Kontext passieren, und was kostet es? Prüft Laden-Bereiche und Freischaltungen. */
	EWIGFELS_API bool CheckChange(const FEwigfelsAppearance& Old, const FEwigfelsAppearance& New, const FEwigfelsLookCatalog& Catalog,
		const FEwigfelsLookContext& Context, int32& OutPrice, FText& OutReason);

	/** Darf der Laden diesen Bereich ändern? ("outfit" deckt alle "outfit:<slot>" und "outfit_colors" ab.) */
	EWIGFELS_API bool ShopAllows(const FEwigfelsLookShop& Shop, FName Area);

	/** Zufälliges, gültiges Startaussehen (nur "start"-Optionen). */
	EWIGFELS_API FEwigfelsAppearance MakeRandom(const FEwigfelsLookCatalog& Catalog, FRandomStream& Random);

	/** Zufälliger Name aus dem Katalog (leer, wenn keiner da ist). Avoid: nicht genau diesen Namen nehmen. */
	EWIGFELS_API FString RandomName(const FEwigfelsLookCatalog& Catalog, FRandomStream& Random, const FString& Avoid = FString());

	/** Look-Code zum Teilen ("NOC1:" + Base64-JSON). */
	EWIGFELS_API FString ToLookCode(const FEwigfelsAppearance& Look);
	EWIGFELS_API bool FromLookCode(const FString& Code, FEwigfelsAppearance& OutLook);

	EWIGFELS_API FString ColorToHex(const FColor& Color);
	EWIGFELS_API bool HexToColor(const FString& Hex, FColor& OutColor);

	/** Name ohne Steuerzeichen, getrimmt, auf Länge gekürzt. */
	EWIGFELS_API FString CleanName(const FString& Name, int32 MaxLen);
}
