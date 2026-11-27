// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EwigfelsSpellbookComponent.generated.h"

class AEwigfelsCharacter;
class APlayerController;
class UEnhancedInputComponent;
class UInputAction;
class UInputMappingContext;
class UEwigfelsSpellCasterComponent;

/**
 * Zauberbuch: aus allen gelernten Weisen die 6 fürs Zauberrad wählen (eigener Kampfstil).
 * Nur lokale Menü-Logik; die Belegung selbst ändert der Server (UEwigfelsSpellCasterComponent::RequestSetWheelSlot).
 *
 * Bedienung, Controller und Maus/Tastatur gleichwertig:
 *   oben 6 Rad-Plätze, unten große Karten zum Blättern (wie ein Kleiderschrank).
 *   Steuerkreuz/Stick/Pfeile/WASD bewegen, A/Enter/Leertaste wählt, B/Esc zurück, LB/RB/Mausrad blättert.
 *   Maus: Platz anklicken, dann Karte anklicken. Tastatur: über einer Karte 1–6 drücken = direkt auf den Platz.
 * Solange das Buch offen ist, liegt ein eigener Eingabe-Kontext mit hoher Priorität oben und schluckt die Tasten,
 * damit die Figur nicht springt oder läuft (auch wenn ein Blueprint Bewegung steuert).
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsSpellbookComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsSpellbookComponent();

	/** Aktionen an die Eingabe der Figur hängen (aus SetupPlayerInputComponent). */
	void BindInput(UEnhancedInputComponent* Input);

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Spellbook")
	void Toggle();

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Spellbook")
	void Open();

	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Spellbook")
	void Close();

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Spellbook")
	bool IsOpen() const { return bOpen; }

	/** Karten: gelernte Weisen ohne Basis und Schild, in fester Reihenfolge. */
	TArray<FName> GetCards() const;

	/** 0 = Platzreihe oben, 1 = Kartenreihe. */
	int32 GetFocusRow() const { return FocusRow; }
	int32 GetFocusedSlot() const { return FocusSlot; }
	int32 GetFocusedCard() const { return FocusCard; }

	/** Kurzes Aufleuchten eines Platzes nach dem Belegen (Zeit in Echtzeit-Sekunden). */
	int32 GetFlashSlot() const { return FlashSlot; }
	float GetFlashTime() const { return FlashTime; }

	/** Das HUD meldet jedes Bild, wo es Plätze und Karten gezeichnet hat (für die Maus). */
	void SetHitRects(const TArray<FBox2D>& InSlotRects, const TArray<FBox2D>& InCardRects);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CreateActions();
	void Move(int32 DX, int32 DY);
	void InputLeft()  { Move(-1, 0); }
	void InputRight() { Move(1, 0); }
	void InputUp()    { Move(0, -1); }
	void InputDown()  { Move(0, 1); }
	void InputPrev()  { Move(-1, 0); }
	void InputNext()  { Move(1, 0); }
	void InputConfirm();
	void InputBack();
	void InputClick();
	void InputSlotKey(int32 Slot);
	void Assign(int32 Slot, int32 CardIndex);
	void UpdateHoverFromMouse();
	void SetMenuMode(bool bMenu);

	AEwigfelsCharacter* GetCharacter() const;
	UEwigfelsSpellCasterComponent* GetCaster() const;
	APlayerController* GetPC() const;

	UPROPERTY(Transient) TObjectPtr<UInputMappingContext> MenuContext;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActLeft;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActRight;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActUp;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActDown;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActPrev;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActNext;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActConfirm;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActBack;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActClick;
	UPROPERTY(Transient) TObjectPtr<UInputAction> ActSwallow;
	UPROPERTY(Transient) TArray<TObjectPtr<UInputAction>> ActSlots;

	bool bOpen = false;
	int32 FocusRow = 0;
	int32 FocusSlot = 1;
	int32 FocusCard = 0;
	int32 FlashSlot = 0;
	float FlashTime = -10.f;
	TArray<FBox2D> SlotRects;
	TArray<FBox2D> CardRects;
	FVector2D LastMouse = FVector2D(-1.f, -1.f);
};
