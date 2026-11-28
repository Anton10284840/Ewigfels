// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Appearance/EwigfelsAppearanceTypes.h"
#include "Dom/JsonObject.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "JsonObjectConverter.h"
#include "Misc/Base64.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#define LOCTEXT_NAMESPACE "EwigfelsLook"

namespace EwigfelsLook
{
	namespace Cat
	{
		const FName Body(TEXT("body"));
		const FName Face(TEXT("face"));
		const FName Hair(TEXT("hair"));
		const FName Beard(TEXT("beard"));
		const FName Brows(TEXT("brows"));
		const FName Marks(TEXT("marks"));
		const FName Voice(TEXT("voice"));
		const FName Outfit(TEXT("outfit"));
		const FName Pattern(TEXT("pattern"));
		const FName Fabric(TEXT("fabric"));
		const FName Metal(TEXT("metal"));
	}

	namespace Change
	{
		const FName Name(TEXT("name"));
		const FName Body(TEXT("body"));
		const FName Face(TEXT("face"));
		const FName Skin(TEXT("skin"));
		const FName Eyes(TEXT("eyes"));
		const FName Hair(TEXT("hair"));
		const FName Beard(TEXT("beard"));
		const FName Brows(TEXT("brows"));
		const FName Marks(TEXT("marks"));
		const FName Voice(TEXT("voice"));
		const FName Outfit(TEXT("outfit"));
		const FName OutfitColors(TEXT("outfit_colors"));
		const FName Rings(TEXT("rings"));
	}
}

namespace
{
	constexpr int32 LookMaxRings = 4;

	/** JSON-Schlüssel → Katalog-Kategorie. */
	struct FLookListKey
	{
		const TCHAR* Key;
		FName Category;
	};

	bool LookIsGerman()
	{
		const FString Lang = FInternationalization::Get().GetCurrentCulture()->GetTwoLetterISOLanguageName();
		return Lang.IsEmpty() || Lang == TEXT("de");
	}

	FText LookReadName(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field = TEXT("name"))
	{
		const TSharedPtr<FJsonObject>* NameObj = nullptr;
		if (Obj->TryGetObjectField(Field, NameObj))
		{
			FString De, En;
			(*NameObj)->TryGetStringField(TEXT("de"), De);
			(*NameObj)->TryGetStringField(TEXT("en"), En);
			return FText::FromString(LookIsGerman() || En.IsEmpty() ? De : En);
		}
		FString Plain;
		Obj->TryGetStringField(Field, Plain);
		return FText::FromString(Plain);
	}

	FName LookReadFName(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field)
	{
		FString Str;
		return Obj->TryGetStringField(Field, Str) && !Str.IsEmpty() ? FName(*Str) : NAME_None;
	}

	FColor LookReadColor(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field, const FColor& Fallback, const FString& Where, TArray<FString>& Errors)
	{
		FString Str;
		FColor Color = Fallback;
		if (Obj->TryGetStringField(Field, Str) && !EwigfelsLook::HexToColor(Str, Color))
		{
			Errors.Add(FString::Printf(TEXT("%s: Farbe '%s' in '%s' ist kein #RRGGBB"), *Where, *Str, Field));
		}
		return Color;
	}

	void LookParseDefaults(const TSharedPtr<FJsonObject>& D, FEwigfelsLookCatalog& Catalog, const FString& Where, TArray<FString>& Errors)
	{
		FEwigfelsAppearance& L = Catalog.Defaults;
		L.BodyType = LookReadFName(D, TEXT("body_type"));
		D->TryGetNumberField(TEXT("height"), L.Height);
		L.FaceA = LookReadFName(D, TEXT("face_a"));
		L.FaceB = LookReadFName(D, TEXT("face_b"));
		D->TryGetNumberField(TEXT("face_blend"), L.FaceBlend);
		L.SkinColor = LookReadColor(D, TEXT("skin_color"), L.SkinColor, Where, Errors);
		L.EyeColorL = LookReadColor(D, TEXT("eye_color_l"), L.EyeColorL, Where, Errors);
		L.EyeColorR = LookReadColor(D, TEXT("eye_color_r"), L.EyeColorL, Where, Errors);
		L.HairStyle = LookReadFName(D, TEXT("hair_style"));
		L.HairColor = LookReadColor(D, TEXT("hair_color"), L.HairColor, Where, Errors);
		L.HairTipColor = LookReadColor(D, TEXT("hair_tip_color"), L.HairColor, Where, Errors);
		L.Beard = LookReadFName(D, TEXT("beard"));
		L.Brows = LookReadFName(D, TEXT("brows"));
		L.Voice = LookReadFName(D, TEXT("voice"));
		D->TryGetNumberField(TEXT("voice_pitch"), L.VoicePitch);

		FString Pronouns;
		if (D->TryGetStringField(TEXT("pronouns"), Pronouns))
		{
			L.Pronouns = Pronouns == TEXT("sie") ? EEwigfelsPronouns::Sie : Pronouns == TEXT("er") ? EEwigfelsPronouns::Er : EEwigfelsPronouns::Neutral;
		}

		FColor Dyes[3] = { FColor(46, 58, 92), FColor(232, 225, 208), FColor(201, 180, 88) };
		const TArray<TSharedPtr<FJsonValue>>* ColorArray = nullptr;
		if (D->TryGetArrayField(TEXT("outfit_colors"), ColorArray))
		{
			for (int32 i = 0; i < FMath::Min(3, ColorArray->Num()); ++i)
			{
				EwigfelsLook::HexToColor((*ColorArray)[i]->AsString(), Dyes[i]);
			}
		}
		const FName Pattern = LookReadFName(D, TEXT("outfit_pattern"));
		const FName Fabric = LookReadFName(D, TEXT("outfit_fabric"));
		const TSharedPtr<FJsonObject>* OutfitObj = nullptr;
		L.Outfit.Reset();
		if (D->TryGetObjectField(TEXT("outfit"), OutfitObj))
		{
			for (const auto& Pair : (*OutfitObj)->Values)
			{
				FEwigfelsOutfitPiece Piece;
				Piece.Slot = FName(*Pair.Key);
				Piece.Item = FName(*Pair.Value->AsString());
				Piece.Color1 = Dyes[0];
				Piece.Color2 = Dyes[1];
				Piece.Color3 = Dyes[2];
				Piece.Pattern = Pattern;
				Piece.Fabric = Fabric;
				L.Outfit.Add(Piece);
			}
		}
		FEwigfelsRingLook Ring;
		Ring.Metal = LookReadFName(D, TEXT("ring_metal"));
		Ring.Quartz = LookReadColor(D, TEXT("quartz"), Ring.Quartz, Where, Errors);
		L.Rings = { Ring };
	}

