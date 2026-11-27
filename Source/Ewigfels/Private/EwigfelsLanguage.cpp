// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "EwigfelsLanguage.h"
#include "HAL/IConsoleManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace
{
	TAutoConsoleVariable<FString> CVarEwigfelsLanguage(
		TEXT("Ewigfels.Language"), TEXT("de"),
		TEXT("Sprache der Spieltexte aus Content/Data (de, en). Gilt für neu geladene Daten."));
}

FString EwigfelsLanguage::Get()
{
	FString FromCmd;
	if (FParse::Value(FCommandLine::Get(), TEXT("EwigfelsLanguage="), FromCmd) && !FromCmd.IsEmpty())
	{
		return FromCmd.ToLower();
	}
	const FString Lang = CVarEwigfelsLanguage.GetValueOnAnyThread();
	return Lang.IsEmpty() ? FString(TEXT("de")) : Lang.ToLower();
}
