// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/NetSerialization.h"
#include "EwigfelsSpellProjectile.generated.h"

class UNiagaraComponent;
class UPointLightComponent;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;
struct FEwigfelsSpellDef;

/**
 * Geschoss einer Weise (Funkenstoß, Glutweise, Frostweise, Gegner-Flüche). Werte kommen komplett aus der Weise.
 * Netzwerk: Der Server spawnt es, Bewegung wird repliziert, Treffer und Schaden nur auf dem Server.
 * Der Aufprall-Effekt kommt per Multicast auf alle Maschinen.
 */
UCLASS(NotPlaceable)
class EWIGFELS_API AEwigfelsSpellProjectile : public AActor
{
	GENERATED_BODY()

public:
	AEwigfelsSpellProjectile();

	/** Vor FinishSpawning aufrufen (SpawnActorDeferred). */
	void InitSpell(FName InSpellId, bool bInChainEnd, AActor* InHomingTarget);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	FName GetSpellId() const { return SpellId; }

protected:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBlockingHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Impact(FVector_NetQuantize Location, FVector_NetQuantizeNormal Normal);

	UFUNCTION()
	void OnRep_SpellId();

private:
	const FEwigfelsSpellDef* GetSpell() const;
	void ApplyVisuals();
	void ConfigureMovement();
	void HitTarget(AActor* Target, const FVector& Location);
	void Impact(const FVector& Location, const FVector& Normal);

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Magic")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Magic")
	TObjectPtr<UProjectileMovementComponent> Movement;

	/** Platzhalter-Kern (Kugel/Lanze). Wird ausgeblendet, sobald ein Niagara-System da ist. */
	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Magic")
	TObjectPtr<UStaticMeshComponent> Core;

	UPROPERTY(VisibleAnywhere, Category = "Ewigfels|Magic")
	TObjectPtr<UPointLightComponent> Light;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraComponent> Trail;

	UPROPERTY(ReplicatedUsing = OnRep_SpellId)
	FName SpellId;

	UPROPERTY(Replicated)
	bool bChainEnd = true;

	UPROPERTY()
	TWeakObjectPtr<AActor> HomingTarget;

	/** Schon getroffene Ziele (Durchschlag trifft niemanden doppelt). Nur Server. */
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> AlreadyHit;

	int32 PierceLeft = 0;
	bool bImpacted = false;
	bool bVisualsApplied = false;
	bool bMovementConfigured = false;
};