	/** Alle gewählten (Kategorie, Id)-Paare eines Aussehens – für Freischaltung und Preis. */
	void LookCollectChoices(const FEwigfelsAppearance& L, TArray<TPair<FName, FName>>& Out)
	{
		using namespace EwigfelsLook;
		Out.Emplace(Cat::Body, L.BodyType);
		Out.Emplace(Cat::Face, L.FaceA);
		Out.Emplace(Cat::Face, L.FaceB);
		Out.Emplace(Cat::Hair, L.HairStyle);
		Out.Emplace(Cat::Beard, L.Beard);
		Out.Emplace(Cat::Brows, L.Brows);
		Out.Emplace(Cat::Voice, L.Voice);
		for (const FEwigfelsMarkLook& Mark : L.Marks)
		{
			Out.Emplace(Cat::Marks, Mark.Id);
		}
		for (const FEwigfelsOutfitPiece& Piece : L.Outfit)
		{
			Out.Emplace(Cat::Outfit, Piece.Item);
			Out.Emplace(Cat::Pattern, Piece.Pattern);
			Out.Emplace(Cat::Fabric, Piece.Fabric);
		}
		for (const FEwigfelsRingLook& Ring : L.Rings)
		{
			Out.Emplace(Cat::Metal, Ring.Metal);
		}
	}

	bool LookIsBodySlider(FName Id)
	{
		return Id.ToString().StartsWith(TEXT("koerper_"));
	}

	bool LookSlidersEqual(const FEwigfelsAppearance& A, const FEwigfelsAppearance& B, bool bBody)
	{
		auto Collect = [bBody](const FEwigfelsAppearance& L)
		{
			TMap<FName, float> Map;
			for (const FEwigfelsSliderValue& S : L.Sliders)
			{
				if (LookIsBodySlider(S.Id) == bBody && S.Value != 0.f)
				{
					Map.Add(S.Id, S.Value);
				}
			}
			return Map;
		};
		const TMap<FName, float> MA = Collect(A);
		const TMap<FName, float> MB = Collect(B);
		if (MA.Num() != MB.Num())
		{
			return false;
		}
		for (const auto& Pair : MA)
		{
			const float* Other = MB.Find(Pair.Key);
			if (!Other || !FMath::IsNearlyEqual(*Other, Pair.Value, 0.001f))
			{
				return false;
			}
		}
		return true;
	}

	float LookSafeClamp(float Value, float Min, float Max, float Fallback)
	{
		return FMath::IsFinite(Value) ? FMath::Clamp(Value, Min, Max) : Fallback;
	}
}

// ---------------------------------------------------------------------------
// FEwigfelsAppearance

float FEwigfelsAppearance::GetSlider(FName Id) const
{
	for (const FEwigfelsSliderValue& S : Sliders)
	{
		if (S.Id == Id)
		{
			return S.Value;
		}
	}
	return 0.f;
}

void FEwigfelsAppearance::SetSlider(FName Id, float Value)
{
	const int32 Index = Sliders.IndexOfByPredicate([Id](const FEwigfelsSliderValue& S) { return S.Id == Id; });
	if (FMath::Abs(Value) < 0.005f)
	{
		if (Index != INDEX_NONE)
		{
			Sliders.RemoveAt(Index);
		}
		return;
	}
	if (Index != INDEX_NONE)
	{
		Sliders[Index].Value = Value;
	}
	else
	{
		Sliders.Add(FEwigfelsSliderValue(Id, Value));
	}
}

const FEwigfelsOutfitPiece* FEwigfelsAppearance::FindPiece(FName Slot) const
{
	return Outfit.FindByPredicate([Slot](const FEwigfelsOutfitPiece& P) { return P.Slot == Slot; });
}

FEwigfelsOutfitPiece& FEwigfelsAppearance::FindOrAddPiece(FName Slot)
{
	if (FEwigfelsOutfitPiece* Found = Outfit.FindByPredicate([Slot](const FEwigfelsOutfitPiece& P) { return P.Slot == Slot; }))
	{
		return *Found;
	}
	FEwigfelsOutfitPiece& Piece = Outfit.AddDefaulted_GetRef();
	Piece.Slot = Slot;
	return Piece;
}

const FEwigfelsMarkLook* FEwigfelsAppearance::FindMark(FName Id) const
{
	return Marks.FindByPredicate([Id](const FEwigfelsMarkLook& M) { return M.Id == Id; });
}

FLinearColor FEwigfelsAppearance::GetMagicColor() const
{
	return Rings.Num() > 0 ? FLinearColor::FromSRGBColor(Rings[0].Quartz) : FLinearColor(1.f, 0.8f, 0.45f);
}

