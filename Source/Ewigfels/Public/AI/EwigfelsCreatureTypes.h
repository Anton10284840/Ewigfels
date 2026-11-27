// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EwigfelsCreatureTypes.generated.h"

UENUM(BlueprintType)
enum class EEwigfelsCreatureBehavior : uint8
{
	/** Nahkampf: rückt an, umkreist, wartet auf einen Angriffsplatz (max. 2 gleichzeitig). */
	Melee,
	/** Fernkampf: hält Abstand, flankiert, stößt zu nahe Spieler weg. */
	Ranged,
	/** Übungspuppe: steht, greift nicht an, heilt sich. */
	Dummy
};

USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsCreatureLook
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FLinearColor Body = FLinearColor(0.1f, 0.1f, 0.12f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FLinearColor Accent = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FLinearColor Eyes = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float Height = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") bool bHood = true;
	/** Optionales Skeletal Mesh (sonst Platzhalter-Körper). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FSoftObjectPath Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FSoftClassPath AnimClass;
};

/** Ein Gegner-/Kreaturtyp aus Content/Data/Creatures/*.json. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsCreatureDef
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FName Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FName Faction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") EEwigfelsCreatureBehavior Behavior = EEwigfelsCreatureBehavior::Melee;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float Health = 80.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float WalkSpeed = 250.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float RunSpeed = 450.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float AggroRadius = 2500.f;
	/** Ab dieser Entfernung wird angegriffen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float AttackRange = 200.f;
	/** Fernkämpfer: Wunschabstand. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float PreferredRange = 0.f;
	/** Fernkämpfer: darunter wird zurückgewichen bzw. weggestoßen. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float FleeRange = 0.f;
	/** Angriffe = Weisen aus Content/Data/Spells (Reihenfolge = Priorität, erste = Standardangriff). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") TArray<FName> Spells;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") TMap<FName, float> ElementMultipliers;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") float StaggerThreshold = 18.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI") FEwigfelsCreatureLook Look;
};

namespace EwigfelsCreatures
{
	EWIGFELS_API bool ParseCreatureFile(const FString& JsonText, const FString& SourceName, TArray<FEwigfelsCreatureDef>& Out, TArray<FString>& OutErrors);
}

/** Katalog aller Gegnertypen. Konsole: Ewigfels.Creatures.List */
UCLASS()
class EWIGFELS_API UEwigfelsCreatureSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|AI")
	int32 ReloadDefinitions();

	const FEwigfelsCreatureDef* FindCreature(FName Id) const { return Creatures.Find(Id); }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|AI")
	TArray<FName> GetAllCreatureIds() const;

	const TArray<FString>& GetLoadErrors() const { return LoadErrors; }

private:
	TMap<FName, FEwigfelsCreatureDef> Creatures;
	TArray<FString> LoadErrors;
};
