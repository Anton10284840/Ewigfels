// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EwigfelsEnemyController.generated.h"

class AEwigfelsEnemy;

UENUM(BlueprintType)
enum class EEwigfelsEnemyState : uint8
{
	Idle,
	/** Ziel bemerkt: kurz innehalten (Spieler sieht, dass er entdeckt wurde). */
	Alert,
	Engage,
	Attack,
	Recover,
	Dead
};

/**
 * Gegner-KI (läuft nur auf dem Server). Bewusst eine kleine, lesbare Zustandsmaschine in C++, damit
 * Phase 3 ohne Editor-Assets spielbar ist. Für Phase 5 ist der Umzug auf StateTree (GDD §5) vorgesehen;
 * die Entscheidungen (ChooseAttack, DesiredMove) sind dafür als eigene Funktionen getrennt.
 *
 * Nahkampf (Novize): rückt an, holt sich einen von 2 Angriffsplätzen (UEwigfelsCombatDirector), sonst umkreisen.
 * Fernkampf (Fluchsänger): hält Abstand, wechselt die Seite, stößt zu nahe Spieler mit dem Fluchkreis weg.
 * Mehrere Spieler (Koop): wählt das nächste sichtbare Ziel, wechselt bei deutlich näherem Ziel.
 */
UCLASS()
class EWIGFELS_API AEwigfelsEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AEwigfelsEnemyController();

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|AI")
	EEwigfelsEnemyState GetState() const { return State; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|AI")
	AActor* GetTarget() const { return Target.Get(); }

private:
	void SetState(EEwigfelsEnemyState NewState);
	void UpdateTarget();
	void TickMelee(float DeltaSeconds, AEwigfelsEnemy* Self, AActor* Goal);
	void TickRanged(float DeltaSeconds, AEwigfelsEnemy* Self, AActor* Goal);
	bool TryCast(FName SpellId, AActor* Goal);
	bool HasLineOfSight(const AActor* Goal) const;
	void SteerTowards(const FVector& Destination, float Scale = 1.f);
	void SteerDirection(const FVector& Direction, float Scale = 1.f);
	void SetEngagedRotation(bool bEngaged);

	TWeakObjectPtr<AActor> Target;
	EEwigfelsEnemyState State = EEwigfelsEnemyState::Idle;
	float StateTime = 0.f;
	float NextTargetScan = 0.f;
	float StrafeSign = 1.f;
	float NextStrafeFlip = 0.f;
	FVector Home = FVector::ZeroVector;
	FVector LastMoveGoal = FVector(FLT_MAX);
};
