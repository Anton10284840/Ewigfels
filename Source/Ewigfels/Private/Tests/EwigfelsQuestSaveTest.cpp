// Copyright Ewigfels. Alle Rechte vorbehalten.
//
// Tests für Questablauf und Speichern/Laden mit echten Subsystemen (eigene Test-GameInstance).

#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/EwigfelsQuestSubsystem.h"
#include "Save/EwigfelsSaveGame.h"
#include "Save/EwigfelsSaveSubsystem.h"
#include "WorldState/EwigfelsWorldStateSubsystem.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	/** Eigene GameInstance mit allen Subsystemen, wird am Ende wieder abgebaut. */
	struct FTestGame
	{
		UGameInstance* GameInstance = nullptr;

		FTestGame()
		{
			GameInstance = NewObject<UGameInstance>(GEngine);
			GameInstance->AddToRoot();
			GameInstance->InitializeStandalone();
			// Kein Autosave während des Tests
			Save()->SetAutosaveInterval(0.f);
			// Echte Quest-Daten raus, damit auto_start-Quests (M101) den Test nicht stören
			Quests()->ResetProgress();
			State()->ReplaceData(FEwigfelsWorldStateData());
		}

		~FTestGame()
		{
			GameInstance->Shutdown();
			GameInstance->RemoveFromRoot();
		}

		UEwigfelsWorldStateSubsystem* State() const { return GameInstance->GetSubsystem<UEwigfelsWorldStateSubsystem>(); }
		UEwigfelsQuestSubsystem* Quests() const { return GameInstance->GetSubsystem<UEwigfelsQuestSubsystem>(); }
		UEwigfelsSaveSubsystem* Save() const { return GameInstance->GetSubsystem<UEwigfelsSaveSubsystem>(); }
	};

	const TCHAR* TestQuestJson = TEXT(R"JSON(
{
  "id": "T_Haupt", "type": "side", "duration_min": 10,
  "title": { "de": "Test", "en": "Test" },
  "stages": [
    { "id": "s1", "text": "Sprich mit Edda",
      "objectives": [
        { "id": "o_edda", "kind": "talk", "target": "Edda" },
        { "id": "o_pilze", "kind": "collect", "target": "Pilz", "count": 3 },
        { "id": "o_bonus", "kind": "interact", "target": "Laterne", "optional": true }
      ],
      "on_complete": ["flag:T_Weiter=1"],
      "next": [
        { "if": ["rel:Lio>=50"], "stage": "s2_lio" },
        { "stage": "s2_allein" }
      ]
    },
    { "id": "s2_lio", "text": "Mit Lio", "objectives": [ { "id": "o_c", "kind": "condition", "target": "rep:Hueter>=Geschaetzt" } ] },
    { "id": "s2_allein", "text": "Allein", "objectives": [ { "id": "o_c", "kind": "condition", "target": "rep:Hueter>=Geschaetzt" } ] }
  ],
  "rewards": { "xp": 50, "effects": ["rel:Lio+5"] }
})JSON");

	const TCHAR* FollowQuestJson = TEXT(R"JSON(
{
  "id": "T_Folge", "type": "side", "duration_min": 5, "auto_start": true,
  "requires": ["quest:T_Haupt"],
  "title": { "de": "Folge", "en": "Follow-up" },
  "stages": [ { "id": "s1", "text": "x", "objectives": [ { "id": "o", "kind": "reach", "target": "Tor" } ] } ]
})JSON");
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsQuestDataTest, "Ewigfels.Quest.RealDataLoads",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsQuestDataTest::RunTest(const FString& Parameters)
{
	FTestGame Game;
	TestEqual(TEXT("Quest-JSONs ohne Fehler geladen"), Game.Quests()->ReloadDefinitions(), 0);
	FEwigfelsQuestDef M110;
	TestTrue(TEXT("M110 vorhanden"), Game.Quests()->GetQuestDefinition(TEXT("M110"), M110));
	TestEqual(TEXT("M110 hat 4 Etappen"), M110.Stages.Num(), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsQuestFlowTest, "Ewigfels.Quest.Flow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsQuestFlowTest::RunTest(const FString& Parameters)
{
	FTestGame Game;
	UEwigfelsQuestSubsystem* Quests = Game.Quests();
	UEwigfelsWorldStateSubsystem* State = Game.State();

	TArray<FString> Errors;
	TestTrue(TEXT("Testquest parst"), Quests->AddDefinitionFromJson(TestQuestJson, TEXT("T_Haupt.json"), Errors));
	TestTrue(TEXT("Folgequest parst"), Quests->AddDefinitionFromJson(FollowQuestJson, TEXT("T_Folge.json"), Errors));
	for (const FString& E : Errors) { AddError(E); }

	TestFalse(TEXT("Folgequest startet nicht vorher"), Quests->CanStartQuest(TEXT("T_Folge")));
	TestTrue(TEXT("Start"), Quests->StartQuest(TEXT("T_Haupt")));
	TestEqual(TEXT("Status-Flag aktiv"), State->GetValue(EEwigfelsStateDomain::Flag, EwigfelsQuest::StatusFlag(TEXT("T_Haupt"))), 1);
	TestFalse(TEXT("Doppelstart verboten"), Quests->StartQuest(TEXT("T_Haupt")));

	TestEqual(TEXT("talk Edda zaehlt"), Quests->ReportEvent(TEXT("talk"), TEXT("edda")), 1); // Groß/klein egal
	Quests->ReportEvent(TEXT("collect"), TEXT("Pilz"), 2);
	TestEqual(TEXT("noch Etappe 1"), Quests->GetCurrentObjectives(TEXT("T_Haupt")).Num(), 3);
	TestFalse(TEXT("Flag noch nicht gesetzt"), State->HasFlag(TEXT("T_Weiter")));

	State->SetValue(EEwigfelsStateDomain::Relationship, TEXT("Lio"), 60);
	Quests->ReportEvent(TEXT("collect"), TEXT("Pilz"), 5); // optionales Ziel offen → trotzdem weiter
	TestTrue(TEXT("on_complete gesetzt"), State->HasFlag(TEXT("T_Weiter")));
	TestEqual(TEXT("Verzweigung Lio"), Quests->GetCurrentStageText(TEXT("T_Haupt")).ToString(), FString(TEXT("Mit Lio")));

	State->SetValue(EEwigfelsStateDomain::Reputation, TEXT("Hueter"), 25); // condition-Ziel erledigt sich selbst
	TestEqual(TEXT("abgeschlossen"), static_cast<int32>(Quests->GetQuestStatus(TEXT("T_Haupt"))), static_cast<int32>(EEwigfelsQuestStatus::Completed));
	TestEqual(TEXT("Belohnung angewendet"), State->GetValue(EEwigfelsStateDomain::Relationship, TEXT("Lio")), 65);
	TestEqual(TEXT("Folgequest automatisch gestartet"), static_cast<int32>(Quests->GetQuestStatus(TEXT("T_Folge"))), static_cast<int32>(EEwigfelsQuestStatus::Active));
	TestTrue(TEXT("Fertige Quest wird nicht mehr verfolgt"), Quests->GetTrackedQuest() != FName(TEXT("T_Haupt")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsQuestBadJsonTest, "Ewigfels.Quest.BadJson",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsQuestBadJsonTest::RunTest(const FString& Parameters)
{
	FEwigfelsQuestDef Quest;
	TArray<FString> Errors;
	TestFalse(TEXT("kaputtes JSON"), EwigfelsQuest::ParseQuestJson(TEXT("{ id: "), TEXT("x.json"), Quest, Errors));
	Errors.Reset();
	TestFalse(TEXT("tote Verzweigung"), EwigfelsQuest::ParseQuestJson(
		TEXT(R"({"id":"X","stages":[{"id":"a","next":[{"stage":"fehlt"}]}]})"), TEXT("x.json"), Quest, Errors));
	TestTrue(TEXT("Fehlertext nennt Etappe"), Errors.Num() > 0 && Errors[0].Contains(TEXT("fehlt")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsSaveRoundTripTest, "Ewigfels.Save.RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsSaveRoundTripTest::RunTest(const FString& Parameters)
{
	const FString Slot = TEXT("EwigfelsTest_RoundTrip");
	FTestGame Game;
	TArray<FString> Errors;
	Game.Quests()->AddDefinitionFromJson(TestQuestJson, TEXT("T_Haupt.json"), Errors);

	Game.State()->SetFlag(TEXT("E1_TrustEdda"));
	Game.State()->SetValue(EEwigfelsStateDomain::Reputation, TEXT("Kollegium"), -30);
	Game.Quests()->StartQuest(TEXT("T_Haupt"));
	Game.Quests()->ReportEvent(TEXT("collect"), TEXT("Pilz"), 2);

	UEwigfelsSaveGame* Snapshot = Game.Save()->CreateSnapshot(Slot, TEXT("Test"), false);
	TestNotNull(TEXT("Snapshot"), Snapshot);
	if (!Snapshot)
	{
		return false;
	}
	TestTrue(TEXT("auf Platte geschrieben"), UGameplayStatics::SaveGameToSlot(Snapshot, Slot, 0));

	// Alles kaputtmachen …
	Game.Save()->StartNewGame();
	TestFalse(TEXT("nach Neustart leer"), Game.State()->HasFlag(TEXT("E1_TrustEdda")));

	// … und wieder laden
	UEwigfelsSaveGame* Loaded = Cast<UEwigfelsSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot, 0));
	TestNotNull(TEXT("geladen"), Loaded);
	if (Loaded)
	{
		Game.Save()->ApplySnapshot(Loaded);
		TestTrue(TEXT("Flag zurück"), Game.State()->HasFlag(TEXT("E1_TrustEdda")));
		TestEqual(TEXT("Ruf zurück"), Game.State()->GetValue(EEwigfelsStateDomain::Reputation, TEXT("Kollegium")), -30);
		TestEqual(TEXT("Misstrauisch"), static_cast<int32>(Game.State()->GetReputationTier(TEXT("Kollegium"))), static_cast<int32>(EEwigfelsReputationTier::Misstrauisch));
		TestEqual(TEXT("Quest aktiv"), static_cast<int32>(Game.Quests()->GetQuestStatus(TEXT("T_Haupt"))), static_cast<int32>(EEwigfelsQuestStatus::Active));
		const TArray<FEwigfelsObjectiveView> Objectives = Game.Quests()->GetCurrentObjectives(TEXT("T_Haupt"));
		const FEwigfelsObjectiveView* Pilze = Objectives.FindByPredicate([](const FEwigfelsObjectiveView& V) { return V.Id == FName(TEXT("o_pilze")); });
		TestTrue(TEXT("Zielfortschritt zurück (2/3)"), Pilze && Pilze->Current == 2);
		TestEqual(TEXT("Version"), Loaded->SaveVersion, EwigfelsSave::CurrentVersion);
	}
	UGameplayStatics::DeleteGameInSlot(Slot, 0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
