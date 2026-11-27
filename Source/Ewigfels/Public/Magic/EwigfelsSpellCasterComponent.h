// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/EwigfelsCombatTypes.h"
#include "Magic/EwigfelsSpellTypes.h"
#include "EwigfelsSpellCasterComponent.generated.h"

class UEwigfelsSpellSubsystem;
class UEwigfelsCombatComponent;

USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsSpellCooldown
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName SpellId;

	/** Server-Weltzeit, ab der die Weise wieder geht. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	float ReadyTime = 0.f;
};

/** Was gerade gewirkt wird – repliziert, damit Animation und HUD auf allen Maschinen gleich sind. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsCastState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName SpellId;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	float StartTime = 0.f;

	/** Ausholen fertig, Weise ausgelöst. */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	float ReleaseTime = 0.f;

	/** Bis dahin keine neue Weise (Erholung). */
	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	float BusyUntil = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Ewigfels|Magic")
	uint8 ChainIndex = 0;

	/** Zählt bei jedem Wirken hoch, damit OnRep auch bei gleicher Weise feuert. */
	UPROPERTY()
	uint8 Serial = 0;
};

UENUM(BlueprintType)
enum class EEwigfelsCastFailReason : uint8
{
	None,
	UnknownSpell,
	NotInLoadout,
	Busy,
	Cooldown,
	NoAtem,
	CannotAct
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEwigfelsCastStarted, FName, SpellId, uint8, ChainIndex, float, CastTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEwigfelsSpellReleased, FName, SpellId, FVector, Origin, FVector, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEwigfelsCastFailed, FName, SpellId, EEwigfelsCastFailReason, Reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEwigfelsShieldChanged, bool, bShieldUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEwigfelsParried, AActor*, Attacker, FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEwigfelsAtemChanged, float, Atem, float, MaxAtem);

