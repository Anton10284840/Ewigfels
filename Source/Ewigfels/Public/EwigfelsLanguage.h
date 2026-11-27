// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"

namespace EwigfelsLanguage
{
	/**
	 * Sprache für Texte aus den JSON-Daten ({"de": …, "en": …}). Standard Deutsch, unabhängig von der Windows-/Editor-Sprache.
	 * Umschalten: Konsole "Ewigfels.Language en" oder Startparameter -EwigfelsLanguage=en. Später aus den Spieloptionen.
	 */
	EWIGFELS_API FString Get();
}
