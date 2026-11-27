// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat/EwigfelsCombatTypes.h"
#include "AI/EwigfelsCreatureTypes.h"
#include "EwigfelsEnemy.generated.h"

class UEwigfelsCombatComponent;
class UEwigfelsSpellCasterComponent;
class UEwigfelsPlaceholderBody;

/**
 * Gegner/Kreatur. Typ kommt aus Content/Data/Creatures (CreatureId), Angriffe sind Weisen (gleiches System wie der Spieler).
 * KI: AEwigfelsEnemyController (nur auf dem Server). Repliziert: Typ, Bewegung, Leben, Status, Wirken.
 */
UCLASS()
class EWIGFELS_API AEwigfelsEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEwigfelsEnemy(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Vor dem Spawnen setzen (SpawnActorDeferred) oder im Level einstellen. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|AI")
	void SetCreatureId(FName NewId) { CreatureId = NewId; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|AI")
	FName GetCreatureId() const { return CreatureId; }

	const FEwigfelsCreatureDef* GetCreatureDef() const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|AI")
	FText GetDisplayName() const;

	UEwigfelsCombatComponent* GetCombat() const { return Combat; }
	UEwigfelsSpellCasterComponent* GetCaster() const { return Caster; }

	/** Gerade am Ausholen (Vorwarnung). Für HUD und Animation. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|AI")
	bool IsTelegraphing() const;

	/** Lauf- oder Gehtempo setzen (Server, KI). */
	void SetRunning(bool bRun);

	/** Wann zuletzt Schaden genommen (HUD blendet Lebensbalken ein). Lokal. */
	float GetLastHitTime() const { return LastHitTime; }

protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CreatureId, Category = "Ewigfels|AI")
	FName CreatureId = TEXT("Novize");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI")
	TObjectPtr<UEwigfelsCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI")
	TObjectPtr<UEwigfelsSpellCasterComponent> Caster;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|AI")
	TObjectPtr<UEwigfelsPlaceholderBody> PlaceholderBody;

	UFUNCTION()
	void OnRep_CreatureId();

	UFUNCTION()
	void HandleCastStarted(FName SpellId, uint8 ChainIndex, float CastTime);

	UFUNCTION()
	void HandleSpellReleased(FName SpellId, FVector Origin, FVector Direction);

	UFUNCTION()
	void HandleHitFeedback(float Amount, FVector Location, EEwigfelsHitResult Result, AActor* InstigatorActor);

	UFUNCTION()
	void HandleDied(AActor* Killer);

	UFUNCTION()
	void HandleStaggered(float Seconds);

private:
	void ApplyCreatureDef();

	float TelegraphUntil = 0.f;
	float LastHitTime = -100.f;
	bool bDefApplied = false;
};
