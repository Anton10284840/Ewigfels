// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat/EwigfelsCombatTypes.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "EwigfelsCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UEwigfelsCombatComponent;
class UEwigfelsSpellCasterComponent;
class UEwigfelsLockOnComponent;
class UEwigfelsPlaceholderBody;
class UEwigfelsDialogueCameraComponent;
class UEwigfelsInputSetup;
class UEwigfelsCharacterMovementComponent;
class UAnimMontage;
struct FInputActionValue;

/**
 * Spielfigur (Third Person). Bewegung, Kamera, Zielaufschaltung, Ausweichen; Weisen über
 * UEwigfelsSpellCasterComponent, Leben/Status über UEwigfelsCombatComponent.
 *
 * Optik:
 *   - Ohne Mesh: Platzhalter-Körper (sofort spielbar).
 *   - Mit Mesh: in DefaultGame.ini DefaultMesh/DefaultAnimClass setzen (z. B. Mannequin aus dem Game Animation Sample)
 *     oder einen Blueprint von dieser Klasse ableiten (BP_NocturnaPlayer) bzw. CBP_SandboxCharacter auf sie umhängen
 *     (Anleitung: docs/PHASE3_EDITOR.md).
 *
 * Netzwerk: Bewegung über die (vorhergesagte) Character Movement; Ausweichen als Root-Motion-Quelle auf Client und
 * Server; alles Spielrelevante (Atem, Schaden, i-Frames) entscheidet der Server.
 */
