// Copyright Ewigfels. Alle Rechte vorbehalten.
//
// Tests für Weltstatus, Bedingungen und Effekte. Ausführen: Scripts/Tools/Test-Ewigfels.ps1
// (oder Editor: Tools > Session Frontend > Automation > "Ewigfels").

#include "Misc/AutomationTest.h"
#include "WorldState/EwigfelsWorldStateTypes.h"
#include "Dialogue/EwigfelsDialogueBridge.h"
#include "Quest/EwigfelsQuestTypes.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsConditionParseTest, "Ewigfels.WorldState.ParseCondition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsConditionParseTest::RunTest(const FString& Parameters)
{
	FEwigfelsCondition C;
	FString Error;

	TestTrue(TEXT("flag ohne Vergleich"), EwigfelsWorldState::ParseCondition(TEXT("flag:E1_TrustEdda"), C, Error));
	TestEqual(TEXT("flag Bereich"), static_cast<int32>(C.Domain), static_cast<int32>(EEwigfelsStateDomain::Flag));
	TestEqual(TEXT("flag Schluessel"), C.Key, FName(TEXT("E1_TrustEdda")));
	TestEqual(TEXT("flag heisst != 0"), static_cast<int32>(C.Op), static_cast<int32>(EEwigfelsCompareOp::NotEqual));

	TestTrue(TEXT("!flag"), EwigfelsWorldState::ParseCondition(TEXT("!flag:E1_TrustEdda"), C, Error));
	TestEqual(TEXT("!flag heisst == 0"), static_cast<int32>(C.Op), static_cast<int32>(EEwigfelsCompareOp::Equal));

	TestTrue(TEXT("rel >="), EwigfelsWorldState::ParseCondition(TEXT(" rel:Lio >= 30 "), C, Error));
	TestEqual(TEXT("rel Bereich"), static_cast<int32>(C.Domain), static_cast<int32>(EEwigfelsStateDomain::Relationship));
	TestEqual(TEXT("rel Op"), static_cast<int32>(C.Op), static_cast<int32>(EEwigfelsCompareOp::GreaterEqual));
	TestEqual(TEXT("rel Wert"), C.Value, 30);

	TestTrue(TEXT("rel negativ"), EwigfelsWorldState::ParseCondition(TEXT("rel:Mara<-20"), C, Error));
	TestEqual(TEXT("rel negativ Wert"), C.Value, -20);
	TestEqual(TEXT("rel negativ Op"), static_cast<int32>(C.Op), static_cast<int32>(EEwigfelsCompareOp::Less));

	TestTrue(TEXT("rep Stufe"), EwigfelsWorldState::ParseCondition(TEXT("rep:Kollegium>=Geschaetzt"), C, Error));
	TestEqual(TEXT("rep Stufe = 20"), C.Value, 20);

	TestTrue(TEXT("quest fertig"), EwigfelsWorldState::ParseCondition(TEXT("quest:M101"), C, Error));
	TestEqual(TEXT("quest Schluessel"), C.Key, FName(TEXT("Quest_M101")));
	TestEqual(TEXT("quest Wert"), C.Value, 2);
	TestTrue(TEXT("quest aktiv"), EwigfelsWorldState::ParseCondition(TEXT("quest:M101=active"), C, Error));
	TestEqual(TEXT("quest aktiv Wert"), C.Value, 1);

	TestFalse(TEXT("ohne Bereich"), EwigfelsWorldState::ParseCondition(TEXT("E1_TrustEdda"), C, Error));
	TestFalse(TEXT("rel ohne Vergleich"), EwigfelsWorldState::ParseCondition(TEXT("rel:Lio"), C, Error));
	TestFalse(TEXT("Wert keine Zahl"), EwigfelsWorldState::ParseCondition(TEXT("rel:Lio>=viel"), C, Error));
	TestFalse(TEXT("unbekannter Bereich"), EwigfelsWorldState::ParseCondition(TEXT("xp:Lio>3"), C, Error));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsEffectParseTest, "Ewigfels.WorldState.ParseEffect",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsEffectParseTest::RunTest(const FString& Parameters)
{
	FEwigfelsEffect E;
	FString Error;
	TestTrue(TEXT("setzen"), EwigfelsWorldState::ParseEffect(TEXT("flag:E1_TrustEdda=1"), E, Error));
	TestEqual(TEXT("setzen Op"), static_cast<int32>(E.Op), static_cast<int32>(EEwigfelsEffectOp::Set));
	TestEqual(TEXT("setzen Wert"), E.Value, 1);
	TestTrue(TEXT("plus"), EwigfelsWorldState::ParseEffect(TEXT("rel:Lio+10"), E, Error));
	TestEqual(TEXT("plus Op"), static_cast<int32>(E.Op), static_cast<int32>(EEwigfelsEffectOp::Add));
	TestEqual(TEXT("plus Wert"), E.Value, 10);
	TestTrue(TEXT("minus"), EwigfelsWorldState::ParseEffect(TEXT("rep:Kollegium-15"), E, Error));
	TestEqual(TEXT("minus Wert"), E.Value, -15);
	TestFalse(TEXT("ohne Aenderung"), EwigfelsWorldState::ParseEffect(TEXT("flag:X"), E, Error));
	TestFalse(TEXT("quest nicht setzbar"), EwigfelsWorldState::ParseEffect(TEXT("quest:M101=2"), E, Error));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsWorldStateDataTest, "Ewigfels.WorldState.Data",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsWorldStateDataTest::RunTest(const FString& Parameters)
{
	FEwigfelsWorldStateData Data;
	int32 Old = 0, New = 0;
	FEwigfelsEffect E;
	FString Error;

	EwigfelsWorldState::ParseEffect(TEXT("rel:Lio+80"), E, Error);
	Data.Apply(E, Old, New);
	Data.Apply(E, Old, New);
	TestEqual(TEXT("Beziehung auf +100 begrenzt"), Data.GetValue(EEwigfelsStateDomain::Relationship, TEXT("Lio")), 100);
	TestEqual(TEXT("alter Wert gemeldet"), Old, 80);

	EwigfelsWorldState::ParseEffect(TEXT("flag:NachhallLoad+150"), E, Error);
	Data.Apply(E, Old, New);
	TestEqual(TEXT("Flags sind nicht begrenzt"), Data.GetValue(EEwigfelsStateDomain::Flag, TEXT("NachhallLoad")), 150);

	Data.SetValue(EEwigfelsStateDomain::Flag, TEXT("NachhallLoad"), 0);
	TestFalse(TEXT("0 wird aus der Map entfernt"), Data.Flags.Contains(TEXT("NachhallLoad")));

	FEwigfelsCondition C;
	EwigfelsWorldState::ParseCondition(TEXT("rel:Lio>=100"), C, Error);
	TestTrue(TEXT("Bedingung wahr"), Data.Check(C));
	EwigfelsWorldState::ParseCondition(TEXT("rep:Kollegium>=Geschaetzt"), C, Error);
	TestFalse(TEXT("Ruf 0 ist nicht Geschaetzt"), Data.Check(C));

	TestEqual(TEXT("-60 = Feind"), static_cast<int32>(EwigfelsWorldState::TierFromValue(-60)), static_cast<int32>(EEwigfelsReputationTier::Feind));
	TestEqual(TEXT("-20 = Misstrauisch"), static_cast<int32>(EwigfelsWorldState::TierFromValue(-20)), static_cast<int32>(EEwigfelsReputationTier::Misstrauisch));
	TestEqual(TEXT("0 = Neutral"), static_cast<int32>(EwigfelsWorldState::TierFromValue(0)), static_cast<int32>(EEwigfelsReputationTier::Neutral));
	TestEqual(TEXT("20 = Geschaetzt"), static_cast<int32>(EwigfelsWorldState::TierFromValue(20)), static_cast<int32>(EEwigfelsReputationTier::Geschaetzt));
	TestEqual(TEXT("60 = Verbuendet"), static_cast<int32>(EwigfelsWorldState::TierFromValue(60)), static_cast<int32>(EEwigfelsReputationTier::Verbuendet));
	for (int32 V = -100; V <= 100; ++V)
	{
		// Untergrenze jeder Stufe muss zur Stufe selbst gehören
		const EEwigfelsReputationTier Tier = EwigfelsWorldState::TierFromValue(V);
		if (V == EwigfelsWorldState::TierMinValue(Tier))
		{
			continue;
		}
		if (V < EwigfelsWorldState::TierMinValue(Tier))
		{
			AddError(FString::Printf(TEXT("Wert %d liegt unter der Untergrenze seiner Stufe"), V));
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsDialogueVariableTest, "Ewigfels.Dialogue.VariableNames",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsDialogueVariableTest::RunTest(const FString& Parameters)
{
	bool bQuest = false;
	EEwigfelsStateDomain Domain = EEwigfelsStateDomain::Flag;
	FName Key;
	TestTrue(TEXT("rel.Lio"), UEwigfelsDialogueBridge::ParseVariableName(TEXT("rel.Lio"), bQuest, Domain, Key));
	TestEqual(TEXT("rel.Lio Bereich"), static_cast<int32>(Domain), static_cast<int32>(EEwigfelsStateDomain::Relationship));
	TestEqual(TEXT("rel.Lio Schluessel"), Key, FName(TEXT("Lio")));
	TestTrue(TEXT("quest.M101"), UEwigfelsDialogueBridge::ParseVariableName(TEXT("quest.M101"), bQuest, Domain, Key));
	TestTrue(TEXT("quest.M101 ist Quest"), bQuest);
	TestEqual(TEXT("quest.M101 Schluessel"), Key, EwigfelsQuest::StatusFlag(TEXT("M101")));
	TestFalse(TEXT("global.* gehoert SUDS"), UEwigfelsDialogueBridge::ParseVariableName(TEXT("global.Foo"), bQuest, Domain, Key));
	TestFalse(TEXT("lokale Variable"), UEwigfelsDialogueBridge::ParseVariableName(TEXT("Zaehler"), bQuest, Domain, Key));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