bool FEwigfelsAppearance::operator==(const FEwigfelsAppearance& O) const
{
	return bCreated == O.bCreated && Name == O.Name && Pronouns == O.Pronouns && BodyType == O.BodyType && Height == O.Height
		&& FaceA == O.FaceA && FaceB == O.FaceB && FaceBlend == O.FaceBlend && Sliders == O.Sliders && SkinColor == O.SkinColor
		&& EyeColorL == O.EyeColorL && EyeColorR == O.EyeColorR && HairStyle == O.HairStyle && HairColor == O.HairColor
		&& HairTipColor == O.HairTipColor && Beard == O.Beard && Brows == O.Brows && Marks == O.Marks && Voice == O.Voice
		&& VoicePitch == O.VoicePitch && Outfit == O.Outfit && Rings == O.Rings;
}

// ---------------------------------------------------------------------------
// FEwigfelsLookCatalog

const TArray<FEwigfelsLookOption>& FEwigfelsLookCatalog::GetOptions(FName Category) const
{
	static const TArray<FEwigfelsLookOption> Empty;
	const TArray<FEwigfelsLookOption>* Found = Options.Find(Category);
	return Found ? *Found : Empty;
}

const FEwigfelsLookOption* FEwigfelsLookCatalog::Find(FName Category, FName Id) const
{
	if (Id.IsNone())
	{
		return nullptr;
	}
	const TArray<FEwigfelsLookOption>* List = Options.Find(Category);
	return List ? List->FindByPredicate([Id](const FEwigfelsLookOption& O) { return O.Id == Id; }) : nullptr;
}

const FEwigfelsSliderDef* FEwigfelsLookCatalog::FindSlider(FName Id) const
{
	return Sliders.FindByPredicate([Id](const FEwigfelsSliderDef& S) { return S.Id == Id; });
}

TArray<const FEwigfelsLookOption*> FEwigfelsLookCatalog::GetOutfitOptions(FName Slot) const
{
	TArray<const FEwigfelsLookOption*> Result;
	for (const FEwigfelsLookOption& Option : GetOptions(EwigfelsLook::Cat::Outfit))
	{
		if (Option.Slot == Slot)
		{
			Result.Add(&Option);
		}
	}
	return Result;
}

// ---------------------------------------------------------------------------
// Laden der Katalog-Dateien

