// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "UObject/StrongObjectPtr.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "UI/EwigfelsCharacterCreator.h"

class SVerticalBox;
class SEditableTextBox;
struct FSlateBrush;

/**
 * Bedienfeld des Charakter-Editors (reines Slate, kein UMG-Asset) im Stil eines Kleiderschranks:
 * Bereiche oben (LB/RB oder Klick), darunter große Bild-Kacheln und runde Farbtupfer, unten Weiter/Fertig.
 * Bunt mit magischem Touch: Farbverlauf, schwebende Funken, leuchtender Rahmen um Auswahl und Fokus.
 *
 * Controller und Maus/Tastatur gehen gleich gut:
 *  - Controller: Steuerkreuz/linker Stick wählen, A nimmt, B zurück, X rückgängig, Y Zufall, Start weiter/fertig,
 *    LB/RB Bereich, rechter Stick dreht die Figur, LT/RT zoomen.
 *  - Maus/Tastatur: klicken, Ziehen dreht, Mausrad zoomt, Q/E Bereich, Strg+Z/Strg+Y, Esc zurück, Enter wählt.
 * Die Hinweise unten zeigen immer die Tasten des Geräts, das zuletzt benutzt wurde.
 *
 * Alle Werte hängen über Lambdas an der Arbeitskopie von UEwigfelsCreatorSubsystem (Rückgängig und Zufall zeigen sich
 * ohne Neuaufbau). Neu aufgebaut wird nur beim Bereichs- oder Kleidungs-Slot-Wechsel.
 */
class SEwigfelsCreatorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEwigfelsCreatorWidget) {}
		SLATE_ARGUMENT(UEwigfelsCreatorSubsystem*, Creator)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** Eine Kachel im Raster. */
	struct FTile
	{
		FText Label;
		/** Klein unter dem Namen (Preis). */
		FText Note;
		FLinearColor Color = FLinearColor::White;
		/** Bild (Vorschau) oder nullptr → farbiger Kreis mit Anfangsbuchstabe. */
		const FSlateBrush* Image = nullptr;
		/** Kreis in dieser Farbe statt Buchstabe (Köpfe: Hautton, Metalle). */
		bool bSolidCircle = false;
		TFunction<bool()> IsSelected;
		TFunction<void()> OnPick;
	};

private:
	using FGetName = TFunction<FName(const FEwigfelsAppearance&)>;
	using FSetName = TFunction<void(FEwigfelsAppearance&, FName)>;
	using FGetColor = TFunction<FColor(const FEwigfelsAppearance&)>;
	using FSetColor = TFunction<void(FEwigfelsAppearance&, FColor)>;

	UEwigfelsCreatorSubsystem* C() const { return Creator.Get(); }

	TSharedRef<SWidget> MakeTabBar();
	TSharedRef<SWidget> MakeStepDots();
	TSharedRef<SWidget> MakeBottomBar();
	TSharedRef<SWidget> MakeHints();
	void RebuildPanel();

	void BuildHead();
	void BuildHair();
	void BuildBody();
	void BuildMarks();
	void BuildOutfit();
	void BuildRing();
	void BuildVoice();
	void BuildName();

	// Bausteine (hängen sich an PanelBox)
	void AddSection(const FText& Title);
	void AddInfo(const FText& Text);
	void AddTiles(const TArray<FTile>& Tiles, bool bSmall = false);
	/** Kacheln aller wählbaren Optionen einer Kategorie. OnPicked läuft nach dem Setzen (z. B. Stimmprobe). */
	void AddOptionTiles(FName Category, FName SlotFilter, FGetName Get, FSetName Set, TFunction<void()> OnPicked = nullptr);
	void AddSwatches(const FText& Title, FName Palette, FGetColor Get, FSetColor Set, bool bGlow = false);

	TSharedRef<SWidget> MakeTile(const FTile& Tile, bool bSmall);
	TSharedRef<SWidget> MakePill(const FText& Label, const FText& PadKey, FLinearColor Color, TFunction<void()> OnClick,
		TFunction<bool()> IsEnabled = nullptr, TFunction<bool()> IsVisible = nullptr);

	/** Optionen einer Kategorie, die hier wählbar sind (plus die aktuelle, auch wenn sie es nicht mehr wäre). */
	TArray<const FEwigfelsLookOption*> GetChoices(FName Category, FName SlotFilter, FName Current) const;
	FText PriceNote(const FEwigfelsLookOption& Option) const;
	const FSlateBrush* GetThumb(const FEwigfelsLookOption& Option);

	/** Weiter (geführt) bzw. Übernehmen. */
	void PrimaryAction();
	/** Zurück (geführt) bzw. Abbrechen. */
	void BackAction();
	void StepTab(int32 Direction);
	void RequestFocus(const TSharedPtr<SWidget>& Candidate, bool bSelected);

	TWeakObjectPtr<UEwigfelsCreatorSubsystem> Creator;
	TSharedPtr<SVerticalBox> PanelBox;
	TSharedPtr<SEditableTextBox> NameBox;
	FName OutfitSlot = TEXT("oberkoerper");
	bool bRebuildPending = false;
	bool bFocusPending = false;
	EEwigfelsCreatorTab SeenTab = EEwigfelsCreatorTab::Count;

	/** Zuletzt benutztes Gerät: Controller oder Maus/Tastatur (für die Tasten-Hinweise). */
	bool bGamepad = false;
	FVector2D RightStick = FVector2D::ZeroVector;
	float TriggerZoom[2] = { 0.f, 0.f };

	/** Wohin der Fokus nach dem Neuaufbau springt: gewählte Kachel, sonst die erste. */
	TSharedPtr<SWidget> FocusFirst;
	TSharedPtr<SWidget> FocusSelected;

	/** Vorschaubilder (bleiben geladen, solange das Fenster offen ist). */
	TMap<FName, TSharedPtr<FSlateBrush>> ThumbBrushes;
	TArray<TStrongObjectPtr<UObject>> ThumbTextures;
};