UCLASS(Config = Game)
class EWIGFELS_API AEwigfelsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEwigfelsCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void NotifyControllerChanged() override;
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	UEwigfelsCombatComponent* GetCombat() const { return Combat; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	UEwigfelsSpellCasterComponent* GetCaster() const { return Caster; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	UEwigfelsLockOnComponent* GetLockOn() const { return LockOn; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	UEwigfelsDialogueCameraComponent* GetDialogueCamera() const { return DialogueCamera; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	UEwigfelsCharacterMovementComponent* GetEwigfelsMovement() const;

	/** Strafen statt in Laufrichtung drehen (Lock-On, Zaubern, Schild). Für das AnimBP. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	bool IsStrafing() const { return bStrafing; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	bool IsDodging() const;

	/** Nächstes Objekt, mit dem E etwas tut (HUD zeigt den Hinweis). Nur lokal. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Character")
	AActor* GetFocusedInteractable() const { return FocusedInteractable.Get(); }

	/** Steuerung aus (Dialog, Menü). Kamera bleibt. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Character")
	void SetGameplayInputBlocked(bool bBlocked);

	bool IsGameplayInputBlocked() const { return bInputBlocked; }

	/** Zielpunkt der Kamera-Mitte (für Weisen). Nur lokal sinnvoll. */
	FVector ComputeAimPoint() const;

	bool IsHelpVisible() const { return bShowHelp; }

	/** Kurzer Hinweis in der Bildmitte („Kein Atem“, „Reinklang!“). Nur lokal. */
	void ShowMessage(const FText& Text, const FLinearColor& Color, float Seconds = 1.2f);

	/** Aktueller Hinweis, false wenn abgelaufen. Alpha blendet aus. */
	bool GetMessage(FText& OutText, FLinearColor& OutColor, float& OutAlpha) const;

	// --- Einstellungen (DefaultGame.ini: [/Script/Ewigfels.EwigfelsCharacter]) ---

	/** Skeletal Mesh, das C++ beim Start setzt, wenn im Blueprint keins zugewiesen ist. Leer = Platzhalter. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Look")
	FSoftObjectPath DefaultMesh;

	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Look")
	FSoftClassPath DefaultAnimClass;

	/** false, wenn ein Blueprint (z. B. aus dem Game Animation Sample) Bewegung und Kamera selbst steuert. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Input")
	bool bUseBuiltInMovementInput = true;

	/** true, wenn der Blueprint eine eigene Kamera mitbringt (GameplayCameraComponent des Game Animation Sample).
	 *  Dann wird CameraBoom/FollowCamera nicht nachgeführt; Zielaufschaltung wirkt über die Control Rotation. */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Camera")
	bool UsesExternalCamera() const { return bExternalCamera; }

	/** Eigene Input-Assets statt der C++-Steuerung (optional). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Input")
	float MouseSensitivity = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Input")
	float GamepadLookRate = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Dodge")
	float DodgeDistance = 520.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Dodge")
	float DodgeDuration = 0.32f;

	/** Unverwundbar ab Beginn des Ausweichens (i-Frames). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Dodge")
	float DodgeInvulnerability = 0.28f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Dodge")
	float DodgeAtemCost = 12.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Dodge")
	float DodgeCooldown = 0.45f;

	/** Wieder aufstehen nach dem Tod (Testarena). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Combat")
	float RespawnDelay = 3.f;

protected:
	// Komponenten
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Character")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Character")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Character")
	TObjectPtr<UEwigfelsCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Character")
	TObjectPtr<UEwigfelsSpellCasterComponent> Caster;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Character")
	TObjectPtr<UEwigfelsLockOnComponent> LockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Character")
	TObjectPtr<UEwigfelsDialogueCameraComponent> DialogueCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ewigfels|Character")
	TObjectPtr<UEwigfelsPlaceholderBody> PlaceholderBody;

	// Eingabe
	void InputMove(const FInputActionValue& Value);
	void InputLook(const FInputActionValue& Value);
	void InputLookStick(const FInputActionValue& Value);
	void InputJump();
	void InputJumpReleased();
	void InputSprint(const FInputActionValue& Value);
	void InputDodge();
	void InputCastBasic();
	void InputShieldStart();
	void InputShieldEnd();
	void InputSpell1() { CastSlot(1); }
	void InputSpell2() { CastSlot(2); }
	void InputSpell3() { CastSlot(3); }
	void InputSpell4() { CastSlot(4); }
	void InputLockOn();
	void InputSwitchTarget(const FInputActionValue& Value);
	void InputInteract();
	void InputToggleHelp();
	void CastSlot(int32 Slot);

	// Netzwerk
	UFUNCTION(Server, Reliable)
	void Server_Dodge(FVector_NetQuantizeNormal Direction);

	UFUNCTION(Server, Reliable)
	void Server_SetLockedOn(bool bLocked);

	UFUNCTION(Server, Reliable)
	void Server_ReportTutorial(FName Step);

	/** Magische Landung für alle anderen sichtbar machen (Server und eigener Client spielen sie selbst). */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_FallCatchFx();

	// Ereignisse aus den Komponenten
	UFUNCTION()
	void HandleCastStarted(FName SpellId, uint8 ChainIndex, float CastTime);

	UFUNCTION()
	void HandleSpellReleased(FName SpellId, FVector Origin, FVector Direction);

	UFUNCTION()
	void HandleCastFailed(FName SpellId, EEwigfelsCastFailReason Reason);

	UFUNCTION()
	void HandleShieldChanged(bool bUp);

	UFUNCTION()
	void HandleParried(AActor* Attacker, FVector Location);

	UFUNCTION()
	void HandleHitFeedback(float Amount, FVector Location, EEwigfelsHitResult Result, AActor* InstigatorActor);

	UFUNCTION()
	void HandleDied(AActor* Killer);

	UFUNCTION()
	void HandleLockOnChanged(AActor* NewTarget);

	UFUNCTION()
	void HandleStaggered(float Seconds);

	UFUNCTION()
	void HandleBeamFired(FName SpellId, FVector Start, FVector End, bool bHitTarget, uint8 ChainIndex);

	/** Montage für die Abfang-Geste beim Fallen (optional, sonst nur Licht und Klangring). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Look")
	FSoftObjectPath FallCatchMontage = FSoftObjectPath(TEXT("/Game/Animations/Magic/AM_Magic_FallCatch.AM_Magic_FallCatch"));

	/** Kamerastoß beim Zaubern/Treffer (Grad Nicken). 0 = aus. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Camera")
	float CameraPunchStrength = 1.f;

private:
	void ApplyDefaultLook();
	void StartDodge(const FVector& Direction);
	void UpdateRotationMode();
	void UpdateCamera(float DeltaSeconds);
	void DetectExternalCamera();
	bool bExternalCamera = false;
	/** Kurzer Kamerastoß über die Control Rotation (wirkt mit jeder Kamera, auch der des Samples). Nur lokal. */
	void AddCameraPunch(float PitchDeg, float YawDeg);
	void UpdateCameraPunch(float DeltaSeconds);
	/** Abklingendes Zittern (Grad), 150–400 ms. */
	void AddCameraShake(float AmplitudeDeg, float Seconds);
	float ShakeAmp = 0.f;
	float ShakeDuration = 0.f;
	float ShakeAge = -1.f;
	void PlayFallCatchFx(bool bLanding);
	FRotator PunchApplied = FRotator::ZeroRotator;
	FRotator PunchPeak = FRotator::ZeroRotator;
	float PunchAge = -1.f;
	bool bWasFallCatching = false;
	float LastFallCatchFxTime = -10.f;
	void UpdateInteractFocus();
	void Respawn();
	void NoteTutorialStep(FName Step);
	void PlaySpellMontage(FName SpellId);

	UPROPERTY(Transient)
	TObjectPtr<UEwigfelsInputSetup> InputSetup;

	UPROPERTY(Replicated)
	bool bLockedOnReplicated = false;

	bool bStrafing = false;
	bool bInputBlocked = false;
	bool bShowHelp = true;
	float LastDodgeTime = -100.f;
	float LocalCastPoseUntil = 0.f;
	float DefaultArmLength = 340.f;
	float DefaultFov = 80.f;
	FVector DefaultSocketOffset = FVector::ZeroVector;
	TWeakObjectPtr<AActor> FocusedInteractable;
	TSet<FName> TutorialSteps;
	bool bTutorialReported = false;
	FText MessageText;
	FLinearColor MessageColor = FLinearColor::White;
	float MessageStart = -100.f;
	float MessageLength = 1.f;
	FTimerHandle RespawnTimer;
};
