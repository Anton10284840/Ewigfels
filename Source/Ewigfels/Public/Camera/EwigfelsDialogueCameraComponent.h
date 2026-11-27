// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EwigfelsDialogueCameraComponent.generated.h"

class ACineCameraActor;

UENUM(BlueprintType)
enum class EEwigfelsDialogueShot : uint8
{
	/** Totale bei Szenenbeginn (24 mm). */
	Wide,
	/** Über die Schulter des Zuhörers auf den Sprecher (40 mm). */
	OverShoulder,
	/** Nah auf den Sprecher bei starker Emotion (85 mm). */
	CloseUp
};

/** Eine Zeile im Test-/Platzhalterdialog. Echte Dialoge kommen aus SUDS (Phase 2), die Kamera hört dann auf Cues. */
USTRUCT(BlueprintType)
struct EWIGFELS_API FEwigfelsDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Dialogue")
	FText Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Dialogue")
	FText Text;

	/** true = das Gegenüber (NPC) spricht, false = der Spieler. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Dialogue")
	bool bPartnerSpeaks = true;

	/** "wide", "ots", "closeup" oder leer = automatisch. Wie "camera" im Dialog-JSON. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Dialogue")
	FName Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Dialogue")
	FName Emotion;

	/** 0–1. Ab 0,7 automatisch Close-up (ANIMATION_BIBLE §5.1). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ewigfels|Dialogue")
	float Intensity = 0.3f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEwigfelsDialogueCameraEvent, bool, bActive);

/**
 * Automatische Dialog-Kamera (ANIMATION_BIBLE §5.1): Totale zu Beginn, Schuss-Gegenschuss über die Schulter,
 * Close-up bei starker Emotion. Hält die 180°-Regel (alle Kameras auf derselben Seite der Sprecherachse,
 * Seite = wo die Spielkamera beim Start stand). Schärfe auf dem Auge des Sprechenden, Blende f/2.
 * Rein lokal (jeder Spieler hat seine eigene Kamera; im Koop sieht der Mitspieler normal weiter).
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsDialogueCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsDialogueCameraComponent();

	/** Startet die Kamera mit Partner. Lines optional (leer = Steuerung von außen über SetShot/HandleCue). */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Dialogue")
	void BeginDialogue(AActor* Partner, const TArray<FEwigfelsDialogueLine>& Lines);

	/** Nächste Zeile (E/X). Nach der letzten endet der Dialog. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Dialogue")
	void Advance();

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Dialogue")
	void EndDialogue();

	/** Schnitt auf eine Einstellung. bOnPartner = auf das Gegenüber, sonst auf den Spieler. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Dialogue")
	void SetShot(EEwigfelsDialogueShot Shot, bool bOnPartner);

	/** Cue aus einem SUDS-Dialog ([event camera `closeup`], [event emotion `angry`]) – siehe UEwigfelsDialogueBridge::OnCue. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Dialogue")
	void HandleCue(FName Cue, const TArray<FString>& Arguments);

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Dialogue")
	bool IsActive() const { return bActive; }

	/** Aktuelle Zeile für Untertitel (false, wenn keine). */
	bool GetCurrentLine(FEwigfelsDialogueLine& OutLine) const;

	/**
	 * Reine Rechnung (getestet): Kameraposition/-drehung für eine Einstellung.
	 * ListenerEye/SpeakerEye = Augenpunkte, SideSign = ±1 (Seite der Achse), OutFocal = Brennweite in mm.
	 */
	static FTransform ComputeShot(EEwigfelsDialogueShot Shot, const FVector& ListenerEye, const FVector& SpeakerEye, float SideSign, float& OutFocal);

	/** Automatische Wahl nach ANIMATION_BIBLE §5.1. */
	static EEwigfelsDialogueShot ChooseShot(const FEwigfelsDialogueLine& Line, bool bFirstLine);

	/** Augenpunkt einer Figur (Socket FACIAL_L_Eye, sonst head, sonst Kapsel oben). */
	static FVector GetEyeLocation(const AActor* Actor);

	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Dialogue")
	FEwigfelsDialogueCameraEvent OnDialogueCameraChanged;

	/** Weiche Blende beim Ein-/Ausstieg, dazwischen harte Schnitte. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Dialogue")
	float EnterBlendTime = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Dialogue")
	float Aperture = 2.f;

private:
	void ShowLine(int32 Index);
	void ApplyCamera(const FTransform& Transform, float Focal, const FVector& FocusPoint, float Blend);
	ACineCameraActor* EnsureCamera();
	class APlayerController* GetPC() const;

	UPROPERTY(Transient)
	TObjectPtr<ACineCameraActor> CameraActor;

	TWeakObjectPtr<AActor> PartnerActor;
	TArray<FEwigfelsDialogueLine> Script;
	int32 LineIndex = INDEX_NONE;
	float Side = 1.f;
	bool bActive = false;
	bool bFirstCut = true;
};
