// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "UI/EwigfelsCharacterCreator.h"

class SVerticalBox;
class SHorizontalBox;

/**
 * Bedienfeld des Charakter-Editors (reines Slate, kein UMG-Asset): Reiter links, Einstellungen rechts,
 * Knöpfe unten. Ziehen mit der Maus dreht die Kamera um die Figur, Mausrad zoomt.
 * Alle Werte hängen über Lambdas an der Arbeitskopie von UEwigfelsCreatorSubsystem – Rückgängig, Zufall und
 * Look-Code-Import zeigen sich ohne Neuaufbau. Neu aufgebaut wird nur beim Reiterwechsel.
 */
class SEwigfelsCreatorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEwigfelsCreatorWidget) {}
		SLATE_ARGUMENT(UEwigfelsCreatorSubsystem*, Creator)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	using FGetName = TFunction<FName(const FEwigfelsAppearance&)>;
	using FSetName = TFunction<void(FEwigfelsAppearance&, FName)>;
	using FGetFloat = TFunction<float(const FEwigfelsAppearance&)>;
	using FSetFloat = TFunction<void(FEwigfelsAppearance&, float)>;
	using FGetColor = TFunction<FColor(const FEwigfelsAppearance&)>;
	using FSetColor = TFunction<void(FEwigfelsAppearance&, FColor)>;

	UEwigfelsCreatorSubsystem* C() const { return Creator.Get(); }

	void RebuildTabs();
	void RebuildPanel();
	TSharedRef<SWidget> MakeBottomBar();

	void BuildIdentity();
	void BuildBody();
	void BuildFace();
	void BuildSkinEyes();
	void BuildHair();
	void BuildMarks();
	void BuildVoice();
	void BuildOutfit();
	void BuildRing();

	// Bausteine (hängen sich an PanelBox)
	void AddHeader(const FText& Text);
	void AddInfo(const FText& Text);
	void AddPicker(const FText& Label, FName Category, FName SlotFilter, FGetName Get, FSetName Set);
	void AddSlider(const FText& Label, float Min, float Max, FGetFloat Get, FSetFloat Set, TFunction<FText(float)> Format = nullptr);
	void AddColor(const FText& Label, FName Palette, FGetColor Get, FSetColor Set);
	void AddChoices(const FText& Label, const TArray<FText>& Choices, TFunction<int32(const FEwigfelsAppearance&)> Get, TFunction<void(FEwigfelsAppearance&, int32)> Set);
	void AddCheck(const FText& Label, TFunction<bool(const FEwigfelsAppearance&)> Get, TFunction<void(FEwigfelsAppearance&, bool)> Set);
	void AddButton(const FText& Label, TFunction<void()> OnClick);

	/** Optionen einer Kategorie, die hier wählbar sind (plus die aktuelle, auch wenn sie es nicht mehr wäre). */
	TArray<const FEwigfelsLookOption*> GetChoices(FName Category, FName SlotFilter, FName Current) const;
	FText OptionLabel(const FEwigfelsLookOption* Option) const;

	TWeakObjectPtr<UEwigfelsCreatorSubsystem> Creator;
	TSharedPtr<SVerticalBox> TabBox;
	TSharedPtr<SVerticalBox> PanelBox;
	FName OutfitSlot = TEXT("oberkoerper");
	TSet<FString> ExpandedColors;
	bool bRebuildPending = false;
	int32 ColorRowCounter = 0;
};
