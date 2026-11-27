// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EwigfelsForcedMotionComponent.generated.h"

class UCharacterMovementComponent;

/**
 * Zwangsbewegung durch Weisen: Hochheben und Halten (Haltelied), Schmettern, und „Aufschlag erwartet“ nach
 * Stoß/Schmettern, damit die on_land-Effekte der Weise beim Aufprall wirken.
 *
 * Läuft NUR auf dem Server (wird bei Bedarf dort angehängt, nicht repliziert). Die Position kommt über die
 * normale Bewegungsreplikation der CharacterMovementComponent zu allen Clients; der Look (Ringe) über den Status "lifted".
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsForcedMotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsForcedMotionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Holt die Komponente oder hängt sie an (nur Server, nur Figuren mit CharacterMovement). */
	static UEwigfelsForcedMotionComponent* FindOrAdd(AActor* Actor);

	/** Hebt um Height cm an und hält Duration s. Nur Server. */
	void Lift(float Height, float Duration, FName SpellId, AActor* Instigator);

	/** Beendet das Halten und schmettert mit Speed cm/s nach unten. Nur Server. */
	void Slam(float Speed, FName SpellId, AActor* Instigator);

	/** Nach einem Stoß: beim nächsten Bodenkontakt on_land der Weise auslösen. Nur Server. */
	void ExpectLanding(FName SpellId, AActor* Instigator);

	/** Lässt los (Status weg, Tod, Arena-Neustart). */
	void Release();

	bool IsHeld() const { return bHolding; }
	bool IsHeldBy(const AActor* Instigator, FName SpellId) const;

	/** Reine Rechenfunktion (getestet): Höhe über Start nach Elapsed s, weich ansteigend, danach leichtes Schweben. */
	static float HoldHeightAt(float Elapsed, float RiseTime, float Height);

	/** Wie lange das Anheben dauert. */
	UPROPERTY(EditAnywhere, Category = "Ewigfels|Combat")
	float RiseTime = 0.35f;

private:
	UCharacterMovementComponent* GetMovement() const;
	void FireLanding();

	bool bHolding = false;
	bool bAwaitLanding = false;
	float Elapsed = 0.f;
	float HoldDuration = 0.f;
	float HoldHeight = 0.f;
	float StartZ = 0.f;
	float AwaitSince = 0.f;
	FName MotionSpell;
	TWeakObjectPtr<AActor> MotionInstigator;
};
