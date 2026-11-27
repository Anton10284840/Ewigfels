// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/EwigfelsCombatTypes.h"
#include "EwigfelsCombatComponent.generated.h"

struct FEwigfelsStatusDef;
class UEwigfelsSpellSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEwigfelsHealthChanged, float, Health, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FEwigfelsHitFeedback, float, Amount, FVector, Location, EEwigfelsHitResult, Result, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEwigfelsDied, AActor*, Killer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEwigfelsStatusesChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEwigfelsStaggered, float, Seconds);

/** Abfangen von Schaden (z. B. Glockenschild). Darf Info.Amount ändern. Nur Server. */
DECLARE_DELEGATE_RetVal_OneParam(EEwigfelsHitResult, FEwigfelsDamageInterceptor, FEwigfelsDamageInfo& /*Info*/);

/**
 * Leben, Team, Status-Effekte, Treffer und Tod – für Spieler, Gegner und Kreaturen gleich.
 *
 * Netzwerk (Koop-fähig): Alles, was den Spielstand ändert (ApplyDamage, ApplyStatus, Heal, …), läuft nur auf dem Server
 * (HasAuthority). Leben, Tod und Status werden repliziert; Treffer-Feedback (Zahlen, Blitz) kommt per Multicast.
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsCombatComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- Server-Aktionen ---

	/** Verarbeitet Schaden inkl. Block/Parade (Interceptor), Unverwundbarkeit und Tod. Nur Server. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	EEwigfelsHitResult ApplyDamage(FEwigfelsDamageInfo Info);

	/** Trägt einen Status auf (Dauer 0 = Standard). Hebt Gegen-Status auf, stapelt und eskaliert. Nur Server. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	bool ApplyStatus(FName StatusId, float Duration, AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	bool RemoveStatus(FName StatusId);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	void Heal(float Amount);

	/** Stoß in Weltkoordinaten (cm/s). Bei Figuren über LaunchCharacter. Nur Server. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	void ApplyImpulse(FVector Velocity);

	/** Unterbricht laufende Angriffe/Weisen für Seconds. Nur Server. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	void Stagger(float Seconds);

	/** Ausweichen: so lange keine Treffer. Nur Server. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	void SetInvulnerableFor(float Seconds);

	/** Volle Gesundheit, Status weg, lebendig (Respawn, Arena-Neustart). Nur Server. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Combat")
	void ResetCombatState();

	// --- Abfragen (überall) ---

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	bool IsDead() const { return bDead; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	bool HasStatus(FName StatusId) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	int32 GetStatusStacks(FName StatusId) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	const TArray<FEwigfelsActiveStatus>& GetActiveStatuses() const { return ActiveStatuses; }

	/** Produkt der Bewegungs-Multiplikatoren aller Status (Gefroren = 0). */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	float GetMoveSpeedMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	float GetActionSpeedMultiplier() const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	float GetAtemRegenMultiplier() const;

	/** Gefroren, betäubt, taumelnd oder tot. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	bool CanAct() const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	bool IsStaggered() const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	EEwigfelsTeam GetTeam() const { return Team; }

	void SetTeam(EEwigfelsTeam NewTeam) { Team = NewTeam; }
	void SetMaxHealth(float NewMax, bool bRefill);

	/** Feindlich = beide haben Kampfkomponente, verschiedene Teams, keins neutral. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Combat")
	static bool AreHostile(const AActor* A, const AActor* B);

	static UEwigfelsCombatComponent* Find(const AActor* Actor);

	/** Server-Weltzeit (gleich auf allen Maschinen), Basis aller Ablaufzeiten. */
	float GetServerTime() const;

	// --- Ereignisse ---

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Combat")
	FEwigfelsHealthChanged OnHealthChanged;

	/** Auf allen Maschinen (Multicast). Für Schadenszahlen, Treffer-Blitz, Treffer-Animation. */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Combat")
	FEwigfelsHitFeedback OnHitFeedback;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Combat")
	FEwigfelsDied OnDied;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Combat")
	FEwigfelsStatusesChanged OnStatusesChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Combat")
	FEwigfelsStaggered OnStaggered;

	/** Glockenschild und andere Abwehr. Nur Server, ein Abfänger pro Komponente. */
	FEwigfelsDamageInterceptor DamageInterceptor;

	/** Feste Werte, nicht repliziert (kommen aus Klasse/Datei). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Combat")
	float DefaultMaxHealth = 100.f;

	/** Element → Schadensfaktor (0,5 = halber Schaden). Z. B. Fluchsänger: Glut 1,5. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Combat")
	TMap<FName, float> ElementMultipliers;

	/** Ab so viel Schaden auf einmal gibt es eine Treffer-Reaktion (Taumeln). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Combat")
	float StaggerThreshold = 18.f;

	/** Wie lange ein Taumeln dauert. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Combat")
	float StaggerDuration = 0.45f;

protected:
	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UFUNCTION()
	void OnRep_Dead();

	UFUNCTION()
	void OnRep_Statuses();

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_HitFeedback(float Amount, FVector_NetQuantize Location, EEwigfelsHitResult Result, AActor* Instigator);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Staggered(float Seconds);

private:
	void SetHealth(float NewHealth);
	void Die(AActor* Killer);
	void TickStatusesServer(float DeltaTime);
	const FEwigfelsStatusDef* StatusDef(FName Id) const;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health = 100.f;

	UPROPERTY(Replicated)
	float MaxHealth = 100.f;

	UPROPERTY(Replicated, EditAnywhere, Category = "Ewigfels|Combat")
	EEwigfelsTeam Team = EEwigfelsTeam::Neutral;

	UPROPERTY(ReplicatedUsing = OnRep_Dead)
	bool bDead = false;

	UPROPERTY(ReplicatedUsing = OnRep_Statuses)
	TArray<FEwigfelsActiveStatus> ActiveStatuses;

	UPROPERTY(Replicated)
	float StaggerEndTime = 0.f;

	/** Nur Server. */
	float InvulnerableEndTime = 0.f;
	float DotAccumulator = 0.f;
};
