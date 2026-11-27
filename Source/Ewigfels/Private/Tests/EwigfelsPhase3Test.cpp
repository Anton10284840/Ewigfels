// Copyright Ewigfels. Alle Rechte vorbehalten.
//
// Phase-3-Tests: Zauberdaten, Kampfrechnung, Status-Effekte, Angriffsplätze, Dialog-Kamera.
// Ausführen: Scripts/Tools/Test-Ewigfels.ps1 (Filter "Ewigfels.")

#include "Misc/AutomationTest.h"
#include "AI/EwigfelsCreatureTypes.h"
#include "Camera/EwigfelsDialogueCameraComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Combat/EwigfelsCombatDirector.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Magic/EwigfelsSpellEffects.h"
#include "Magic/EwigfelsSpellSubsystem.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	/** Eigene GameInstance mit Welt und allen Subsystemen (echte Daten aus Content/Data). */
	struct FPhase3TestGame
	{
		UGameInstance* GameInstance = nullptr;

		FPhase3TestGame()
		{
			GameInstance = NewObject<UGameInstance>(GEngine);
			GameInstance->AddToRoot();
			GameInstance->InitializeStandalone();
		}

		~FPhase3TestGame()
		{
			GameInstance->Shutdown();
			GameInstance->RemoveFromRoot();
		}

		UWorld* World() const { return GameInstance->GetWorld(); }
		UEwigfelsSpellSubsystem* Spells() const { return GameInstance->GetSubsystem<UEwigfelsSpellSubsystem>(); }
		UEwigfelsCreatureSubsystem* Creatures() const { return GameInstance->GetSubsystem<UEwigfelsCreatureSubsystem>(); }

		/** Leerer Actor mit Kampfkomponente (Server-Rolle). */
		UEwigfelsCombatComponent* SpawnFighter(EEwigfelsTeam Team, float Health = 100.f)
		{
			AActor* Actor = World()->SpawnActor<AActor>();
			UEwigfelsCombatComponent* Combat = NewObject<UEwigfelsCombatComponent>(Actor, TEXT("Combat"));
			Combat->SetTeam(Team);
			Combat->RegisterComponent();
			Actor->AddInstanceComponent(Combat);
			Combat->SetMaxHealth(Health, true);
			return Combat;
		}
	};
}

// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsSpellParseTest, "Ewigfels.Magic.ParseSpellFile",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsSpellParseTest::RunTest(const FString& Parameters)
{
	const FString Json = TEXT(R"({
		"spells": [
			{ "id": "probe", "name": { "de": "Probe", "en": "Probe" }, "school": "Schlag", "chord": ["Glut", "Strom", "Strom"],
			  "catalog_id": "K99", "cost": 10, "cooldown": 2, "cast_time": 0.2, "hands": "two",
			  "delivery": { "type": "projectile", "speed": 1000, "chain": 3, "homing": 4 },
			  "effects": [ { "type": "damage", "amount": 5, "bonus": { "if_status": "frozen", "mult": 2, "consume": true } },
			               { "type": "status", "status": "burning" } ],
			  "learn": { "how": "meister", "who": "Grell" }, "vfx": { "color": "#FF8000" } },
			{ "id": "kaputt", "delivery": { "type": "teleport" } },
			{ "id": "ohne_status", "delivery": { "type": "self" }, "effects": [ { "type": "status" } ] }
		],
		"statuses": [ { "id": "burning", "duration": 4, "damage_per_second": 5, "cancels": ["frozen"] } ],
		"loadout": ["probe"]
	})");
	TArray<FEwigfelsSpellDef> Spells;
	TArray<FEwigfelsStatusDef> Statuses;
	TArray<FName> Loadout;
	TArray<FString> Errors;
	TestTrue(TEXT("JSON gelesen"), EwigfelsSpells::ParseSpellFile(Json, TEXT("test.json"), Spells, Statuses, Loadout, Errors));

	TestEqual(TEXT("eine gültige Weise"), Spells.Num(), 1);
	TestEqual(TEXT("zwei Fehler (unbekannte Auslieferung, Status ohne Namen)"), Errors.Num(), 2);
	if (Spells.Num() == 1)
	{
		const FEwigfelsSpellDef& S = Spells[0];
		TestTrue(TEXT("Id"), S.Id == FName(TEXT("probe")));
		TestTrue(TEXT("Katalog-Id"), S.CatalogId == FName(TEXT("K99")));
		TestEqual(TEXT("Akkord"), S.Chord.Num(), 3);
		TestTrue(TEXT("zweihändig"), S.bTwoHanded);
		TestEqual(TEXT("Kette"), S.Delivery.Chain, 3);
		TestEqual(TEXT("Zielsuche begrenzt auf 1"), S.Delivery.Homing, 1.f);
		TestEqual(TEXT("Effekte"), S.Effects.Num(), 2);
		TestEqual(TEXT("Bonus-Faktor"), S.Effects[0].Bonus.Multiplier, 2.f);
		TestTrue(TEXT("unbekanntes Feld 'learn' bleibt erhalten"), S.Extra.Contains(TEXT("learn")));
		TestTrue(TEXT("Farbe gelesen"), S.Visual.Color.R > 0.9f && S.Visual.Color.B < 0.05f);
	}
	TestEqual(TEXT("Status"), Statuses.Num(), 1);
	TestEqual(TEXT("Loadout"), Loadout.Num(), 1);

	FLinearColor C;
	TestFalse(TEXT("falsche Farbe erkannt"), EwigfelsSpells::ParseHexColor(TEXT("#12345"), C));
	return true;
}

// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsSpellDataTest, "Ewigfels.Magic.RealData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsSpellDataTest::RunTest(const FString& Parameters)
{
	FPhase3TestGame Game;
	const UEwigfelsSpellSubsystem* Spells = Game.Spells();
	if (!TestNotNull(TEXT("Weisen-Subsystem"), Spells))
	{
		return false;
	}
	for (const FString& Error : Spells->GetLoadErrors())
	{
		AddError(Error);
	}
	TestEqual(TEXT("Startbelegung = 5 Weisen"), Spells->GetDefaultLoadout().Num(), 5);
	for (const FName Id : Spells->GetDefaultLoadout())
	{
		TestNotNull(*FString::Printf(TEXT("Weise %s existiert"), *Id.ToString()), Spells->FindSpell(Id));
	}
	for (const FName Id : Spells->GetAllSpellIds())
	{
		const FEwigfelsSpellDef* Def = Spells->FindSpell(Id);
		for (const FEwigfelsSpellEffect& E : Def->Effects)
		{
			if (E.Type == EEwigfelsSpellEffectType::Status)
			{
				TestNotNull(*FString::Printf(TEXT("%s: Status %s definiert"), *Id.ToString(), *E.Status.ToString()), Spells->FindStatus(E.Status));
			}
		}
		if (!Def->CounterSpell.IsNone())
		{
			TestNotNull(*FString::Printf(TEXT("%s: Konter existiert"), *Id.ToString()), Spells->FindSpell(Def->CounterSpell));
		}
	}
	// Gegner greifen mit existierenden Weisen an
	const UEwigfelsCreatureSubsystem* Creatures = Game.Creatures();
	if (TestNotNull(TEXT("Kreaturen-Subsystem"), Creatures))
	{
		TestTrue(TEXT("Novize und Fluchsänger geladen"),
			Creatures->FindCreature(TEXT("Novize")) != nullptr && Creatures->FindCreature(TEXT("Fluchsaenger")) != nullptr);
		for (const FName Id : Creatures->GetAllCreatureIds())
		{
			for (const FName SpellId : Creatures->FindCreature(Id)->Spells)
			{
				TestNotNull(*FString::Printf(TEXT("%s greift mit %s an"), *Id.ToString(), *SpellId.ToString()), Spells->FindSpell(SpellId));
			}
		}
	}
	return true;
}

// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsCombatMathTest, "Ewigfels.Combat.Math",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsCombatMathTest::RunTest(const FString& Parameters)
{
	// Kegel (Hammerton): 650 cm, 70°
	const FVector O = FVector::ZeroVector;
	const FVector F = FVector::ForwardVector;
	TestTrue(TEXT("gerade vorne"), EwigfelsSpellEffects::IsInCone(O, F, 650.f, 70.f, FVector(400.f, 0.f, 0.f)));
	TestTrue(TEXT("30° seitlich"), EwigfelsSpellEffects::IsInCone(O, F, 650.f, 70.f, FVector(400.f, 230.f, 0.f)));
	TestFalse(TEXT("45° seitlich"), EwigfelsSpellEffects::IsInCone(O, F, 650.f, 70.f, FVector(400.f, 400.f, 0.f)));
	TestFalse(TEXT("hinten"), EwigfelsSpellEffects::IsInCone(O, F, 650.f, 70.f, FVector(-200.f, 0.f, 0.f)));
	TestFalse(TEXT("zu weit"), EwigfelsSpellEffects::IsInCone(O, F, 650.f, 70.f, FVector(700.f, 0.f, 0.f)));

	// Funkenstoß-Kette: 3 Glieder, Fenster 0,7 s
	FEwigfelsSpellDef Spark;
	Spark.Id = TEXT("funkenstoss");
	Spark.Delivery.Chain = 3;
	Spark.Delivery.ChainWindow = 0.7f;
	TestEqual(TEXT("erster Schuss"), (int32)UEwigfelsSpellCasterComponent::NextChainIndex(Spark, NAME_None, 0, 0.f, 1.f), 0);
	TestEqual(TEXT("zweiter Schuss"), (int32)UEwigfelsSpellCasterComponent::NextChainIndex(Spark, Spark.Id, 0, 1.f, 1.4f), 1);
	TestEqual(TEXT("dritter Schuss"), (int32)UEwigfelsSpellCasterComponent::NextChainIndex(Spark, Spark.Id, 1, 1.4f, 1.8f), 2);
	TestEqual(TEXT("nach dem dritten neu"), (int32)UEwigfelsSpellCasterComponent::NextChainIndex(Spark, Spark.Id, 2, 1.8f, 2.1f), 0);
	TestEqual(TEXT("zu langsam = neu"), (int32)UEwigfelsSpellCasterComponent::NextChainIndex(Spark, Spark.Id, 1, 1.f, 2.f), 0);

	// Parierfenster 0,2 s
	TestTrue(TEXT("Parade rechtzeitig"), UEwigfelsSpellCasterComponent::IsWithinParryWindow(10.f, 10.15f, 0.2f));
	TestFalse(TEXT("Parade zu spät"), UEwigfelsSpellCasterComponent::IsWithinParryWindow(10.f, 10.3f, 0.2f));

	// Schaden ohne Ziel-Status: Grundwert
	FEwigfelsSpellEffect Hammer;
	Hammer.Amount = 26.f;
	Hammer.Bonus.IfStatus = TEXT("frozen");
	Hammer.Bonus.Multiplier = 2.5f;
	Hammer.Bonus.bConsume = true;
	bool bConsume = true;
	TestEqual(TEXT("ohne Ziel"), EwigfelsSpellEffects::ComputeDamage(Hammer, nullptr, bConsume), 26.f);
	TestFalse(TEXT("nichts verbraucht"), bConsume);
	return true;
}

// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsCombatStatusTest, "Ewigfels.Combat.StatusAndDamage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsCombatStatusTest::RunTest(const FString& Parameters)
{
	FPhase3TestGame Game;
	UEwigfelsCombatComponent* Target = Game.SpawnFighter(EEwigfelsTeam::Enemy, 100.f);
	UEwigfelsCombatComponent* Player = Game.SpawnFighter(EEwigfelsTeam::Player, 120.f);
	if (!TestNotNull(TEXT("Ziel"), Target) || !TestNotNull(TEXT("Spieler"), Player))
	{
		return false;
	}
	TestTrue(TEXT("Spieler und Gegner sind Feinde"), UEwigfelsCombatComponent::AreHostile(Player->GetOwner(), Target->GetOwner()));

	// 3× Unterkühlt → Gefroren (Status.json: escalate_at 3)
	Target->ApplyStatus(TEXT("chilled"), 0.f, Player->GetOwner());
	Target->ApplyStatus(TEXT("chilled"), 0.f, Player->GetOwner());
	TestEqual(TEXT("2 Stapel"), Target->GetStatusStacks(TEXT("chilled")), 2);
	TestTrue(TEXT("verlangsamt"), Target->GetMoveSpeedMultiplier() < 1.f);
	Target->ApplyStatus(TEXT("chilled"), 0.f, Player->GetOwner());
	TestTrue(TEXT("gefroren"), Target->HasStatus(TEXT("frozen")));
	TestFalse(TEXT("unterkühlt weg"), Target->HasStatus(TEXT("chilled")));
	TestFalse(TEXT("gefroren kann nicht handeln"), Target->CanAct());

	// Hammerton gegen Gefroren: ×2,5 und zerbricht das Eis
	FEwigfelsSpellEffect Hammer;
	Hammer.Amount = 20.f;
	Hammer.Bonus.IfStatus = TEXT("frozen");
	Hammer.Bonus.Multiplier = 2.5f;
	Hammer.Bonus.bConsume = true;
	bool bConsume = false;
	TestEqual(TEXT("Bonus-Schaden"), EwigfelsSpellEffects::ComputeDamage(Hammer, Target, bConsume), 50.f);
	TestTrue(TEXT("Eis wird verbraucht"), bConsume);

	// Glut taut Eis (cancels) und brennt
	Target->ApplyStatus(TEXT("burning"), 0.f, Player->GetOwner());
	TestFalse(TEXT("Feuer taut Eis"), Target->HasStatus(TEXT("frozen")));
	TestTrue(TEXT("brennt"), Target->HasStatus(TEXT("burning")));
	TestTrue(TEXT("kann wieder handeln"), Target->CanAct());

	// Schaden, Element-Faktor, Tod
	Target->ElementMultipliers.Add(TEXT("Glut"), 1.5f);
	FEwigfelsDamageInfo Info;
	Info.Amount = 20.f;
	Info.Element = TEXT("Glut");
	Info.Instigator = Player->GetOwner();
	TestEqual(TEXT("Treffer"), static_cast<int32>(Target->ApplyDamage(Info)), static_cast<int32>(EEwigfelsHitResult::Hit));
	TestEqual(TEXT("Glut-Schwäche ×1,5"), Target->GetHealth(), 70.f);

	Target->SetInvulnerableFor(1.f);
	TestEqual(TEXT("unverwundbar beim Ausweichen"), static_cast<int32>(Target->ApplyDamage(Info)), static_cast<int32>(EEwigfelsHitResult::Ignored));
	Target->ResetCombatState();
	TestEqual(TEXT("Reset füllt Leben"), Target->GetHealth(), 100.f);

	Info.Amount = 500.f;
	Info.Element = NAME_None;
	TestEqual(TEXT("tödlich"), static_cast<int32>(Target->ApplyDamage(Info)), static_cast<int32>(EEwigfelsHitResult::Killed));
	TestTrue(TEXT("tot"), Target->IsDead());
	TestEqual(TEXT("Tote nehmen keinen Schaden"), static_cast<int32>(Target->ApplyDamage(Info)), static_cast<int32>(EEwigfelsHitResult::Ignored));

	// Angriffsplätze: max. 2 Nahkämpfer pro Ziel
	UEwigfelsCombatDirector* Director = Game.World()->GetSubsystem<UEwigfelsCombatDirector>();
	if (TestNotNull(TEXT("Kampf-Koordinator"), Director))
	{
		UEwigfelsCombatComponent* A = Game.SpawnFighter(EEwigfelsTeam::Enemy);
		UEwigfelsCombatComponent* B = Game.SpawnFighter(EEwigfelsTeam::Enemy);
		UEwigfelsCombatComponent* C = Game.SpawnFighter(EEwigfelsTeam::Enemy);
		AActor* P = Player->GetOwner();
		TestTrue(TEXT("1. Platz"), Director->RequestMeleeToken(A->GetOwner(), P));
		TestTrue(TEXT("2. Platz"), Director->RequestMeleeToken(B->GetOwner(), P));
		TestFalse(TEXT("3. muss warten"), Director->RequestMeleeToken(C->GetOwner(), P));
		Director->ReleaseMeleeToken(A->GetOwner());
		TestTrue(TEXT("frei geworden"), Director->RequestMeleeToken(C->GetOwner(), P));
	}
	return true;
}