/**
 * Wirkt Weisen: Atem, Abklingzeiten, Ausholen/Erholung, Ketten (Funkenstoß), Schild mit Parade (Reinklang).
 * Für Spieler UND Gegner – Gegner-Angriffe sind ebenfalls Weisen (Content/Data/Spells/Gegnerweisen.json).
 *
 * Netzwerk: Der lokale Spieler ruft RequestCast/RequestShield. Das schickt Ziel und Zielpunkt an den Server
 * (Server_Cast), der prüft Kosten/Abklingzeit, zieht Atem ab und löst die Weise aus. Geschosse sind replizierte Actors,
 * Schaden nur auf dem Server. Animation/VFX: der eigene Client spielt sofort (Vorhersage), alle anderen per Multicast.
 * KI ruft direkt CastSpell (läuft eh auf dem Server).
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsSpellCasterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsSpellCasterComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- Lokaler Spieler ---

	/** Schnellwahl-Slot (0 = Basisangriff Funkenstoß, 1–4 = Weisen 1–4). Sendet an den Server. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Magic")
	void RequestCastSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Magic")
	void RequestCast(FName SpellId);

	/** Glockenschild hoch/runter. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Magic")
	void RequestShield(bool bUp);

	/** Lock-On-Ziel des lokalen Spielers (für Zielsuche der Geschosse). */
	void SetAimTarget(AActor* Target) { AimTarget = Target; }

	/** Zielpunkt für die nächste Weise (Spieler: Kamera-Mitte, KI: Gegner). Wird mitgeschickt. */
	void SetAimPointProvider(TFunction<FVector()> Provider) { AimPointProvider = MoveTemp(Provider); }

	// --- Server ---

	/** Wirkt die Weise sofort auf dem Server (KI, Konter, Tests). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Magic")
	EEwigfelsCastFailReason CastSpell(FName SpellId, FVector AimPoint, AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Magic")
	void RestoreAtem(float Amount);

	/** Zieht Atem ab, wenn genug da ist (Ausweichen). Nur Server. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Magic")
	bool TrySpendAtem(float Amount);

	/** Bricht Ausholen ab (Treffer, Taumeln). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Magic")
	void InterruptCast();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Ewigfels|Magic")
	void SetLoadout(const TArray<FName>& NewLoadout);

	// --- Abfragen ---

	/** Prüfung ohne Wirkung (HUD: Slot ausgegraut). */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	EEwigfelsCastFailReason CanCast(FName SpellId) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	float GetAtem() const { return Atem; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	float GetMaxAtem() const { return MaxAtem; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	const TArray<FName>& GetLoadout() const { return Loadout; }

	/** Restzeit in s bis die Weise wieder geht (0 = bereit). */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	float GetCooldownRemaining(FName SpellId) const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	bool IsShieldUp() const { return bShieldUp; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	bool IsCasting() const;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	const FEwigfelsCastState& GetCastState() const { return CastState; }

	/** Fingerspitze (bzw. Mitte beider Hände), aus der die Weise kommt. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Magic")
	FVector GetCastOrigin(bool bTwoHanded) const;

	const FEwigfelsSpellDef* FindSpellDef(FName SpellId) const;

	/** Kettenglied für die nächste Wirkung dieser Weise (rein rechnerisch, für Tests öffentlich). */
	static uint8 NextChainIndex(const FEwigfelsSpellDef& Spell, FName LastSpell, uint8 LastIndex, float LastReleaseTime, float Now);

	/** Parade-Entscheid: true = innerhalb des Fensters seit dem Hochnehmen. */
	static bool IsWithinParryWindow(float ShieldUpTime, float Now, float Window) { return Now - ShieldUpTime <= Window; }

	// --- Einstellungen ---

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float DefaultMaxAtem = 100.f;

	/** Atem pro Sekunde, nach AtemRegenDelay ohne Ausgabe. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float AtemRegenPerSecond = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float AtemRegenDelay = 0.9f;

	/** Glockenschild-Weise (Schutz-Taste). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName ShieldSpell = TEXT("glockenschild");

	/** Slot 0 (linke Maustaste). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	FName BasicSpell = TEXT("funkenstoss");

	/** Schild wirkt nur gegen Treffer von vorne (voller Winkel in Grad). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	float ShieldArc = 220.f;

	/** Zeitlupe bei Reinklang – nur im Einzelspiel (im Koop würde sie alle verlangsamen). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Magic")
	bool bParrySlowMotion = true;

	// --- Ereignisse (auf allen Maschinen) ---

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Magic")
	FEwigfelsCastStarted OnCastStarted;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Magic")
	FEwigfelsSpellReleased OnSpellReleased;

	/** Nur lokal beim Wirker (HUD-Hinweis „Kein Atem“). */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Magic")
	FEwigfelsCastFailed OnCastFailed;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Magic")
	FEwigfelsShieldChanged OnShieldChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Magic")
	FEwigfelsParried OnParried;

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Magic")
	FEwigfelsAtemChanged OnAtemChanged;

protected:
	UFUNCTION(Server, Reliable)
	void Server_Cast(FName SpellId, FVector_NetQuantize AimPoint, AActor* Target);

	UFUNCTION(Server, Reliable)
	void Server_SetShield(bool bUp);

	UFUNCTION(Client, Reliable)
	void Client_CastRejected(FName SpellId, EEwigfelsCastFailReason Reason);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_SpellReleased(FName SpellId, FVector_NetQuantize Origin, FVector_NetQuantizeNormal Direction);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Parried(AActor* Attacker, FVector_NetQuantize Location);

	UFUNCTION()
	void OnRep_CastState();

	UFUNCTION()
	void OnRep_ShieldUp();

	UFUNCTION()
	void OnRep_Atem();

private:
	EEwigfelsCastFailReason CastSpellInternal(FName SpellId, FVector AimPoint, AActor* Target, bool bSkipLoadoutCheck);
	void ReleaseSpell(FName SpellId, uint8 ChainIndex);
	void ExecuteDelivery(const FEwigfelsSpellDef& Spell, uint8 ChainIndex, const FVector& Origin, const FVector& Direction);
	void SpendAtem(float Amount);
	void SetShieldServer(bool bUp);
	EEwigfelsHitResult InterceptDamage(FEwigfelsDamageInfo& Info);
	bool IsLocallyControlledOwner() const;
	float Now() const;
	UEwigfelsCombatComponent* GetCombat() const;
	UEwigfelsSpellSubsystem* GetSpells() const;
	FVector ComputeLocalAimPoint() const;

	UPROPERTY(ReplicatedUsing = OnRep_Atem)
	float Atem = 100.f;

	UPROPERTY(Replicated)
	float MaxAtem = 100.f;

	UPROPERTY(Replicated)
	TArray<FName> Loadout;

	UPROPERTY(Replicated)
	TArray<FEwigfelsSpellCooldown> Cooldowns;

	UPROPERTY(ReplicatedUsing = OnRep_CastState)
	FEwigfelsCastState CastState;

	UPROPERTY(ReplicatedUsing = OnRep_ShieldUp)
	bool bShieldUp = false;

	/** Server-Zeit, zu der der Schild hochging (Parierfenster). */
	UPROPERTY(Replicated)
	float ShieldUpTime = 0.f;

	// Nur Server
	FName PendingSpell;
	FVector PendingAimPoint = FVector::ZeroVector;
	TWeakObjectPtr<AActor> PendingTarget;
	FTimerHandle ReleaseTimer;
	float LastSpendTime = -100.f;

	// Lokal
	TWeakObjectPtr<AActor> AimTarget;
	TFunction<FVector()> AimPointProvider;
	/** Serial der letzten lokal vorhergesagten Wirkung, damit OnRep sie nicht doppelt abspielt. */
	uint8 PredictedSerial = 0;
	bool bHasPrediction = false;
};
