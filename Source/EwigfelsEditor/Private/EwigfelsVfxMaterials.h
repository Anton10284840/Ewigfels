// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"

/** Effekt-Materialien /Game/VFX/Materials/M_EwVfx_* (Glow, Flare, Burst, Spark, Bolt, Trail, Fire, Smoke, Wisp, Cloud, GlowCloud). */
namespace EwigfelsVfxMaterials
{
	/** Baut alle Materialien (vorhandene werden neu verdrahtet). Rückgabe: Anzahl ohne Fehler. */
	int32 BuildAll();

	/** Objektpfad eines Materials, z. B. Path(TEXT("M_EwVfx_Glow")). */
	FString Path(const TCHAR* Name);
}