// ---------------------------------------------------------------------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsDialogueCameraTest, "Ewigfels.Camera.DialogueShots",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsDialogueCameraTest::RunTest(const FString& Parameters)
{
	const FVector Player(0.f, 0.f, 160.f);
	const FVector Npc(200.f, 0.f, 160.f);
	float Focal = 0.f;

	// Über die Schulter des Spielers auf den NPC
	const FTransform Ots = UEwigfelsDialogueCameraComponent::ComputeShot(EEwigfelsDialogueShot::OverShoulder, Player, Npc, 1.f, Focal);
	TestEqual(TEXT("Schulter = 40 mm"), Focal, 40.f);
	TestTrue(TEXT("Kamera hinter dem Spieler"), Ots.GetLocation().X < Player.X);
	TestTrue(TEXT("blickt zum NPC"), FVector::DotProduct(Ots.GetRotation().GetForwardVector(), (Npc - Ots.GetLocation()).GetSafeNormal()) > 0.99f);

	// Gegenschuss: gleiche Seite der Achse (180°-Regel). Achse Spieler→NPC = +X, Seite = +Y bei SideSign 1
	const FTransform Reverse = UEwigfelsDialogueCameraComponent::ComputeShot(EEwigfelsDialogueShot::OverShoulder, Npc, Player, -1.f, Focal);
	TestTrue(TEXT("180°-Regel: beide Kameras auf derselben Seite"), FMath::Sign(Ots.GetLocation().Y) == FMath::Sign(Reverse.GetLocation().Y));

	UEwigfelsDialogueCameraComponent::ComputeShot(EEwigfelsDialogueShot::CloseUp, Player, Npc, 1.f, Focal);
	TestEqual(TEXT("Close-up = 85 mm"), Focal, 85.f);
	UEwigfelsDialogueCameraComponent::ComputeShot(EEwigfelsDialogueShot::Wide, Player, Npc, 1.f, Focal);
	TestEqual(TEXT("Totale = 24 mm"), Focal, 24.f);

	FEwigfelsDialogueLine Line;
	TestEqual(TEXT("erste Zeile = Totale"), static_cast<int32>(UEwigfelsDialogueCameraComponent::ChooseShot(Line, true)), static_cast<int32>(EEwigfelsDialogueShot::Wide));
	Line.Intensity = 0.85f;
	TestEqual(TEXT("starke Emotion = Close-up"), static_cast<int32>(UEwigfelsDialogueCameraComponent::ChooseShot(Line, false)), static_cast<int32>(EEwigfelsDialogueShot::CloseUp));
	Line.Camera = TEXT("wide");
	TestEqual(TEXT("Hinweis im Dialog gewinnt"), static_cast<int32>(UEwigfelsDialogueCameraComponent::ChooseShot(Line, false)), static_cast<int32>(EEwigfelsDialogueShot::Wide));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
