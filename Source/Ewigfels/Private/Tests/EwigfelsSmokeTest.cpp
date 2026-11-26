// Copyright Ewigfels. Alle Rechte vorbehalten.
//
// Rauchtest für Phase 0. Ausführen:
//   Editor: Tools > Session Frontend > Automation > "Ewigfels.Smoke" anhaken > Start
//   Kommandozeile: Scripts/Tools/Test-Ewigfels.ps1

#include "Misc/AutomationTest.h"
#include "EwigfelsGameMode.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEwigfelsSmokeTest, "Ewigfels.Smoke.GameModeExists",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FEwigfelsSmokeTest::RunTest(const FString& Parameters)
{
	const AEwigfelsGameMode* CDO = GetDefault<AEwigfelsGameMode>();
	TestNotNull(TEXT("GameMode-CDO existiert"), CDO);
	if (CDO)
	{
		TestFalse(TEXT("StartupMessage ist gesetzt"), CDO->StartupMessage.IsEmpty());
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