bool EwigfelsLook::ParseCatalogJson(const FString& JsonText, const FString& SourceName, FEwigfelsLookCatalog& Catalog, TArray<FString>& OutErrors)
{
	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		OutErrors.Add(FString::Printf(TEXT("%s: kein gueltiges JSON"), *SourceName));
		return false;
	}
	const int32 ErrorsBefore = OutErrors.Num();

	const TSharedPtr<FJsonObject>* Limits = nullptr;
	if (Root->TryGetObjectField(TEXT("limits"), Limits))
	{
		(*Limits)->TryGetNumberField(TEXT("height_min"), Catalog.HeightMin);
		(*Limits)->TryGetNumberField(TEXT("height_max"), Catalog.HeightMax);
		(*Limits)->TryGetNumberField(TEXT("max_marks"), Catalog.MaxMarks);
		(*Limits)->TryGetNumberField(TEXT("name_min"), Catalog.NameMin);
		(*Limits)->TryGetNumberField(TEXT("name_max"), Catalog.NameMax);
	}

	const TArray<TSharedPtr<FJsonValue>>* Slots = nullptr;
	if (Root->TryGetArrayField(TEXT("outfit_slots"), Slots))
	{
		for (const TSharedPtr<FJsonValue>& Slot : *Slots)
		{
			Catalog.OutfitSlots.AddUnique(FName(*Slot->AsString()));
		}
	}

	static const FLookListKey Lists[] = {
		{ TEXT("body_types"), EwigfelsLook::Cat::Body }, { TEXT("heads"), EwigfelsLook::Cat::Face },
		{ TEXT("face_presets"), EwigfelsLook::Cat::Face },
		{ TEXT("hair_styles"), EwigfelsLook::Cat::Hair }, { TEXT("beards"), EwigfelsLook::Cat::Beard },
		{ TEXT("brows"), EwigfelsLook::Cat::Brows }, { TEXT("marks"), EwigfelsLook::Cat::Marks },
		{ TEXT("voices"), EwigfelsLook::Cat::Voice }, { TEXT("outfit"), EwigfelsLook::Cat::Outfit },
		{ TEXT("patterns"), EwigfelsLook::Cat::Pattern }, { TEXT("fabrics"), EwigfelsLook::Cat::Fabric },
		{ TEXT("ring_metals"), EwigfelsLook::Cat::Metal } };

	for (const FLookListKey& List : Lists)
	{
		const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
		if (!Root->TryGetArrayField(List.Key, Items))
		{
			continue;
		}
		TArray<FEwigfelsLookOption>& Target = Catalog.Options.FindOrAdd(List.Category);
		for (const TSharedPtr<FJsonValue>& Value : *Items)
		{
			const TSharedPtr<FJsonObject> Obj = Value->AsObject();
			if (!Obj.IsValid())
			{
				continue;
			}
			FEwigfelsLookOption Option;
			Option.Category = List.Category;
			Option.Id = LookReadFName(Obj, TEXT("id"));
			Option.Slot = LookReadFName(Obj, TEXT("slot"));
			Option.Kind = LookReadFName(Obj, TEXT("kind"));
			Option.Morph = LookReadFName(Obj, TEXT("morph"));
			Option.DisplayName = LookReadName(Obj);
			FString Asset;
			if (Obj->TryGetStringField(TEXT("asset"), Asset) && !Asset.IsEmpty())
			{
				Option.Asset = FSoftObjectPath(Asset);
			}
			Obj->TryGetStringField(TEXT("unlock"), Option.Unlock);
			Obj->TryGetNumberField(TEXT("price"), Option.Price);

			const FString Where = FString::Printf(TEXT("%s/%s/%s"), *SourceName, List.Key, *Option.Id.ToString());
			FString Thumb;
			if (Obj->TryGetStringField(TEXT("thumb"), Thumb) && !Thumb.IsEmpty())
			{
				Option.Thumb = FSoftObjectPath(Thumb);
			}
			// Farbe der Kachel bzw. Hautton eines Kopfs
			for (const TCHAR* Field : { TEXT("skin"), TEXT("color") })
			{
				if (Obj->HasField(Field))
				{
					Option.Tint = LookReadColor(Obj, Field, Option.Tint, Where, OutErrors);
					Option.bHasTint = true;
				}
			}
			if (Option.Id.IsNone())
			{
				OutErrors.Add(FString::Printf(TEXT("%s/%s: Eintrag ohne id"), *SourceName, List.Key));
				continue;
			}
			if (Option.Unlock != TEXT("start") && Option.Unlock != TEXT("shop") && !Option.Unlock.StartsWith(TEXT("flag:")))
			{
				OutErrors.Add(FString::Printf(TEXT("%s: unlock '%s' unbekannt (start, shop, flag:<Flag>)"), *Where, *Option.Unlock));
			}
			if (List.Category == EwigfelsLook::Cat::Outfit && !Catalog.OutfitSlots.Contains(Option.Slot))
			{
				OutErrors.Add(FString::Printf(TEXT("%s: Slot '%s' unbekannt"), *Where, *Option.Slot.ToString()));
			}
			if (Target.ContainsByPredicate([&Option](const FEwigfelsLookOption& O) { return O.Id == Option.Id; }))
			{
				OutErrors.Add(FString::Printf(TEXT("%s: id doppelt"), *Where));
				continue;
			}
			Target.Add(MoveTemp(Option));
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* Names = nullptr;
	if (Root->TryGetArrayField(TEXT("names"), Names))
	{
		for (const TSharedPtr<FJsonValue>& Value : *Names)
		{
			const FString NameText = Value.IsValid() ? Value->AsString() : FString();
			if (!NameText.IsEmpty())
			{
				Catalog.RandomNames.AddUnique(NameText);
			}
		}
	}

	const TArray<TSharedPtr<FJsonValue>>* Sliders = nullptr;
	if (Root->TryGetArrayField(TEXT("sliders"), Sliders))
	{
		for (const TSharedPtr<FJsonValue>& Value : *Sliders)
		{
			const TSharedPtr<FJsonObject> Obj = Value->AsObject();
			if (!Obj.IsValid())
			{
				continue;
			}
			FEwigfelsSliderDef Def;
			Def.Id = LookReadFName(Obj, TEXT("id"));
			Def.Group = LookReadFName(Obj, TEXT("group"));
			Def.GroupName = LookReadName(Obj, TEXT("group_name"));
			Def.DisplayName = LookReadName(Obj);
			Def.MorphPos = LookReadFName(Obj, TEXT("morph_pos"));
			Def.MorphNeg = LookReadFName(Obj, TEXT("morph_neg"));
			if (Def.Id.IsNone() || Catalog.FindSlider(Def.Id))
			{
				OutErrors.Add(FString::Printf(TEXT("%s/sliders: id fehlt oder doppelt ('%s')"), *SourceName, *Def.Id.ToString()));
				continue;
			}
			Catalog.Sliders.Add(Def);
		}
	}

	const TSharedPtr<FJsonObject>* Palettes = nullptr;
	if (Root->TryGetObjectField(TEXT("palettes"), Palettes))
	{
		for (const auto& Pair : (*Palettes)->Values)
		{
			TArray<FColor>& Colors = Catalog.Palettes.FindOrAdd(FName(*Pair.Key));
			for (const TSharedPtr<FJsonValue>& Entry : Pair.Value->AsArray())
			{
				FColor Color;
				if (HexToColor(Entry->AsString(), Color))
				{
					Colors.Add(Color);
				}
				else
				{
					OutErrors.Add(FString::Printf(TEXT("%s/palettes/%s: '%s' ist kein #RRGGBB"), *SourceName, *Pair.Key, *Entry->AsString()));
				}
			}
		}
	}

	const TSharedPtr<FJsonObject>* Defaults = nullptr;
	if (Root->TryGetObjectField(TEXT("defaults"), Defaults))
	{
		LookParseDefaults(*Defaults, Catalog, SourceName + TEXT("/defaults"), OutErrors);
	}

	const TArray<TSharedPtr<FJsonValue>>* Shops = nullptr;
	if (Root->TryGetArrayField(TEXT("shops"), Shops))
	{
		for (const TSharedPtr<FJsonValue>& Value : *Shops)
		{
			const TSharedPtr<FJsonObject> Obj = Value->AsObject();
			if (!Obj.IsValid())
			{
				continue;
			}
			FEwigfelsLookShop Shop;
			Shop.Id = LookReadFName(Obj, TEXT("id"));
			Shop.DisplayName = LookReadName(Obj);
			Shop.Npc = LookReadFName(Obj, TEXT("npc"));
			Shop.PriceItem = LookReadFName(Obj, TEXT("price_item"));
			Obj->TryGetNumberField(TEXT("base_price"), Shop.BasePrice);
			Obj->TryGetNumberField(TEXT("price_item_count"), Shop.PriceItemCount);
			const TArray<TSharedPtr<FJsonValue>>* Cats = nullptr;
			if (Obj->TryGetArrayField(TEXT("categories"), Cats))
			{
				for (const TSharedPtr<FJsonValue>& C : *Cats)
				{
					Shop.Categories.Add(FName(*C->AsString()));
				}
			}
			if (Shop.Id.IsNone() || Catalog.Shops.Contains(Shop.Id))
			{
				OutErrors.Add(FString::Printf(TEXT("%s/shops: id fehlt oder doppelt ('%s')"), *SourceName, *Shop.Id.ToString()));
				continue;
			}
			Catalog.Shops.Add(Shop.Id, Shop);
		}
	}

	return OutErrors.Num() == ErrorsBefore;
}

// ---------------------------------------------------------------------------
// Reparieren

int32 EwigfelsLook::Sanitize(FEwigfelsAppearance& L, const FEwigfelsLookCatalog& C, TArray<FString>* OutFixes)
{
	int32 Fixes = 0;
	auto Fix = [&Fixes, OutFixes](const FString& What)
	{
		++Fixes;
		if (OutFixes)
		{
			OutFixes->Add(What);
		}
	};
	const FEwigfelsAppearance& D = C.Defaults;

	// Id muss in der Kategorie existieren, sonst Standard (oder erste Option).
	auto FixId = [&](FName& Id, FName Category, FName Default, const TCHAR* What, bool bAllowNone)
	{
		if ((bAllowNone && Id.IsNone()) || C.Find(Category, Id))
		{
			return;
		}
		FName Replacement = C.Find(Category, Default) ? Default : NAME_None;
		if (Replacement.IsNone() && !bAllowNone && C.GetOptions(Category).Num() > 0)
		{
			Replacement = C.GetOptions(Category)[0].Id;
		}
		Fix(FString::Printf(TEXT("%s '%s' unbekannt -> '%s'"), What, *Id.ToString(), *Replacement.ToString()));
		Id = Replacement;
	};
	auto FixFloat = [&](float& Value, float Min, float Max, float Fallback, const TCHAR* What)
	{
		const float Clean = LookSafeClamp(Value, Min, Max, Fallback);
		if (Clean != Value)
		{
			Fix(FString::Printf(TEXT("%s %.3f -> %.3f"), What, Value, Clean));
			Value = Clean;
		}
	};
	auto FixAlpha = [](FColor& Color) { Color.A = 255; };

	const FString CleanedName = CleanName(L.Name, C.NameMax);
	if (CleanedName != L.Name)
	{
		Fix(TEXT("Name bereinigt"));
		L.Name = CleanedName;
	}
	if (static_cast<uint8>(L.Pronouns) > static_cast<uint8>(EEwigfelsPronouns::Er))
	{
		Fix(TEXT("Anrede ungueltig"));
		L.Pronouns = EEwigfelsPronouns::Neutral;
	}

	FixId(L.BodyType, Cat::Body, D.BodyType, TEXT("Koerpertyp"), false);
	FixFloat(L.Height, C.HeightMin, C.HeightMax, 1.f, TEXT("Groesse"));
	FixId(L.FaceA, Cat::Face, D.FaceA, TEXT("Gesicht A"), false);
	FixId(L.FaceB, Cat::Face, D.FaceB, TEXT("Gesicht B"), false);
	FixFloat(L.FaceBlend, 0.f, 1.f, 0.f, TEXT("Gesichtsmischung"));

	// Regler: nur bekannte, jeder einmal (der erste zählt), -1 … 1, Nullen raus
	TSet<FName> SeenSliders;
	for (int32 i = 0; i < L.Sliders.Num();)
	{
		FEwigfelsSliderValue& S = L.Sliders[i];
		if (!C.FindSlider(S.Id) || SeenSliders.Contains(S.Id))
		{
			Fix(FString::Printf(TEXT("Regler '%s' unbekannt/doppelt entfernt"), *S.Id.ToString()));
			L.Sliders.RemoveAt(i);
			continue;
		}
		SeenSliders.Add(S.Id);
		FixFloat(S.Value, -1.f, 1.f, 0.f, TEXT("Regler"));
		if (FMath::Abs(S.Value) < 0.005f)
		{
			L.Sliders.RemoveAt(i); // 0 = Grundform, muss nicht gespeichert werden
			continue;
		}
		++i;
	}
	L.Sliders.Sort([](const FEwigfelsSliderValue& A, const FEwigfelsSliderValue& B) { return A.Id.LexicalLess(B.Id); });

	FixAlpha(L.SkinColor);
	FixAlpha(L.EyeColorL);
	FixAlpha(L.EyeColorR);
	FixAlpha(L.HairColor);
	FixAlpha(L.HairTipColor);

	FixId(L.HairStyle, Cat::Hair, D.HairStyle, TEXT("Frisur"), false);
	FixId(L.Beard, Cat::Beard, D.Beard, TEXT("Bart"), false);
	FixId(L.Brows, Cat::Brows, D.Brows, TEXT("Brauen"), false);
	FixId(L.Voice, Cat::Voice, D.Voice, TEXT("Stimme"), false);
	FixFloat(L.VoicePitch, -1.f, 1.f, 0.f, TEXT("Tonhoehe"));

	// Male: bekannt, einmalig, begrenzt
	TSet<FName> SeenMarks;
	for (int32 i = 0; i < L.Marks.Num();)
	{
		FEwigfelsMarkLook& M = L.Marks[i];
		if (!C.Find(Cat::Marks, M.Id) || SeenMarks.Contains(M.Id) || SeenMarks.Num() >= C.MaxMarks)
		{
			Fix(FString::Printf(TEXT("Mal '%s' entfernt"), *M.Id.ToString()));
			L.Marks.RemoveAt(i);
			continue;
		}
		SeenMarks.Add(M.Id);
		FixAlpha(M.Color);
		FixFloat(M.Opacity, 0.f, 1.f, 1.f, TEXT("Deckkraft"));
		++i;
	}

	// Kleidung: genau ein Stück pro Slot, in Slot-Reihenfolge
	const FName DefaultPattern = C.GetOptions(Cat::Pattern).Num() ? C.GetOptions(Cat::Pattern)[0].Id : NAME_None;
	const FName DefaultFabric = C.GetOptions(Cat::Fabric).Num() ? C.GetOptions(Cat::Fabric)[0].Id : NAME_None;
	TArray<FEwigfelsOutfitPiece> Clean;
	for (const FName Slot : C.OutfitSlots)
	{
		const FEwigfelsOutfitPiece* Existing = L.FindPiece(Slot);
		FEwigfelsOutfitPiece Piece;
		if (Existing)
		{
			Piece = *Existing;
		}
		else
		{
			Fix(FString::Printf(TEXT("Slot '%s' fehlte"), *Slot.ToString()));
			if (const FEwigfelsOutfitPiece* Def = D.FindPiece(Slot))
			{
				Piece = *Def;
			}
			Piece.Slot = Slot;
		}
		const FEwigfelsLookOption* Item = C.Find(Cat::Outfit, Piece.Item);
		if (!Item || Item->Slot != Slot)
		{
			const FEwigfelsOutfitPiece* Def = D.FindPiece(Slot);
			const FEwigfelsLookOption* DefItem = Def ? C.Find(Cat::Outfit, Def->Item) : nullptr;
			const TArray<const FEwigfelsLookOption*> SlotOptions = C.GetOutfitOptions(Slot);
			const FName Replacement = DefItem ? DefItem->Id : (SlotOptions.Num() ? SlotOptions[0]->Id : NAME_None);
			if (Existing)
			{
				Fix(FString::Printf(TEXT("Kleidung '%s' passt nicht in Slot '%s' -> '%s'"), *Piece.Item.ToString(), *Slot.ToString(), *Replacement.ToString()));
			}
			Piece.Item = Replacement;
		}
		FixId(Piece.Pattern, Cat::Pattern, DefaultPattern, TEXT("Muster"), false);
		FixId(Piece.Fabric, Cat::Fabric, DefaultFabric, TEXT("Stoff"), false);
		FixAlpha(Piece.Color1);
		FixAlpha(Piece.Color2);
		FixAlpha(Piece.Color3);
		Clean.Add(Piece);
	}
	if (L.Outfit.Num() != Clean.Num())
	{
		if (L.Outfit.Num() > Clean.Num())
		{
			Fix(TEXT("Unbekannte/doppelte Kleidungs-Slots entfernt"));
		}
	}
	L.Outfit = MoveTemp(Clean);

	// Ringe: 1 … 4
	if (L.Rings.Num() == 0)
	{
		Fix(TEXT("Ring fehlte"));
		L.Rings = D.Rings.Num() ? D.Rings : TArray<FEwigfelsRingLook>{ FEwigfelsRingLook() };
	}
	if (L.Rings.Num() > LookMaxRings)
	{
		Fix(TEXT("Mehr als 4 Ringe"));
		L.Rings.SetNum(LookMaxRings);
	}
	const FName DefaultMetal = D.Rings.Num() ? D.Rings[0].Metal : NAME_None;
	for (FEwigfelsRingLook& Ring : L.Rings)
	{
		FixId(Ring.Metal, Cat::Metal, DefaultMetal, TEXT("Metall"), false);
		FixAlpha(Ring.Quartz);
	}
	return Fixes;
}

// ---------------------------------------------------------------------------
// Freischaltung, Läden, Preise

bool EwigfelsLook::IsUnlocked(const FEwigfelsLookOption& Option, const FEwigfelsLookContext& Context)
{
	if (Option.Unlock == TEXT("start"))
	{
		return true;
	}
	if (Option.Unlock == TEXT("shop"))
	{
		return !Context.IsCreation();
	}
	if (Option.Unlock.StartsWith(TEXT("flag:")))
	{
		return Context.HasFlag && Context.HasFlag(FName(*Option.Unlock.RightChop(5)));
	}
	return false;
}

TArray<FName> EwigfelsLook::GetChangedAreas(const FEwigfelsAppearance& O, const FEwigfelsAppearance& N)
{
	TArray<FName> Areas;
	if (O.Name != N.Name || O.Pronouns != N.Pronouns)
	{
		Areas.Add(Change::Name);
	}
	if (O.BodyType != N.BodyType || O.Height != N.Height || !LookSlidersEqual(O, N, true))
	{
		Areas.Add(Change::Body);
	}
	if (O.FaceA != N.FaceA || O.FaceB != N.FaceB || O.FaceBlend != N.FaceBlend || !LookSlidersEqual(O, N, false))
	{
		Areas.Add(Change::Face);
	}
	if (O.SkinColor != N.SkinColor)
	{
		Areas.Add(Change::Skin);
	}
	if (O.EyeColorL != N.EyeColorL || O.EyeColorR != N.EyeColorR)
	{
		Areas.Add(Change::Eyes);
	}
	if (O.HairStyle != N.HairStyle || O.HairColor != N.HairColor || O.HairTipColor != N.HairTipColor)
	{
		Areas.Add(Change::Hair);
	}
	if (O.Beard != N.Beard)
	{
		Areas.Add(Change::Beard);
	}
	if (O.Brows != N.Brows)
	{
		Areas.Add(Change::Brows);
	}
	if (O.Marks != N.Marks)
	{
		Areas.Add(Change::Marks);
	}
	if (O.Voice != N.Voice || O.VoicePitch != N.VoicePitch)
	{
		Areas.Add(Change::Voice);
	}
	for (const FEwigfelsOutfitPiece& NewPiece : N.Outfit)
	{
		const FEwigfelsOutfitPiece* OldPiece = O.FindPiece(NewPiece.Slot);
		if (!OldPiece || OldPiece->Item != NewPiece.Item)
		{
			Areas.Add(FName(*FString::Printf(TEXT("outfit:%s"), *NewPiece.Slot.ToString())));
		}
		else if (!OldPiece->SameColors(NewPiece))
		{
			Areas.Add(FName(*FString::Printf(TEXT("outfit_colors:%s"), *NewPiece.Slot.ToString())));
		}
	}
	if (O.Rings != N.Rings)
	{
		Areas.Add(Change::Rings);
	}
	return Areas;
}

bool EwigfelsLook::ShopAllows(const FEwigfelsLookShop& Shop, FName Area)
{
	if (Shop.Categories.Contains(Area))
	{
		return true;
	}
	FString AreaStr = Area.ToString();
	FString Kind, Slot;
	if (!AreaStr.Split(TEXT(":"), &Kind, &Slot))
	{
		return false;
	}
	if (Shop.Categories.Contains(Change::Outfit))
	{
		return true; // ganze Schneiderei: alle Slots, Stücke und Farben
	}
	if (Kind == TEXT("outfit_colors"))
	{
		return Shop.Categories.Contains(Change::OutfitColors) || Shop.Categories.Contains(FName(*(TEXT("outfit:") + Slot)));
	}
	return false;
}

bool EwigfelsLook::CheckChange(const FEwigfelsAppearance& Old, const FEwigfelsAppearance& New, const FEwigfelsLookCatalog& Catalog,
	const FEwigfelsLookContext& Context, int32& OutPrice, FText& OutReason)
{
	OutPrice = 0;
	const FEwigfelsLookShop* Shop = nullptr;

	if (Context.IsCreation())
	{
		if (Old.bCreated)
		{
			OutReason = LOCTEXT("AlreadyCreated", "Deine Figur steht schon. Änderungen gibt es jetzt in den Läden.");
			return false;
		}
		if (New.Name.TrimStartAndEnd().Len() < Catalog.NameMin)
		{
			OutReason = FText::Format(LOCTEXT("NameShort", "Der Name braucht mindestens {0} Zeichen."), Catalog.NameMin);
			return false;
		}
	}
	else
	{
		Shop = Catalog.Shops.Find(Context.ShopId);
		if (!Shop)
		{
			OutReason = FText::Format(LOCTEXT("NoShop", "Unbekannter Laden '{0}'."), FText::FromName(Context.ShopId));
			return false;
		}
		if (!Old.bCreated)
		{
			OutReason = LOCTEXT("NotCreated", "Erst die Figur erstellen.");
			return false;
		}
		const TArray<FName> Areas = GetChangedAreas(Old, New);
		for (const FName Area : Areas)
		{
			if (!ShopAllows(*Shop, Area))
			{
				OutReason = FText::Format(LOCTEXT("NotHere", "Das gibt es hier nicht ({0})."), FText::FromName(Area));
				return false;
			}
		}
		OutPrice = Shop->BasePrice * Areas.Num();
	}

	// Neu gewählte Optionen müssen freigeschaltet sein. Alte bleiben erlaubt (z. B. Uniform aus einer Quest).
	TArray<TPair<FName, FName>> OldChoices, NewChoices;
	if (!Context.IsCreation())
	{
		LookCollectChoices(Old, OldChoices);
	}
	LookCollectChoices(New, NewChoices);
	TSet<TPair<FName, FName>> Counted;
	for (const TPair<FName, FName>& Choice : NewChoices)
	{
		if (OldChoices.Contains(Choice) || Counted.Contains(Choice))
		{
			continue;
		}
		Counted.Add(Choice);
		const FEwigfelsLookOption* Option = Catalog.Find(Choice.Key, Choice.Value);
		if (!Option)
		{
			OutReason = FText::Format(LOCTEXT("Unknown", "Unbekannte Auswahl '{0}'."), FText::FromName(Choice.Value));
			return false;
		}
		if (!IsUnlocked(*Option, Context))
		{
			OutReason = FText::Format(LOCTEXT("Locked", "„{0}“ ist noch nicht freigeschaltet."), Option->DisplayName);
			return false;
		}
		if (!Context.IsCreation())
		{
			OutPrice += Option->Price;
		}
	}
	return true;
}

// ---------------------------------------------------------------------------
// Zufall, Look-Code, Hilfen

FString EwigfelsLook::RandomName(const FEwigfelsLookCatalog& Catalog, FRandomStream& Random, const FString& Avoid)
{
	TArray<const FString*> Pool;
	for (const FString& Name : Catalog.RandomNames)
	{
		if (Name != Avoid)
		{
			Pool.Add(&Name);
		}
	}
	return Pool.Num() ? CleanName(*Pool[Random.RandRange(0, Pool.Num() - 1)], Catalog.NameMax) : FString();
}

FEwigfelsAppearance EwigfelsLook::MakeRandom(const FEwigfelsLookCatalog& C, FRandomStream& R)
{
	FEwigfelsAppearance L = C.Defaults;
	const FEwigfelsLookContext StartOnly{}; // Erstellung, keine Flags

	auto Pick = [&](FName Category, FName Fallback) -> FName
	{
		TArray<FName> Ids;
		for (const FEwigfelsLookOption& O : C.GetOptions(Category))
		{
			if (IsUnlocked(O, StartOnly))
			{
				Ids.Add(O.Id);
			}
		}
		return Ids.Num() ? Ids[R.RandRange(0, Ids.Num() - 1)] : Fallback;
	};
	auto PickColor = [&](FName Palette, const FColor& Fallback) -> FColor
	{
		const TArray<FColor>* Colors = C.Palettes.Find(Palette);
		return Colors && Colors->Num() ? (*Colors)[R.RandRange(0, Colors->Num() - 1)] : Fallback;
	};

	L.Pronouns = static_cast<EEwigfelsPronouns>(R.RandRange(0, 2));
	L.BodyType = Pick(Cat::Body, L.BodyType);
	L.Height = FMath::RoundToFloat(R.FRandRange(C.HeightMin, C.HeightMax) * 1000.f) / 1000.f;
	// Fertige Köpfe: kein Mischen, Hautton meist passend zum Kopf
	L.FaceA = Pick(Cat::Face, L.FaceA);
	L.FaceB = L.FaceA;
	L.FaceBlend = 0.f;
	L.Sliders.Reset();
	for (const FEwigfelsSliderDef& S : C.Sliders)
	{
		if (R.FRand() < 0.35f)
		{
			L.SetSlider(S.Id, FMath::RoundToFloat(R.FRandRange(-0.6f, 0.6f) * 100.f) / 100.f);
		}
	}
	const FEwigfelsLookOption* Head = C.Find(Cat::Face, L.FaceA);
	L.SkinColor = Head && Head->bHasTint && R.FRand() < 0.8f ? Head->Tint : PickColor(TEXT("skin"), L.SkinColor);
	L.EyeColorL = PickColor(TEXT("eyes"), L.EyeColorL);
	L.EyeColorR = L.EyeColorL;
	L.HairStyle = Pick(Cat::Hair, L.HairStyle);
	L.HairColor = PickColor(TEXT("hair"), L.HairColor);
	L.HairTipColor = L.HairColor;
	L.Beard = R.FRand() < 0.7f ? C.Defaults.Beard : Pick(Cat::Beard, L.Beard);
	L.Brows = Pick(Cat::Brows, L.Brows);
	L.Voice = Pick(Cat::Voice, L.Voice);
	L.VoicePitch = 0.f;

	L.Marks.Reset();
	for (const FEwigfelsLookOption& O : C.GetOptions(Cat::Marks))
	{
		if (IsUnlocked(O, StartOnly) && R.FRand() < 0.12f && L.Marks.Num() < C.MaxMarks)
		{
			FEwigfelsMarkLook Mark;
			Mark.Id = O.Id;
			Mark.Color = O.Kind == TEXT("makeup") ? PickColor(TEXT("makeup"), Mark.Color) : Mark.Color;
			Mark.Opacity = FMath::RoundToFloat(R.FRandRange(0.4f, 1.f) * 100.f) / 100.f;
			L.Marks.Add(Mark);
		}
	}

	const FColor Dye1 = PickColor(TEXT("dye"), FColor(46, 58, 92));
	const FColor Dye2 = PickColor(TEXT("dye"), FColor(232, 225, 208));
	const FColor Dye3 = PickColor(TEXT("dye"), FColor(201, 180, 88));
	L.Outfit.Reset();
	for (const FName Slot : C.OutfitSlots)
	{
		TArray<FName> Items;
		for (const FEwigfelsLookOption* O : C.GetOutfitOptions(Slot))
		{
			if (IsUnlocked(*O, StartOnly))
			{
				Items.Add(O->Id);
			}
		}
		FEwigfelsOutfitPiece& Piece = L.FindOrAddPiece(Slot);
		Piece.Item = Items.Num() ? Items[R.RandRange(0, Items.Num() - 1)] : NAME_None;
		Piece.Color1 = Dye1;
		Piece.Color2 = Dye2;
		Piece.Color3 = Dye3;
		Piece.Pattern = Pick(Cat::Pattern, NAME_None);
		Piece.Fabric = Pick(Cat::Fabric, NAME_None);
	}
	L.Rings.SetNum(1);
	L.Rings[0].Metal = Pick(Cat::Metal, L.Rings[0].Metal);
	L.Rings[0].Quartz = PickColor(TEXT("quartz"), L.Rings[0].Quartz);

	Sanitize(L, C);
	return L;
}

FString EwigfelsLook::ToLookCode(const FEwigfelsAppearance& Look)
{
	FString Json;
	FJsonObjectConverter::UStructToJsonObjectString(Look, Json, 0, 0, 0, nullptr, false);
	const FTCHARToUTF8 Utf8(*Json);
	return TEXT("NOC1:") + FBase64::Encode(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
}

bool EwigfelsLook::FromLookCode(const FString& Code, FEwigfelsAppearance& OutLook)
{
	const FString Trimmed = Code.TrimStartAndEnd();
	if (!Trimmed.StartsWith(TEXT("NOC1:")))
	{
		return false;
	}
	TArray<uint8> Bytes;
	if (!FBase64::Decode(Trimmed.RightChop(5), Bytes) || Bytes.Num() == 0)
	{
		return false;
	}
	const FUTF8ToTCHAR Wide(reinterpret_cast<const ANSICHAR*>(Bytes.GetData()), Bytes.Num());
	const FString Json(Wide.Length(), Wide.Get());
	FEwigfelsAppearance Parsed;
	if (!FJsonObjectConverter::JsonObjectStringToUStruct(Json, &Parsed, 0, 0))
	{
		return false;
	}
	OutLook = Parsed;
	return true;
}

FString EwigfelsLook::ColorToHex(const FColor& Color)
{
	return FString::Printf(TEXT("#%02X%02X%02X"), Color.R, Color.G, Color.B);
}

bool EwigfelsLook::HexToColor(const FString& Hex, FColor& OutColor)
{
	if (Hex.Len() != 7 || Hex[0] != TEXT('#'))
	{
		return false;
	}
	for (int32 i = 1; i < 7; ++i)
	{
		if (!FChar::IsHexDigit(Hex[i]))
		{
			return false;
		}
	}
	OutColor = FColor::FromHex(Hex);
	OutColor.A = 255;
	return true;
}

FString EwigfelsLook::CleanName(const FString& Name, int32 MaxLen)
{
	FString Out;
	Out.Reserve(Name.Len());
	for (const TCHAR Ch : Name)
	{
		if (!FChar::IsControl(Ch) && Ch != TEXT('{') && Ch != TEXT('}') && Ch != TEXT('[') && Ch != TEXT(']'))
		{
			Out.AppendChar(Ch);
		}
	}
	Out.TrimStartAndEndInline();
	while (Out.Contains(TEXT("  ")))
	{
		Out.ReplaceInline(TEXT("  "), TEXT(" "));
	}
	Out.LeftInline(FMath::Max(1, MaxLen));
	Out.TrimEndInline();
	return Out;
}

#undef LOCTEXT_NAMESPACE
