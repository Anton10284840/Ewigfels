// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "EwigfelsCharacterCreator.generated.h"

class ACameraActor;
class APointLight;
class SWidget;
class SEwigfelsCreatorWidget;
class UEwigfelsAppearanceComponent;
class UEwigfelsAppearanceSubsystem;

/** Seiten des Editors (Reiter links). */
UENUM()
enum class EEwigfelsCreatorTab : uint8
{
	Identity,  // Name + Anrede
	Body,
	Face,
	SkinEyes,
	Hair,
	Marks,
	Voice,
	Outfit,
	Ring
};

/**
 * Charakter-Editor und Laden-Spiegel für einen lokalen Spieler (Koop am selben Bildschirm: jeder hat seinen eigenen).
 * Hält die Arbeitskopie des Aussehens mit Rückgängig/Wiederholen, steuert Vorschau-Kamera und Licht und zeigt das
 * Slate-Fenster (kein UMG-Asset nötig). Übernehmen läuft über UEwigfelsAppearanceComponent::RequestChange (Server prüft).
 *
 * Öffnet sich automatisch bei neuer Figur (Spielbeginn) und über AEwigfelsMirror in Läden.
 * Konsole: Ewigfels.Look.Editor [LadenId]
 */
UCLASS()
class EWIGFELS_API UEwigfelsCreatorSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	/** Öffnet den Editor für die Figur. ShopId None = Erstellung zu Spielbeginn. */
	bool Open(UEwigfelsAppearanceComponent* Target, FName ShopId);

	/** Schließt ohne zu übernehmen (Vorschau wird zurückgesetzt). Bei der Erstellung nicht möglich. */
	void Cancel();

	bool IsOpen() const { return Target.IsValid(); }
	bool IsCreation() const { return ShopId.IsNone(); }
	FName GetShopId() const { return ShopId; }

	// --- Für das Fenster ---
	const FEwigfelsAppearance& GetWorking() const { return Working; }
	const FEwigfelsLookCatalog& GetCatalog() const;
	const FEwigfelsLookContext& GetContext() const { return Context; }
	const FEwigfelsLookShop* GetShop() const;

	/** Ändern mit Rückgängig-Schritt. */
	void Edit(TFunctionRef<void(FEwigfelsAppearance&)> Change);
	/** Ändern ohne neuen Schritt (während ein Regler gezogen wird). Vorher BeginDrag aufrufen. */
	void EditLive(TFunctionRef<void(FEwigfelsAppearance&)> Change);
	void BeginDrag();

	void Undo();
	void Redo();
	bool CanUndo() const { return UndoStack.Num() > 0; }
	bool CanRedo() const { return RedoStack.Num() > 0; }
	void Randomize();
	void ResetToOriginal();

	/** Darf der Bereich (EwigfelsLook::Change / "outfit:<slot>") hier geändert werden? */
	bool IsAreaEditable(FName Area) const;
	bool IsTabVisible(EEwigfelsCreatorTab Tab) const;
	bool IsOptionAvailable(const FEwigfelsLookOption& Option) const;

	/** Preis und Gültigkeit der aktuellen Änderung (lokal vorgerechnet, Server prüft noch einmal). */
	bool PreviewCheck(int32& OutPrice, FText& OutReason) const;
	/** Verfügbares Geld (-1 = unbekannt). */
	int32 GetMoney() const;

	/** Übernehmen: schickt die Änderung an den Server. Antwort schließt das Fenster oder zeigt den Grund. */
	void Confirm();
	bool IsWaitingForServer() const { return bWaiting; }
	FText GetStatusText() const { return StatusText; }

	void SetTab(EEwigfelsCreatorTab Tab);
	EEwigfelsCreatorTab GetTab() const { return CurrentTab; }

	/** Kamera: drehen (Maus ziehen) und zoomen (Mausrad). */
	void Orbit(float DeltaYaw, float DeltaPitch);
	void Zoom(float Delta);

	/** Stimmprobe der aktuellen Stimme abspielen. */
	void PlayVoiceSample() const;

	/** Änderungszähler, damit das Fenster weiß, wann es neu zeichnen muss (Rückgängig, Zufall). */
	uint32 GetRevision() const { return Revision; }

private:
	UFUNCTION()
	void HandleChangeResult(bool bAccepted, int32 Price, const FText& Reason);

	UFUNCTION()
	void HandleAppearanceApplied(const FEwigfelsAppearance& Look);

	void ApplyPreview();
	void Close();
	void SpawnStage();
	void DestroyStage();
	void UpdateCamera(bool bInstant);
	UEwigfelsAppearanceSubsystem* GetLookSubsystem() const;

	TWeakObjectPtr<UEwigfelsAppearanceComponent> Target;
	FName ShopId;
	FEwigfelsLookContext Context;
	FEwigfelsAppearance Original;
	FEwigfelsAppearance Working;
	TArray<FEwigfelsAppearance> UndoStack;
	TArray<FEwigfelsAppearance> RedoStack;
	EEwigfelsCreatorTab CurrentTab = EEwigfelsCreatorTab::Identity;
	bool bWaiting = false;
	FText StatusText;
	uint32 Revision = 0;

	// Bühne
	UPROPERTY(Transient) TObjectPtr<ACameraActor> Camera;
	UPROPERTY(Transient) TObjectPtr<APointLight> KeyLight;
	UPROPERTY(Transient) TObjectPtr<APointLight> RimLight;
	float OrbitYaw = 0.f;
	float OrbitPitch = -5.f;
	float ZoomAlpha = 0.f;

	TSharedPtr<SEwigfelsCreatorWidget> Widget;
	TSharedPtr<SWidget> WidgetContainer;
};
