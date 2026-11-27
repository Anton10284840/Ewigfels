// Copyright Ewigfels. Alle Rechte vorbehalten.
//
// Tests für den Charakter-Editor: Katalog, Reparieren, Laden-Regeln, Zufall, Look-Code, Server-Prüfung.
// Ausführen: Scripts/Tools/Test-Ewigfels.ps1 (Filter "Ewigfels.")

#include "Misc/AutomationTest.h"
#include "Appearance/EwigfelsAppearanceComponent.h"
#include "Appearance/EwigfelsAppearanceSubsystem.h"
#include "World/EwigfelsMirror.h"
#include "WorldState/EwigfelsWorldStateSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include <limits>

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	/** Eigene GameInstance mit Welt und allen Subsystemen (echte Daten aus Content/Data). */
	struct FLookTestGame
	{
		UGameInstance* GameInstance = nullptr;

		FLookTestGame()
		{
			GameInstance = NewObject<UGameInstance>(GEngine);
			GameInstance->AddToRoot();
			GameInstance->InitializeStandalone();
		}

		~FLookTestGame()
		{
			GameInstance->Shutdown();
			GameInstance->RemoveFromRoot();
		}

		UWorld* World() const { return GameInstance->GetWorld(); }
		UEwigfelsAppearanceSubsystem* Look() const { return GameInstance->GetSubsystem<UEwigfelsAppearanceSubsystem>(); }
		const FEwigfelsLookCatalog& Catalog() const { return Look()->GetCatalog(); }
	};

	/** Erste Option einer Kategorie mit diesem unlock-Wert. */
	const FEwigfelsLookOption* FirstWith(const FEwigfelsLookCatalog& Catalog, FName Category, const FString& Unlock)
	{
		for (const FEwigfelsLookOption& O : Catalog.GetOptions(Category))
		{
			if (O.Unlock == Unlock)
			{
				return &O;
			}
		}
		return nullptr;
	}

	FEwigfelsAppearance NamedDefault(const FEwigfelsLookCatalog& Catalog)
	{
		FEwigfelsAppearance L = Catalog.Defaults;
		L.Name = TEXT("Mira Testklang");
		EwigfelsLook::Sanitize(L, Catalog);
		return L;
	}

	FEwigfelsLookContext Ctx(FName Shop, TSet<FName> Flags = {})
	{
		FEwigfelsLookContext C;
		C.ShopId = Shop;
		C.HasFlag = [Flags](FName F) { return Flags.Contains(F); };
		return C;
	}
}

// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsLookCatalogTest, "Ewigfels.Look.Catalog",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsLookCatalogTest::RunTest(const FString& Parameters)
{
	FLookTestGame Game;
	if (!TestNotNull(TEXT("Look-Subsystem"), Game.Look()))
	{
		return false;
	}
	for (const FString& Error : Game.Look()->GetLoadErrors())
	{
		AddError(Error);
	}
	const FEwigfelsLookCatalog& Cat = Game.Catalog();
	TestEqual(TEXT("8 Grundgesichter"), Cat.GetOptions(EwigfelsLook::Cat::Face).Num(), 8);
	TestEqual(TEXT("10 Kleidungs-Slots"), Cat.OutfitSlots.Num(), 10);
	TestTrue(TEXT("mind. 30 Regler"), Cat.Sliders.Num() >= 30);
	TestTrue(TEXT("mind. 7 Läden"), Cat.Shops.Num() >= 7);
	TestTrue(TEXT("Hautpalette"), Cat.Palettes.Contains(TEXT("skin")) && Cat.Palettes[TEXT("skin")].Num() >= 8);

	FEwigfelsAppearance Defaults = Cat.Defaults;
	TArray<FString> Fixes;
	TestEqual(TEXT("Standard-Aussehen ist sauber"), EwigfelsLook::Sanitize(Defaults, Cat, &Fixes), 0);
	for (const FString& Fix : Fixes)
	{
		AddError(Fix);
	}
	for (const FName Slot : Cat.OutfitSlots)
	{
		TestNotNull(*FString::Printf(TEXT("Standard hat Slot %s"), *Slot.ToString()), Defaults.FindPiece(Slot));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsLookSanitizeTest, "Ewigfels.Look.Sanitize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsLookSanitizeTest::RunTest(const FString& Parameters)
{
	FLookTestGame Game;
	const FEwigfelsLookCatalog& Cat = Game.Catalog();

	FEwigfelsAppearance L = Cat.Defaults;
	L.Name = TEXT("   Ein  [viel] zu langer Name mit {Klammern}   ");
	L.Height = 3.f;
	L.FaceBlend = std::numeric_limits<float>::quiet_NaN();
	L.HairStyle = TEXT("gibt_es_nicht");
	L.Sliders = { FEwigfelsSliderValue(TEXT("nase_laenge"), 4.f), FEwigfelsSliderValue(TEXT("nase_laenge"), 0.2f),
		FEwigfelsSliderValue(TEXT("unbekannt"), 0.5f), FEwigfelsSliderValue(TEXT("kinn_laenge"), 0.f) };
	L.Marks.Reset();
	for (const FEwigfelsLookOption& O : Cat.GetOptions(EwigfelsLook::Cat::Marks))
	{
		FEwigfelsMarkLook M;
		M.Id = O.Id;
		M.Opacity = 7.f;
		L.Marks.Add(M);
	}
	FEwigfelsOutfitPiece Wing;
	Wing.Slot = TEXT("fluegel");
	Wing.Item = TEXT("x");
	L.Outfit.Add(Wing);
	L.FindOrAddPiece(TEXT("schuhe")).Item = TEXT("kopf_spitzhut"); // falscher Slot
	L.Rings.SetNum(6);

	TArray<FString> Fixes;
	TestTrue(TEXT("Korrekturen gemacht"), EwigfelsLook::Sanitize(L, Cat, &Fixes) > 0);
	TestTrue(TEXT("Name gekürzt"), L.Name.Len() <= Cat.NameMax && !L.Name.Contains(TEXT("[")) && !L.Name.Contains(TEXT("{")));
	TestEqual(TEXT("Größe begrenzt"), L.Height, Cat.HeightMax);
	TestEqual(TEXT("NaN repariert"), L.FaceBlend, 0.f);
	TestEqual(TEXT("Frisur auf Standard"), L.HairStyle, Cat.Defaults.HairStyle);
	TestEqual(TEXT("ein Regler übrig"), L.Sliders.Num(), 1);
	TestEqual(TEXT("Regler begrenzt"), L.GetSlider(TEXT("nase_laenge")), 1.f);
	TestEqual(TEXT("Male begrenzt"), L.Marks.Num(), Cat.MaxMarks);
	TestEqual(TEXT("Deckkraft begrenzt"), L.Marks[0].Opacity, 1.f);
	TestEqual(TEXT("genau 10 Slots"), L.Outfit.Num(), Cat.OutfitSlots.Num());
	TestNotEqual(TEXT("Hut nicht an den Füßen"), L.FindPiece(TEXT("schuhe"))->Item, FName(TEXT("kopf_spitzhut")));
	TestEqual(TEXT("max. 4 Ringe"), L.Rings.Num(), 4);

	TestEqual(TEXT("zweiter Durchlauf ohne Korrektur"), EwigfelsLook::Sanitize(L, Cat), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsLookRulesTest, "Ewigfels.Look.Rules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsLookRulesTest::RunTest(const FString& Parameters)
{
	FLookTestGame Game;
	const FEwigfelsLookCatalog& Cat = Game.Catalog();
	using namespace EwigfelsLook;

	const FEwigfelsAppearance Fresh = Cat.Defaults; // bCreated = false
	FEwigfelsAppearance New = NamedDefault(Cat);
	int32 Price = 0;
	FText Reason;

	// --- Erstellung ---
	TestTrue(TEXT("Erstellung mit Standard"), CheckChange(Fresh, New, Cat, Ctx(NAME_None), Price, Reason));
	TestEqual(TEXT("Erstellung kostet nichts"), Price, 0);

	FEwigfelsAppearance NoName = New;
	NoName.Name = TEXT("A");
	TestFalse(TEXT("Name zu kurz"), CheckChange(Fresh, NoName, Cat, Ctx(NAME_None), Price, Reason));

	const FEwigfelsLookOption* ShopHair = FirstWith(Cat, Cat::Hair, TEXT("shop"));
	if (TestNotNull(TEXT("Laden-Frisur im Katalog"), ShopHair))
	{
		FEwigfelsAppearance WithShopHair = New;
		WithShopHair.HairStyle = ShopHair->Id;
		TestFalse(TEXT("Laden-Frisur nicht bei Erstellung"), CheckChange(Fresh, WithShopHair, Cat, Ctx(NAME_None), Price, Reason));
	}

	const FEwigfelsLookOption* Uniform = Cat.Find(Cat::Outfit, TEXT("ober_uniform"));
	if (TestNotNull(TEXT("Uniform im Katalog"), Uniform))
	{
		FEwigfelsAppearance WithUniform = New;
		WithUniform.FindOrAddPiece(TEXT("oberkoerper")).Item = Uniform->Id;
		TestFalse(TEXT("Uniform ohne Flag gesperrt"), CheckChange(Fresh, WithUniform, Cat, Ctx(NAME_None), Price, Reason));
		TestTrue(TEXT("Uniform mit Flag frei"), CheckChange(Fresh, WithUniform, Cat, Ctx(NAME_None, { TEXT("Look_Uniform") }), Price, Reason));
	}

	// --- Läden ---
	FEwigfelsAppearance Created = New;
	Created.bCreated = true;
	TestFalse(TEXT("zweite Erstellung verboten"), CheckChange(Created, Created, Cat, Ctx(NAME_None), Price, Reason));

	if (ShopHair)
	{
		FEwigfelsAppearance Haircut = Created;
		Haircut.HairStyle = ShopHair->Id;
		TestTrue(TEXT("Haarstube darf Frisur"), CheckChange(Created, Haircut, Cat, Ctx(TEXT("haarstube")), Price, Reason));
		TestEqual(TEXT("Preis = Grundpreis + Frisur"), Price, Cat.Shops[TEXT("haarstube")].BasePrice + ShopHair->Price);
		TestFalse(TEXT("Schneiderei darf keine Frisur"), CheckChange(Created, Haircut, Cat, Ctx(TEXT("schneiderei")), Price, Reason));
	}

	FEwigfelsAppearance NewCoat = Created;
	NewCoat.FindOrAddPiece(TEXT("umhang")).Item = TEXT("umhang_mantel_lang");
	TestTrue(TEXT("Schneiderei: neuer Mantel"), CheckChange(Created, NewCoat, Cat, Ctx(TEXT("schneiderei")), Price, Reason));
	TestFalse(TEXT("Handschuhmacher: kein Mantel"), CheckChange(Created, NewCoat, Cat, Ctx(TEXT("handschuhmacher")), Price, Reason));
	TestFalse(TEXT("Turmzimmer: kein neues Stück"), CheckChange(Created, NewCoat, Cat, Ctx(TEXT("turmzimmer")), Price, Reason));

	FEwigfelsAppearance Recolor = Created;
	Recolor.FindOrAddPiece(TEXT("umhang")).Color1 = FColor(107, 30, 42);
	TestTrue(TEXT("Turmzimmer: umfärben"), CheckChange(Created, Recolor, Cat, Ctx(TEXT("turmzimmer")), Price, Reason));
	TestEqual(TEXT("Umfärben im Turm gratis"), Price, 0);
	TestFalse(TEXT("Handschuhmacher: Umhang umfärben nicht"), CheckChange(Created, Recolor, Cat, Ctx(TEXT("handschuhmacher")), Price, Reason));

	FEwigfelsAppearance NewFace = Created;
	NewFace.SetSlider(TEXT("nase_laenge"), 0.5f);
	TestFalse(TEXT("Barbier ändert kein Gesicht"), CheckChange(Created, NewFace, Cat, Ctx(TEXT("haarstube")), Price, Reason));
	TestTrue(TEXT("Spiegelhexe ändert Gesicht"), CheckChange(Created, NewFace, Cat, Ctx(TEXT("spiegelhexe")), Price, Reason));

	FEwigfelsAppearance Taller = Created;
	Taller.Height = 1.03f;
	const TArray<FName> Areas = GetChangedAreas(Created, Taller);
	TestTrue(TEXT("Größe = Bereich Körper"), Areas.Num() == 1 && Areas[0] == Change::Body);

	TestFalse(TEXT("unbekannter Laden"), CheckChange(Created, Recolor, Cat, Ctx(TEXT("gibtsnicht")), Price, Reason));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsLookRandomTest, "Ewigfels.Look.RandomAndCode",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsLookRandomTest::RunTest(const FString& Parameters)
{
	FLookTestGame Game;
	const FEwigfelsLookCatalog& Cat = Game.Catalog();
	const FEwigfelsAppearance Fresh = Cat.Defaults;

	for (int32 Seed = 1; Seed <= 50; ++Seed)
	{
		FRandomStream Random(Seed);
		FEwigfelsAppearance L = EwigfelsLook::MakeRandom(Cat, Random);
		L.Name = FString::Printf(TEXT("Zufall %d"), Seed);
		FEwigfelsAppearance Copy = L;
		TArray<FString> Fixes;
		if (EwigfelsLook::Sanitize(Copy, Cat, &Fixes) > 0)
		{
			AddError(FString::Printf(TEXT("Zufall %d braucht Korrektur: %s"), Seed, *FString::Join(Fixes, TEXT("; "))));
		}
		int32 Price = 0;
		FText Reason;
		if (!EwigfelsLook::CheckChange(Fresh, L, Cat, Ctx(NAME_None), Price, Reason))
		{
			AddError(FString::Printf(TEXT("Zufall %d nicht erlaubt: %s"), Seed, *Reason.ToString()));
		}

		// Look-Code hin und zurück
		const FString Code = EwigfelsLook::ToLookCode(L);
		FEwigfelsAppearance Back;
		if (!TestTrue(TEXT("Code lesbar"), EwigfelsLook::FromLookCode(Code, Back)))
		{
			return false;
		}
		if (Back != L)
		{
			AddError(FString::Printf(TEXT("Look-Code %d verändert das Aussehen"), Seed));
		}
	}

	FEwigfelsAppearance Dummy;
	TestFalse(TEXT("Müll wird abgelehnt"), EwigfelsLook::FromLookCode(TEXT("NOC1:%%%"), Dummy));
	TestFalse(TEXT("ohne Präfix abgelehnt"), EwigfelsLook::FromLookCode(TEXT("hallo"), Dummy));

	FColor Color;
	TestTrue(TEXT("Hex lesen"), EwigfelsLook::HexToColor(TEXT("#C08560"), Color) && Color == FColor(192, 133, 96));
	TestEqual(TEXT("Hex schreiben"), EwigfelsLook::ColorToHex(Color), FString(TEXT("#C08560")));
	TestFalse(TEXT("falsches Hex"), EwigfelsLook::HexToColor(TEXT("#C0856"), Color));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsLookServerTest, "Ewigfels.Look.ServerChecks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsLookServerTest::RunTest(const FString& Parameters)
{
	FLookTestGame Game;
	UWorld* World = Game.World();
	UEwigfelsAppearanceSubsystem* Sub = Game.Look();
	const FEwigfelsLookCatalog& Cat = Game.Catalog();

	AActor* Actor = World->SpawnActor<AActor>();
	UEwigfelsAppearanceComponent* Look = NewObject<UEwigfelsAppearanceComponent>(Actor, TEXT("Appearance"));
	Look->RegisterComponent();

	bool bLastAccepted = false;
	int32 LastPrice = -1;
	FText LastReason;
	auto Request = [&](const FEwigfelsAppearance& NewLook, FName Shop)
	{
		Look->RequestChange(NewLook, Shop); // Standalone: Server-RPC und Client-Antwort laufen sofort lokal
		bLastAccepted = Look->GetLastResult(LastPrice, LastReason);
	};

	// Ohne Namen keine Figur
	FEwigfelsAppearance L = Cat.Defaults;
	L.Name = TEXT("");
	Request(L, NAME_None);
	TestFalse(TEXT("ohne Namen abgelehnt"), Look->IsCreated());

	// Erstellen (Client schickt bCreated = true und Unsinn – Server bereinigt)
	L.Name = TEXT("  Juna  ");
	L.Height = 9.f;
	L.bCreated = true;
	Request(L, NAME_None);
	TestTrue(TEXT("erstellt"), Look->IsCreated() && bLastAccepted);
	TestEqual(TEXT("Name bereinigt"), Look->GetAppearance().Name, FString(TEXT("Juna")));
	TestEqual(TEXT("Größe begrenzt"), Look->GetAppearance().Height, Cat.HeightMax);
	TestTrue(TEXT("Flag Look_Created"), Game.GameInstance->GetSubsystem<UEwigfelsWorldStateSubsystem>()->HasFlag(TEXT("Look_Created")));

	// Zweite Erstellung abgelehnt
	FEwigfelsAppearance Again = Look->GetAppearance();
	Again.SkinColor = FColor(67, 39, 25);
	Request(Again, NAME_None);
	TestFalse(TEXT("zweite Erstellung abgelehnt"), bLastAccepted);
	TestNotEqual(TEXT("Haut unverändert"), Look->GetAppearance().SkinColor, FColor(67, 39, 25));

	// Laden ohne Spiegel in der Nähe → abgelehnt
	const FEwigfelsLookOption* ShopHair = FirstWith(Cat, EwigfelsLook::Cat::Hair, TEXT("shop"));
	if (!TestNotNull(TEXT("Laden-Frisur"), ShopHair))
	{
		return false;
	}
	FEwigfelsAppearance Haircut = Look->GetAppearance();
	Haircut.HairStyle = ShopHair->Id;
	Request(Haircut, TEXT("haarstube"));
	TestFalse(TEXT("ohne Spiegel abgelehnt"), bLastAccepted);

	// Mit Spiegel → angenommen, Preis stimmt
	AEwigfelsMirror* Mirror = World->SpawnActor<AEwigfelsMirror>(AEwigfelsMirror::StaticClass(), Actor->GetActorLocation() + FVector(150.f, 0.f, 0.f), FRotator::ZeroRotator);
	Mirror->ShopId = TEXT("haarstube");
	Request(Haircut, TEXT("haarstube"));
	TestTrue(TEXT("mit Spiegel angenommen"), bLastAccepted);
	TestEqual(TEXT("Frisur gesetzt"), Look->GetAppearance().HairStyle, ShopHair->Id);
	TestEqual(TEXT("Preis"), LastPrice, Cat.Shops[TEXT("haarstube")].BasePrice + ShopHair->Price);

	// Barbier darf keine Kleidung
	FEwigfelsAppearance Coat = Look->GetAppearance();
	Coat.FindOrAddPiece(TEXT("umhang")).Item = TEXT("umhang_mantel_lang");
	Request(Coat, TEXT("haarstube"));
	TestFalse(TEXT("Barbier verkauft keine Mäntel"), bLastAccepted);

	// Bezahlen schlägt fehl → nichts ändert sich
	Sub->PaymentHandler = [](APawn*, const FEwigfelsLookShop&, int32) { return false; };
	FEwigfelsAppearance Color = Look->GetAppearance();
	Color.HairColor = FColor(168, 64, 42);
	Request(Color, TEXT("haarstube"));
	TestFalse(TEXT("ohne Geld abgelehnt"), bLastAccepted);
	TestNotEqual(TEXT("Haarfarbe unverändert"), Look->GetAppearance().HairColor, FColor(168, 64, 42));
	Sub->PaymentHandler = nullptr;

	Actor->Destroy();
	Mirror->Destroy();
	return true;
}

#endif
