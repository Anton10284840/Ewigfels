// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "EwigfelsCombatTypes.generated.h"

UENUM(BlueprintType)
enum class EEwigfelsTeam : uint8
{
	Player,
	Enemy,
	Neutral
};

UENUM(BlueprintType)
enum class EEwigfelsHitResult : uint8
{
	/** Voller Treffer. */
	Hit,
	/** Vom Glockenschild abgefangen (Restschaden). */
	Blocked,
	/** Reinklang: perfekte Parade, kein Schaden, Konter. */
	Parried,
	/** Unverwundbar (Ausweichen, tot, gleiches Team). */
	Ignored,
	/** Treffer war tödlich. */
	Killed
};

/** Ein Schadensereignis. Wird nur auf dem Server erzeugt und verarbeitet. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Ewigfels|Combat")
	float Amount = 0.f;

	/** Grundton/Element (Glut, Firn, Strom, …). Leer = körperlich. */
	UPROPERTY(BlueprintReadWrite, Category = "Ewigfels|Combat")
	FName Element;

	/** Wer den Schaden verursacht (Wirker, Gegner). */
	UPROPERTY(BlueprintReadWrite, Category = "Ewigfels|Combat")
	TObjectPtr<AActor> Instigator = nullptr;

	/** Weise oder Gegner-Angriff, aus dem der Schaden stammt. */
	UPROPERTY(BlueprintReadWrite, Category = "Ewigfels|Combat")
	FName Source;

	UPROPERTY(BlueprintReadWrite, Category = "Ewigfels|Combat")
	FVector HitLocation = FVector::ZeroVector;

	/** DoT (Brennen) lässt sich nicht blocken. */
	UPROPERTY(BlueprintReadWrite, Category = "Ewigfels|Combat")
	bool bCanBeBlocked = true;

	/** Zeigt keine Treffer-Reaktion (für DoT). */
	UPROPERTY(BlueprintReadWrite, Category = "Ewigfels|Combat")
	bool bSilent = false;
};

/** Laufender Status auf einem Ziel. Wird repliziert, Ablaufzeit in Server-Weltzeit. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsActiveStatus
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Combat")
	FName Id;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Combat")
	float EndTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Combat")
	float Duration = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Combat")
	int32 Stacks = 1;

	UPROPERTY()
	TObjectPtr<AActor> Instigator = nullptr;
};
