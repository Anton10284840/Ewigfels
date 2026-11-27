// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "UI/SEwigfelsCreatorWidget.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "EwigfelsCreator"

namespace
{
	const FLinearColor CrGold(1.f, 0.8f, 0.45f);
	const FLinearColor CrInk(0.015f, 0.02f, 0.045f, 0.84f);
	const FLinearColor CrTextColor(0.93f, 0.91f, 0.86f);
	const FLinearColor CrMuted(0.62f, 0.63f, 0.7f);
	const FLinearColor CrWarn(1.f, 0.55f, 0.45f);

	FSlateFontInfo CrFont(int32 Size, bool bBold = false)
	{
		return FCoreStyle::GetDefaultFontStyle(bBold ? "Bold" : "Regular", Size);
	}

	const FSlateBrush* CrWhite() { return FCoreStyle::Get().GetBrush("WhiteBrush"); }

	FText CrTabName(EEwigfelsCreatorTab Tab)
	{
		switch (Tab)
		{
		case EEwigfelsCreatorTab::Identity: return LOCTEXT("TabIdentity", "Wer bist du?");
		case EEwigfelsCreatorTab::Body: return LOCTEXT("TabBody", "Körper");
		case EEwigfelsCreatorTab::Face: return LOCTEXT("TabFace", "Gesicht");
		case EEwigfelsCreatorTab::SkinEyes: return LOCTEXT("TabSkin", "Haut & Augen");
		case EEwigfelsCreatorTab::Hair: return LOCTEXT("TabHair", "Haare & Bart");
		case EEwigfelsCreatorTab::Marks: return LOCTEXT("TabMarks", "Male & Schminke");
		case EEwigfelsCreatorTab::Voice: return LOCTEXT("TabVoice", "Stimme");
		case EEwigfelsCreatorTab::Outfit: return LOCTEXT("TabOutfit", "Kleidung");
		case EEwigfelsCreatorTab::Ring: return LOCTEXT("TabRing", "Stimmring");
		}
		return FText::GetEmpty();
	}

	FText CrSlotName(FName Slot)
	{
		static const TMap<FName, FText> Names = {
			{ TEXT("kopf"), LOCTEXT("SlotKopf", "Kopf") }, { TEXT("gesicht"), LOCTEXT("SlotGesicht", "Gesicht") },
			{ TEXT("hals"), LOCTEXT("SlotHals", "Hals") }, { TEXT("oberkoerper"), LOCTEXT("SlotOber", "Oberkörper") },
			{ TEXT("umhang"), LOCTEXT("SlotUmhang", "Umhang") }, { TEXT("guertel"), LOCTEXT("SlotGuertel", "Gürtel") },
			{ TEXT("handschuhe"), LOCTEXT("SlotHand", "Handschuhe") }, { TEXT("beine"), LOCTEXT("SlotBeine", "Beine") },
			{ TEXT("schuhe"), LOCTEXT("SlotSchuhe", "Schuhe") }, { TEXT("tasche"), LOCTEXT("SlotTasche", "Tasche") } };
		const FText* Found = Names.Find(Slot);
		return Found ? *Found : FText::FromName(Slot);
	}

	FText CrKindName(FName Kind)
	{
		if (Kind == TEXT("freckles")) { return LOCTEXT("KindFreckles", "Sommersprossen & Male"); }
		if (Kind == TEXT("scar")) { return LOCTEXT("KindScar", "Narben"); }
		if (Kind == TEXT("makeup")) { return LOCTEXT("KindMakeup", "Schminke"); }
		if (Kind == TEXT("tattoo")) { return LOCTEXT("KindTattoo", "Tattoos"); }
		if (Kind == TEXT("paint")) { return LOCTEXT("KindPaint", "Bemalung"); }
		if (Kind == TEXT("rune")) { return LOCTEXT("KindRune", "Klang-Runen"); }
		return FText::FromName(Kind);
	}

	FName CrOutfitArea(FName Slot) { return FName(*(TEXT("outfit:") + Slot.ToString())); }
	FName CrOutfitColorArea(FName Slot) { return FName(*(TEXT("outfit_colors:") + Slot.ToString())); }

	/** Unsichtbare Fläche hinter dem Bedienfeld: Ziehen dreht die Kamera, Mausrad zoomt. */
	class SEwigfelsOrbitArea : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SEwigfelsOrbitArea) {}
			SLATE_ARGUMENT(TWeakObjectPtr<UEwigfelsCreatorSubsystem>, Creator)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs)
		{
			Creator = InArgs._Creator;
			SetVisibility(EVisibility::Visible);
			ChildSlot[SNullWidget::NullWidget];
		}

		virtual FReply OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override
		{
			if (Event.GetEffectingButton() == EKeys::LeftMouseButton || Event.GetEffectingButton() == EKeys::RightMouseButton)
			{
				return FReply::Handled().CaptureMouse(SharedThis(this));
			}
			return FReply::Unhandled();
		}

		virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override
		{
			return HasMouseCapture() ? FReply::Handled().ReleaseMouseCapture() : FReply::Unhandled();
		}

		virtual FReply OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event) override
		{
			if (HasMouseCapture() && Creator.IsValid())
			{
				const FVector2D Delta = Event.GetCursorDelta();
				Creator->Orbit(Delta.X * 0.4f, -Delta.Y * 0.25f);
				return FReply::Handled();
			}
			return FReply::Unhandled();
		}

		virtual FReply OnMouseWheel(const FGeometry& Geometry, const FPointerEvent& Event) override
		{
			if (Creator.IsValid())
			{
				Creator->Zoom(Event.GetWheelDelta() * 0.12f);
			}
			return FReply::Handled();
		}

	private:
		TWeakObjectPtr<UEwigfelsCreatorSubsystem> Creator;
	};
}

// ---------------------------------------------------------------------------
// Aufbau

void SEwigfelsCreatorWidget::Construct(const FArguments& InArgs)
{
	Creator = InArgs._Creator;
	UEwigfelsCreatorSubsystem* Cr = C();
	const bool bCreation = Cr && Cr->IsCreation();
	const FEwigfelsLookShop* Shop = Cr ? Cr->GetShop() : nullptr;

	const FText Title = bCreation ? LOCTEXT("TitleCreate", "Dein Spiegelbild")
		: Shop ? Shop->DisplayName : LOCTEXT("TitleMirror", "Spiegel");
	const FText Subtitle = bCreation
		? LOCTEXT("SubCreate", "Das Nebelboot schaukelt. Im dunklen Wasser siehst du dich so, wie Nocturna dich gleich sehen wird.")
		: LOCTEXT("SubShop", "Probier in Ruhe. Bezahlt wird erst beim Übernehmen.");

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SEwigfelsOrbitArea).Creator(Creator)
		]
		// Links: Titel + Reiter
		+ SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Top).Padding(24.f, 24.f)
		[
			SNew(SBox).WidthOverride(260.f)
			[
				SNew(SBorder).BorderImage(CrWhite()).BorderBackgroundColor(CrInk).Padding(16.f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock).Text(Title).Font(CrFont(22, true)).ColorAndOpacity(CrGold)
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f, 0.f, 14.f)
					[
						SNew(STextBlock).Text(Subtitle).Font(CrFont(11)).ColorAndOpacity(CrMuted).AutoWrapText(true)
					]
					+ SVerticalBox::Slot().AutoHeight()
					[
						SAssignNew(TabBox, SVerticalBox)
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 14.f, 0.f, 0.f)
					[
						SNew(STextBlock).Text(LOCTEXT("Help", "Maus ziehen: drehen · Mausrad: näher\nStrg+Z / Strg+Y: zurück / vor"))
						.Font(CrFont(10)).ColorAndOpacity(CrMuted)
					]
				]
			]
		]
		// Rechts: Einstellungen
		+ SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Fill).Padding(24.f, 24.f, 24.f, 96.f)
		[
			SNew(SBox).WidthOverride(440.f)
			[
				SNew(SBorder).BorderImage(CrWhite()).BorderBackgroundColor(CrInk).Padding(16.f)
				[
					SNew(SScrollBox)
					+ SScrollBox::Slot()
					[
						SAssignNew(PanelBox, SVerticalBox)
					]
				]
			]
		]
		// Unten: Knöpfe + Status
		+ SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Bottom).Padding(24.f, 0.f, 24.f, 24.f)
		[
			MakeBottomBar()
		]
	];

	RebuildTabs();
	RebuildPanel();
}

void SEwigfelsCreatorWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	if (bRebuildPending)
	{
		bRebuildPending = false;
		RebuildPanel(); // nach dem Klick, nicht mitten im Klick des Knopfs, der gerade neu gebaut wird
	}
}

FReply SEwigfelsCreatorWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr)
	{
		return FReply::Unhandled();
	}
	const FKey Key = InKeyEvent.GetKey();
	if (InKeyEvent.IsControlDown() && Key == EKeys::Z)
	{
		Cr->Undo();
		return FReply::Handled();
	}
	if (InKeyEvent.IsControlDown() && Key == EKeys::Y)
	{
		Cr->Redo();
		return FReply::Handled();
	}
	if (Key == EKeys::Escape)
	{
		Cr->Cancel();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void SEwigfelsCreatorWidget::RebuildTabs()
{
	TabBox->ClearChildren();
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr)
	{
		return;
	}
	for (uint8 i = 0; i <= static_cast<uint8>(EEwigfelsCreatorTab::Ring); ++i)
	{
		const EEwigfelsCreatorTab Tab = static_cast<EEwigfelsCreatorTab>(i);
		if (!Cr->IsTabVisible(Tab))
		{
			continue;
		}
		TabBox->AddSlot().AutoHeight().Padding(0.f, 2.f)
		[
			SNew(SButton)
			.ButtonColorAndOpacity_Lambda([this, Tab]()
			{
				return C() && C()->GetTab() == Tab ? FLinearColor(CrGold.R, CrGold.G, CrGold.B, 0.9f) : FLinearColor(0.25f, 0.27f, 0.36f, 0.9f);
			})
			.OnClicked_Lambda([this, Tab]()
			{
				if (UEwigfelsCreatorSubsystem* Cr2 = C())
				{
					Cr2->SetTab(Tab);
					RebuildPanel();
				}
				return FReply::Handled();
			})
			[
				SNew(STextBlock).Text(CrTabName(Tab)).Font(CrFont(13, true)).ColorAndOpacity(FLinearColor::White)
			]
		];
	}
}

TSharedRef<SWidget> SEwigfelsCreatorWidget::MakeBottomBar()
{
	UEwigfelsCreatorSubsystem* Cr = C();
	const bool bCreation = Cr && Cr->IsCreation();

	auto Button = [](const FText& Label, TFunction<void()> OnClick, TFunction<bool()> IsEnabled, bool bPrimary = false) -> TSharedRef<SWidget>
	{
		return SNew(SBox).MinDesiredWidth(bPrimary ? 200.f : 120.f)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
			.IsEnabled_Lambda([IsEnabled]() { return IsEnabled ? IsEnabled() : true; })
			.ButtonColorAndOpacity(bPrimary ? CrGold : FLinearColor(0.25f, 0.27f, 0.36f, 0.95f))
			.OnClicked_Lambda([OnClick]() { OnClick(); return FReply::Handled(); })
			[
				SNew(STextBlock).Text(Label).Font(CrFont(bPrimary ? 15 : 13, bPrimary)).ColorAndOpacity(FLinearColor::White)
			]
		];
	};

	TSharedRef<SHorizontalBox> Bar = SNew(SHorizontalBox);
	// Status: Fehler, Preis, Geld
	Bar->AddSlot().FillWidth(1.f).VAlign(VAlign_Center)
	[
		SNew(SBorder).BorderImage(CrWhite()).BorderBackgroundColor(CrInk).Padding(FMargin(14.f, 10.f))
		[
			SNew(STextBlock).Font(CrFont(13)).AutoWrapText(true)
			.ColorAndOpacity_Lambda([this]()
			{
				return C() && !C()->GetStatusText().IsEmpty() && !C()->IsWaitingForServer() ? FSlateColor(CrWarn) : FSlateColor(CrTextColor);
			})
			.Text_Lambda([this]()
			{
				UEwigfelsCreatorSubsystem* Cr2 = C();
				if (!Cr2)
				{
					return FText::GetEmpty();
				}
				if (!Cr2->GetStatusText().IsEmpty())
				{
					return Cr2->GetStatusText();
				}
				int32 Price = 0;
				FText Reason;
				const bool bOk = Cr2->PreviewCheck(Price, Reason);
				if (Cr2->IsCreation())
				{
					return bOk ? FText::Format(LOCTEXT("ReadyCreate", "Bereit, {0}? Dann „Fertig“."), FText::FromString(Cr2->GetWorking().Name)) : Reason;
				}
				if (!bOk)
				{
					return Reason;
				}
				const FEwigfelsLookShop* Shop = Cr2->GetShop();
				FText PriceText = Shop && !Shop->PriceItem.IsNone() && Shop->PriceItemCount > 0
					? FText::Format(LOCTEXT("PriceItem", "Preis: {0} × {1}"), Shop->PriceItemCount, FText::FromName(Shop->PriceItem))
					: FText::Format(LOCTEXT("PriceCrowns", "Preis: {0} Kronen"), Price);
				const int32 Money = Cr2->GetMoney();
				return Money >= 0 ? FText::Format(LOCTEXT("PriceMoney", "{0} · Du hast {1}"), PriceText, Money) : PriceText;
			})
		]
	];
	if (bCreation)
	{
		Bar->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
		[
			Button(LOCTEXT("Random", "Zufall"), [this]() { if (C()) { C()->Randomize(); } }, nullptr)
		];
	}
	Bar->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
	[
		Button(LOCTEXT("Undo", "Rückgängig"), [this]() { if (C()) { C()->Undo(); } }, [this]() { return C() && C()->CanUndo(); })
	];
	Bar->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
	[
		Button(LOCTEXT("Redo", "Wiederholen"), [this]() { if (C()) { C()->Redo(); } }, [this]() { return C() && C()->CanRedo(); })
	];
	Bar->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
	[
		Button(LOCTEXT("Reset", "Zurücksetzen"), [this]() { if (C()) { C()->ResetToOriginal(); } }, nullptr)
	];
	if (!bCreation)
	{
		Bar->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
		[
			Button(LOCTEXT("Cancel", "Abbrechen"), [this]() { if (C()) { C()->Cancel(); } }, nullptr)
		];
	}
	Bar->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
	[
		Button(bCreation ? LOCTEXT("Done", "Fertig") : LOCTEXT("Apply", "Übernehmen"),
			[this]() { if (C()) { C()->Confirm(); } },
			[this]() { return C() && !C()->IsWaitingForServer(); }, true)
	];
	return Bar;
}

void SEwigfelsCreatorWidget::RebuildPanel()
{
	PanelBox->ClearChildren();
	ColorRowCounter = 0;
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr)
	{
		return;
	}
	switch (Cr->GetTab())
	{
	case EEwigfelsCreatorTab::Identity: BuildIdentity(); break;
	case EEwigfelsCreatorTab::Body: BuildBody(); break;
	case EEwigfelsCreatorTab::Face: BuildFace(); break;
	case EEwigfelsCreatorTab::SkinEyes: BuildSkinEyes(); break;
	case EEwigfelsCreatorTab::Hair: BuildHair(); break;
	case EEwigfelsCreatorTab::Marks: BuildMarks(); break;
	case EEwigfelsCreatorTab::Voice: BuildVoice(); break;
	case EEwigfelsCreatorTab::Outfit: BuildOutfit(); break;
	case EEwigfelsCreatorTab::Ring: BuildRing(); break;
	}
}

// ---------------------------------------------------------------------------
// Reiter

void SEwigfelsCreatorWidget::BuildIdentity()
{
	const int32 MaxLen = C()->GetCatalog().NameMax;
	AddHeader(LOCTEXT("NameHeader", "Name"));
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 4.f, 0.f, 10.f)
	[
		SNew(SEditableTextBox)
		.Font(CrFont(16))
		.HintText(LOCTEXT("NameHint", "Wie heißt du?"))
		.Text_Lambda([this]() { return C() ? FText::FromString(C()->GetWorking().Name) : FText::GetEmpty(); })
		.OnTextChanged_Lambda([this, MaxLen](const FText& NewText)
		{
			// Beim Tippen nichts kürzen/trimmen (sonst springt der Cursor), nur Länge begrenzen. Bereinigt wird beim Bestätigen.
			const FString Raw = NewText.ToString().Left(MaxLen);
			if (C() && Raw != C()->GetWorking().Name)
			{
				C()->EditLive([&Raw](FEwigfelsAppearance& L) { L.Name = Raw; });
			}
		})
		.OnTextCommitted_Lambda([this, MaxLen](const FText& NewText, ETextCommit::Type)
		{
			const FString Clean = EwigfelsLook::CleanName(NewText.ToString(), MaxLen);
			if (C())
			{
				C()->Edit([&Clean](FEwigfelsAppearance& L) { L.Name = Clean; });
			}
		})
	];
	AddChoices(LOCTEXT("Pronouns", "Wie sollen andere von dir sprechen?"),
		{ LOCTEXT("PronNeutral", "Nur mit Namen"), LOCTEXT("PronSie", "sie / ihr"), LOCTEXT("PronEr", "er / ihm") },
		[](const FEwigfelsAppearance& L) { return static_cast<int32>(L.Pronouns); },
		[](FEwigfelsAppearance& L, int32 V) { L.Pronouns = static_cast<EEwigfelsPronouns>(V); });
	AddInfo(LOCTEXT("PronInfo", "Die Anrede hängt nicht an Körper oder Stimme. Alles lässt sich frei kombinieren."));
	if (C()->IsCreation())
	{
		AddInfo(LOCTEXT("CreateInfo", "Keine Sorge: Kleidung, Haare und Schmuck änderst du später in den Läden von Velmark. Das Gesicht nur bei der Spiegelhexe im Raunwald."));
	}
}

void SEwigfelsCreatorWidget::BuildBody()
{
	const FEwigfelsLookCatalog& Cat = C()->GetCatalog();
	AddHeader(LOCTEXT("BodyHeader", "Statur"));
	AddPicker(LOCTEXT("BodyType", "Körperbau"), EwigfelsLook::Cat::Body, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.BodyType; }, [](FEwigfelsAppearance& L, FName V) { L.BodyType = V; });
	AddSlider(LOCTEXT("Height", "Größe"), Cat.HeightMin, Cat.HeightMax,
		[](const FEwigfelsAppearance& L) { return L.Height; }, [](FEwigfelsAppearance& L, float V) { L.Height = V; },
		[](float V) { return FText::Format(LOCTEXT("Cm", "{0} cm"), FMath::RoundToInt(172.f * V)); });
	for (const FEwigfelsSliderDef& Def : Cat.Sliders)
	{
		if (Def.Group == TEXT("koerper"))
		{
			const FName Id = Def.Id;
			AddSlider(Def.DisplayName, -1.f, 1.f, [Id](const FEwigfelsAppearance& L) { return L.GetSlider(Id); },
				[Id](FEwigfelsAppearance& L, float V) { L.SetSlider(Id, V); });
		}
	}
}

void SEwigfelsCreatorWidget::BuildFace()
{
	const FEwigfelsLookCatalog& Cat = C()->GetCatalog();
	AddHeader(LOCTEXT("FaceHeader", "Grundgesicht"));
	AddInfo(LOCTEXT("FaceInfo", "Wähle zwei Gesichter und misch sie. Danach formst du jedes Detail mit den Reglern."));
	AddPicker(LOCTEXT("FaceA", "Gesicht 1"), EwigfelsLook::Cat::Face, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.FaceA; }, [](FEwigfelsAppearance& L, FName V) { L.FaceA = V; });
	AddPicker(LOCTEXT("FaceB", "Gesicht 2"), EwigfelsLook::Cat::Face, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.FaceB; }, [](FEwigfelsAppearance& L, FName V) { L.FaceB = V; });
	AddSlider(LOCTEXT("FaceBlend", "Mischung 1 ↔ 2"), 0.f, 1.f,
		[](const FEwigfelsAppearance& L) { return L.FaceBlend; }, [](FEwigfelsAppearance& L, float V) { L.FaceBlend = V; },
		[](float V) { return FText::Format(LOCTEXT("Percent", "{0} %"), FMath::RoundToInt(V * 100.f)); });

	FName LastGroup;
	for (const FEwigfelsSliderDef& Def : Cat.Sliders)
	{
		if (Def.Group == TEXT("koerper"))
		{
			continue;
		}
		if (Def.Group != LastGroup)
		{
			AddHeader(Def.GroupName);
			LastGroup = Def.Group;
		}
		const FName Id = Def.Id;
		AddSlider(Def.DisplayName, -1.f, 1.f, [Id](const FEwigfelsAppearance& L) { return L.GetSlider(Id); },
			[Id](FEwigfelsAppearance& L, float V) { L.SetSlider(Id, V); });
	}
}

void SEwigfelsCreatorWidget::BuildSkinEyes()
{
	using namespace EwigfelsLook;
	if (C()->IsAreaEditable(Change::Skin))
	{
		AddHeader(LOCTEXT("SkinHeader", "Haut"));
		AddColor(LOCTEXT("SkinColor", "Hautton"), TEXT("skin"),
			[](const FEwigfelsAppearance& L) { return L.SkinColor; }, [](FEwigfelsAppearance& L, FColor V) { L.SkinColor = V; });
	}
	if (C()->IsAreaEditable(Change::Eyes))
	{
		AddHeader(LOCTEXT("EyesHeader", "Augen"));
		AddColor(LOCTEXT("EyeColor", "Augenfarbe"), TEXT("eyes"),
			[](const FEwigfelsAppearance& L) { return L.EyeColorL; },
			[](FEwigfelsAppearance& L, FColor V)
			{
				const bool bSame = L.EyeColorL == L.EyeColorR;
				L.EyeColorL = V;
				if (bSame)
				{
					L.EyeColorR = V;
				}
			});
		AddCheck(LOCTEXT("Hetero", "Zwei verschiedene Augenfarben"),
			[](const FEwigfelsAppearance& L) { return L.EyeColorL != L.EyeColorR; },
			[](FEwigfelsAppearance& L, bool bOn)
			{
				L.EyeColorR = bOn ? FColor(76, 122, 140) : L.EyeColorL;
				if (bOn && L.EyeColorR == L.EyeColorL)
				{
					L.EyeColorR = FColor(201, 162, 39);
				}
			});
		AddColor(LOCTEXT("EyeColorR", "Rechtes Auge"), TEXT("eyes"),
			[](const FEwigfelsAppearance& L) { return L.EyeColorR; }, [](FEwigfelsAppearance& L, FColor V) { L.EyeColorR = V; });
	}
}

void SEwigfelsCreatorWidget::BuildHair()
{
	using namespace EwigfelsLook;
	if (C()->IsAreaEditable(Change::Hair))
	{
		AddHeader(LOCTEXT("HairHeader", "Haare"));
		AddPicker(LOCTEXT("HairStyle", "Frisur"), Cat::Hair, NAME_None,
			[](const FEwigfelsAppearance& L) { return L.HairStyle; }, [](FEwigfelsAppearance& L, FName V) { L.HairStyle = V; });
		AddColor(LOCTEXT("HairColor", "Haarfarbe"), TEXT("hair"),
			[](const FEwigfelsAppearance& L) { return L.HairColor; },
			[](FEwigfelsAppearance& L, FColor V)
			{
				const bool bSame = L.HairColor == L.HairTipColor;
				L.HairColor = V;
				if (bSame)
				{
					L.HairTipColor = V;
				}
			});
		AddCheck(LOCTEXT("HairGradient", "Farbverlauf zu den Spitzen"),
			[](const FEwigfelsAppearance& L) { return L.HairColor != L.HairTipColor; },
			[](FEwigfelsAppearance& L, bool bOn) { L.HairTipColor = bOn ? FColor(232, 207, 156) : L.HairColor; });
		AddColor(LOCTEXT("HairTip", "Spitzen"), TEXT("hair"),
			[](const FEwigfelsAppearance& L) { return L.HairTipColor; }, [](FEwigfelsAppearance& L, FColor V) { L.HairTipColor = V; });
	}
	if (C()->IsAreaEditable(Change::Brows))
	{
		AddPicker(LOCTEXT("Brows", "Augenbrauen"), Cat::Brows, NAME_None,
			[](const FEwigfelsAppearance& L) { return L.Brows; }, [](FEwigfelsAppearance& L, FName V) { L.Brows = V; });
	}
	if (C()->IsAreaEditable(Change::Beard))
	{
		AddPicker(LOCTEXT("Beard", "Bart"), Cat::Beard, NAME_None,
			[](const FEwigfelsAppearance& L) { return L.Beard; }, [](FEwigfelsAppearance& L, FName V) { L.Beard = V; });
	}
}

void SEwigfelsCreatorWidget::BuildMarks()
{
	const FEwigfelsLookCatalog& Cat = C()->GetCatalog();
	AddInfo(FText::Format(LOCTEXT("MarksInfo", "Bis zu {0} Details gleichzeitig. Jedes mit eigener Farbe und Stärke."), Cat.MaxMarks));
	FName LastKind;
	for (const FEwigfelsLookOption& Option : Cat.GetOptions(EwigfelsLook::Cat::Marks))
	{
		const FName Id = Option.Id;
		const bool bHas = C()->GetWorking().FindMark(Id) != nullptr;
		if (!bHas && !C()->IsOptionAvailable(Option))
		{
			continue;
		}
		if (Option.Kind != LastKind)
		{
			AddHeader(CrKindName(Option.Kind));
			LastKind = Option.Kind;
		}
		const int32 MaxMarks = Cat.MaxMarks;
		const FName Kind = Option.Kind;
		PanelBox->AddSlot().AutoHeight().Padding(0.f, 3.f)
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([this, Id]() { return C() && C()->GetWorking().FindMark(Id) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
			.OnCheckStateChanged_Lambda([this, Id, MaxMarks, Kind](ECheckBoxState State)
			{
				if (!C())
				{
					return;
				}
				C()->Edit([Id, State, MaxMarks, Kind](FEwigfelsAppearance& L)
				{
					L.Marks.RemoveAll([Id](const FEwigfelsMarkLook& M) { return M.Id == Id; });
					if (State == ECheckBoxState::Checked && L.Marks.Num() < MaxMarks)
					{
						FEwigfelsMarkLook Mark;
						Mark.Id = Id;
						Mark.Color = Kind == TEXT("makeup") ? FColor(168, 58, 74) : Kind == TEXT("tattoo") ? FColor(26, 26, 40) : FColor(26, 20, 20);
						Mark.Opacity = 0.8f;
						L.Marks.Add(Mark);
					}
				});
				bRebuildPending = true;
			})
			[
				SNew(STextBlock).Text(OptionLabel(&Option)).Font(CrFont(13)).ColorAndOpacity(CrTextColor)
			]
		];
		if (bHas)
		{
			const FName Palette = Option.Kind == TEXT("makeup") ? FName(TEXT("makeup")) : FName(TEXT("dye"));
			AddColor(LOCTEXT("MarkColor", "Farbe"), Palette,
				[Id](const FEwigfelsAppearance& L) { const FEwigfelsMarkLook* M = L.FindMark(Id); return M ? M->Color : FColor::Black; },
				[Id](FEwigfelsAppearance& L, FColor V)
				{
					if (FEwigfelsMarkLook* M = L.Marks.FindByPredicate([Id](const FEwigfelsMarkLook& X) { return X.Id == Id; }))
					{
						M->Color = V;
					}
				});
			AddSlider(LOCTEXT("MarkOpacity", "Stärke"), 0.f, 1.f,
				[Id](const FEwigfelsAppearance& L) { const FEwigfelsMarkLook* M = L.FindMark(Id); return M ? M->Opacity : 0.f; },
				[Id](FEwigfelsAppearance& L, float V)
				{
					if (FEwigfelsMarkLook* M = L.Marks.FindByPredicate([Id](const FEwigfelsMarkLook& X) { return X.Id == Id; }))
					{
						M->Opacity = V;
					}
				});
		}
	}
}

void SEwigfelsCreatorWidget::BuildVoice()
{
	AddHeader(LOCTEXT("VoiceHeader", "Stimme"));
	AddInfo(LOCTEXT("VoiceInfo", "Hörst du beim Zaubern, Springen und bei Treffern. In Gesprächen wählst du Antworten als Text."));
	AddPicker(LOCTEXT("Voice", "Stimmlage"), EwigfelsLook::Cat::Voice, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.Voice; }, [](FEwigfelsAppearance& L, FName V) { L.Voice = V; });
	AddSlider(LOCTEXT("Pitch", "Tonhöhe"), -1.f, 1.f,
		[](const FEwigfelsAppearance& L) { return L.VoicePitch; }, [](FEwigfelsAppearance& L, float V) { L.VoicePitch = V; });
	AddButton(LOCTEXT("Listen", "Probehören"), [this]() { if (C()) { C()->PlayVoiceSample(); } });
}

void SEwigfelsCreatorWidget::BuildOutfit()
{
	const FEwigfelsLookCatalog& Cat = C()->GetCatalog();
	// Slots, die hier änderbar sind
	TArray<FName> Slots;
	for (const FName Slot : Cat.OutfitSlots)
	{
		if (C()->IsAreaEditable(CrOutfitArea(Slot)) || C()->IsAreaEditable(CrOutfitColorArea(Slot)))
		{
			Slots.Add(Slot);
		}
	}
	if (Slots.Num() == 0)
	{
		return;
	}
	if (!Slots.Contains(OutfitSlot))
	{
		OutfitSlot = Slots[0];
	}

	AddHeader(LOCTEXT("SlotHeader", "Was möchtest du ändern?"));
	TSharedRef<SWrapBox> SlotWrap = SNew(SWrapBox).UseAllottedSize(true);
	for (const FName Slot : Slots)
	{
		SlotWrap->AddSlot().Padding(2.f)
		[
			SNew(SButton)
			.ButtonColorAndOpacity_Lambda([this, Slot]() { return OutfitSlot == Slot ? CrGold : FLinearColor(0.25f, 0.27f, 0.36f, 0.95f); })
			.OnClicked_Lambda([this, Slot]() { OutfitSlot = Slot; bRebuildPending = true; return FReply::Handled(); })
			[
				SNew(STextBlock).Text(CrSlotName(Slot)).Font(CrFont(12)).ColorAndOpacity(FLinearColor::White)
			]
		];
	}
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 4.f, 0.f, 8.f)[SlotWrap];

	const FName Slot = OutfitSlot;
	auto Piece = [Slot](const FEwigfelsAppearance& L) -> const FEwigfelsOutfitPiece*
	{
		return L.FindPiece(Slot);
	};
	AddHeader(CrSlotName(Slot));
	if (C()->IsAreaEditable(CrOutfitArea(Slot)))
	{
		AddPicker(LOCTEXT("Item", "Stück"), EwigfelsLook::Cat::Outfit, Slot,
			[Piece](const FEwigfelsAppearance& L) { const FEwigfelsOutfitPiece* P = Piece(L); return P ? P->Item : NAME_None; },
			[Slot](FEwigfelsAppearance& L, FName V) { L.FindOrAddPiece(Slot).Item = V; });
	}
	if (C()->IsAreaEditable(CrOutfitColorArea(Slot)))
	{
		const FText Zones[3] = { LOCTEXT("Dye1", "Hauptfarbe"), LOCTEXT("Dye2", "Zweitfarbe"), LOCTEXT("Dye3", "Akzent (Saum, Knöpfe)") };
		for (int32 Zone = 0; Zone < 3; ++Zone)
		{
			AddColor(Zones[Zone], TEXT("dye"),
				[Piece, Zone](const FEwigfelsAppearance& L)
				{
					const FEwigfelsOutfitPiece* P = Piece(L);
					return !P ? FColor::Black : Zone == 0 ? P->Color1 : Zone == 1 ? P->Color2 : P->Color3;
				},
				[Slot, Zone](FEwigfelsAppearance& L, FColor V)
				{
					FEwigfelsOutfitPiece& P = L.FindOrAddPiece(Slot);
					(Zone == 0 ? P.Color1 : Zone == 1 ? P.Color2 : P.Color3) = V;
				});
		}
		AddPicker(LOCTEXT("Pattern", "Muster"), EwigfelsLook::Cat::Pattern, NAME_None,
			[Piece](const FEwigfelsAppearance& L) { const FEwigfelsOutfitPiece* P = Piece(L); return P ? P->Pattern : NAME_None; },
			[Slot](FEwigfelsAppearance& L, FName V) { L.FindOrAddPiece(Slot).Pattern = V; });
		AddPicker(LOCTEXT("Fabric", "Stoff"), EwigfelsLook::Cat::Fabric, NAME_None,
			[Piece](const FEwigfelsAppearance& L) { const FEwigfelsOutfitPiece* P = Piece(L); return P ? P->Fabric : NAME_None; },
			[Slot](FEwigfelsAppearance& L, FName V) { L.FindOrAddPiece(Slot).Fabric = V; });
		AddButton(LOCTEXT("DyeAll", "Diese Farben für alle Stücke"), [this, Slot]()
		{
			if (!C())
			{
				return;
			}
			C()->Edit([Slot](FEwigfelsAppearance& L)
			{
				const FEwigfelsOutfitPiece Source = L.FindOrAddPiece(Slot);
				for (FEwigfelsOutfitPiece& P : L.Outfit)
				{
					P.Color1 = Source.Color1;
					P.Color2 = Source.Color2;
					P.Color3 = Source.Color3;
				}
			});
		});
	}
}

void SEwigfelsCreatorWidget::BuildRing()
{
	AddHeader(LOCTEXT("RingHeader", "Dein Stimmring"));
	AddInfo(C()->IsCreation()
		? LOCTEXT("RingInfoStart", "Ein Erbstück von zu Hause. Die Farbe seines Quarzes ist die Farbe deiner Magie – jeder Funke aus deinen Fingern leuchtet so.")
		: LOCTEXT("RingInfoShop", "Die Quarzfarbe ist die Farbe deiner Magie."));
	AddPicker(LOCTEXT("Metal", "Metall"), EwigfelsLook::Cat::Metal, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.Rings.Num() ? L.Rings[0].Metal : NAME_None; },
		[](FEwigfelsAppearance& L, FName V) { if (L.Rings.Num() == 0) { L.Rings.AddDefaulted(); } L.Rings[0].Metal = V; });
	AddColor(LOCTEXT("Quartz", "Quarzfarbe"), TEXT("quartz"),
		[](const FEwigfelsAppearance& L) { return L.Rings.Num() ? L.Rings[0].Quartz : FColor::CrWhite; },
		[](FEwigfelsAppearance& L, FColor V) { if (L.Rings.Num() == 0) { L.Rings.AddDefaulted(); } L.Rings[0].Quartz = V; });
}

// ---------------------------------------------------------------------------
// Bausteine

void SEwigfelsCreatorWidget::AddHeader(const FText& Text)
{
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 12.f, 0.f, 4.f)
	[
		SNew(STextBlock).Text(Text).Font(CrFont(15, true)).ColorAndOpacity(CrGold)
	];
}

void SEwigfelsCreatorWidget::AddInfo(const FText& Text)
{
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 2.f, 0.f, 6.f)
	[
		SNew(STextBlock).Text(Text).Font(CrFont(11)).ColorAndOpacity(CrMuted).AutoWrapText(true)
	];
}

TArray<const FEwigfelsLookOption*> SEwigfelsCreatorWidget::GetChoices(FName Category, FName SlotFilter, FName Current) const
{
	TArray<const FEwigfelsLookOption*> Result;
	if (!C())
	{
		return Result;
	}
	for (const FEwigfelsLookOption& Option : C()->GetCatalog().GetOptions(Category))
	{
		if ((!SlotFilter.IsNone() && Option.Slot != SlotFilter))
		{
			continue;
		}
		if (C()->IsOptionAvailable(Option) || Option.Id == Current)
		{
			Result.Add(&Option);
		}
	}
	return Result;
}

FText SEwigfelsCreatorWidget::OptionLabel(const FEwigfelsLookOption* Option) const
{
	if (!Option)
	{
		return LOCTEXT("None", "–");
	}
	if (C() && !C()->IsCreation() && Option->Price > 0)
	{
		return FText::Format(LOCTEXT("WithPrice", "{0}  ({1} Kr.)"), Option->DisplayName, Option->Price);
	}
	return Option->DisplayName;
}

void SEwigfelsCreatorWidget::AddPicker(const FText& Label, FName Category, FName SlotFilter, FGetName Get, FSetName Set)
{
	auto Step = [this, Category, SlotFilter, Get, Set](int32 Dir)
	{
		UEwigfelsCreatorSubsystem* Cr = C();
		if (!Cr)
		{
			return;
		}
		const FName Current = Get(Cr->GetWorking());
		const TArray<const FEwigfelsLookOption*> Choices = GetChoices(Category, SlotFilter, Current);
		if (Choices.Num() == 0)
		{
			return;
		}
		const int32 Index = Choices.IndexOfByPredicate([Current](const FEwigfelsLookOption* O) { return O->Id == Current; });
		const int32 Next = (FMath::Max(Index, 0) + Dir + Choices.Num()) % Choices.Num();
		const FName NewId = Choices[Next]->Id;
		Cr->Edit([&Set, NewId](FEwigfelsAppearance& L) { Set(L, NewId); });
	};

	auto Arrow = [Step](const FText& Glyph, int32 Dir) -> TSharedRef<SWidget>
	{
		return SNew(SButton)
			.ButtonColorAndOpacity(FLinearColor(0.25f, 0.27f, 0.36f, 0.95f))
			.OnClicked_Lambda([Step, Dir]() { Step(Dir); return FReply::Handled(); })
			[
				SNew(STextBlock).Text(Glyph).Font(CrFont(14, true)).ColorAndOpacity(FLinearColor::White)
			];
	};

	PanelBox->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(STextBlock).Text(Label).Font(CrFont(12)).ColorAndOpacity(CrMuted)
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth()[Arrow(LOCTEXT("Prev", "◀"), -1)]
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center).HAlign(HAlign_Center)
			[
				SNew(STextBlock).Font(CrFont(14, true)).ColorAndOpacity(CrTextColor)
				.Text_Lambda([this, Category, SlotFilter, Get]()
				{
					if (!C())
					{
						return FText::GetEmpty();
					}
					const FName Current = Get(C()->GetWorking());
					const TArray<const FEwigfelsLookOption*> Choices = GetChoices(Category, SlotFilter, Current);
					const int32 Index = Choices.IndexOfByPredicate([Current](const FEwigfelsLookOption* O) { return O->Id == Current; });
					const FText Name = OptionLabel(C()->GetCatalog().Find(Category, Current));
					return FText::Format(LOCTEXT("PickerFmt", "{0}   {1}/{2}"), Name, Index + 1, Choices.Num());
				})
			]
			+ SHorizontalBox::Slot().AutoWidth()[Arrow(LOCTEXT("Next", "▶"), 1)]
		]
	];
}

void SEwigfelsCreatorWidget::AddSlider(const FText& Label, float Min, float Max, FGetFloat Get, FSetFloat Set, TFunction<FText(float)> Format)
{
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 3.f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().FillWidth(0.42f).VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(Label).Font(CrFont(12)).ColorAndOpacity(CrTextColor)
		]
		+ SHorizontalBox::Slot().FillWidth(0.43f).VAlign(VAlign_Center)
		[
			SNew(SSlider)
			.MinValue(Min)
			.MaxValue(Max)
			.SliderBarColor(FLinearColor(0.35f, 0.36f, 0.45f))
			.SliderHandleColor(CrGold)
			.Value_Lambda([this, Get]() { return C() ? Get(C()->GetWorking()) : 0.f; })
			.OnMouseCaptureBegin_Lambda([this]() { if (C()) { C()->BeginDrag(); } })
			.OnValueChanged_Lambda([this, Set](float V)
			{
				if (C())
				{
					C()->EditLive([&Set, V](FEwigfelsAppearance& L) { Set(L, V); });
				}
			})
		]
		+ SHorizontalBox::Slot().FillWidth(0.15f).VAlign(VAlign_Center).HAlign(HAlign_Right)
		[
			SNew(STextBlock).Font(CrFont(11)).ColorAndOpacity(CrMuted)
			.Text_Lambda([this, Get, Format]()
			{
				const float V = C() ? Get(C()->GetWorking()) : 0.f;
				return Format ? Format(V) : FText::AsNumber(FMath::RoundToInt(V * 100.f));
			})
		]
	];
}

void SEwigfelsCreatorWidget::AddColor(const FText& Label, FName Palette, FGetColor Get, FSetColor Set)
{
	const TArray<FColor>* Colors = C() ? C()->GetCatalog().Palettes.Find(Palette) : nullptr;
	// Schlüssel für „Feinabstimmung offen“: Reiter + Kleidungs-Slot + Nummer der Farbzeile (bleibt beim Aufklappen gleich)
	const FString Key = FString::Printf(TEXT("%d_%s_%d"), C() ? static_cast<int32>(C()->GetTab()) : 0, *OutfitSlot.ToString(), ColorRowCounter++);
	const bool bExpanded = ExpandedColors.Contains(Key);

	TSharedRef<SWrapBox> Swatches = SNew(SWrapBox).UseAllottedSize(true);
	if (Colors)
	{
		for (const FColor& Color : *Colors)
		{
			Swatches->AddSlot().Padding(2.f)
			[
				SNew(SButton)
				.ContentPadding(FMargin(1.f))
				.ButtonColorAndOpacity_Lambda([this, Get, Color]()
				{
					return C() && Get(C()->GetWorking()) == Color ? FLinearColor::White : FLinearColor(0.1f, 0.1f, 0.14f);
				})
				.OnClicked_Lambda([this, Set, Color]()
				{
					if (C())
					{
						C()->Edit([&Set, Color](FEwigfelsAppearance& L) { Set(L, Color); });
					}
					return FReply::Handled();
				})
				[
					SNew(SBox).WidthOverride(22.f).HeightOverride(22.f)
					[
						SNew(SColorBlock).Color(FLinearColor::FromSRGBColor(Color))
					]
				]
			];
		}
	}

	PanelBox->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 8.f, 0.f)
			[
				SNew(SBox).WidthOverride(28.f).HeightOverride(18.f)
				[
					SNew(SColorBlock).Color_Lambda([this, Get]() { return C() ? FLinearColor::FromSRGBColor(Get(C()->GetWorking())) : FLinearColor::Black; })
				]
			]
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center)
			[
				SNew(STextBlock).Text(Label).Font(CrFont(12)).ColorAndOpacity(CrTextColor)
			]
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor(0.25f, 0.27f, 0.36f, 0.95f))
				.OnClicked_Lambda([this, Key]()
				{
					if (ExpandedColors.Contains(Key)) { ExpandedColors.Remove(Key); } else { ExpandedColors.Add(Key); }
					bRebuildPending = true;
					return FReply::Handled();
				})
				[
					SNew(STextBlock).Text(bExpanded ? LOCTEXT("Less", "Weniger") : LOCTEXT("Fine", "Feinabstimmung"))
					.Font(CrFont(10)).ColorAndOpacity(FLinearColor::White)
				]
			]
		]
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 3.f)[Swatches]
	];

	if (bExpanded)
	{
		// Freie Farbe über Farbton / Sättigung / Helligkeit
		auto ToHsv = [](const FColor& Col) { return FLinearColor::FromSRGBColor(Col).LinearRGBToHSV(); };
		auto FromHsv = [](const FLinearColor& Hsv) { return Hsv.HSVToLinearRGB().ToFColor(true); };
		const FText Names[3] = { LOCTEXT("Hue", "Farbton"), LOCTEXT("Sat", "Sättigung"), LOCTEXT("Val", "Helligkeit") };
		const float Max[3] = { 359.f, 1.f, 1.f };
		for (int32 Channel = 0; Channel < 3; ++Channel)
		{
			AddSlider(Names[Channel], 0.f, Max[Channel],
				[Get, ToHsv, Channel](const FEwigfelsAppearance& L) { return ToHsv(Get(L)).Component(Channel); },
				[Get, Set, ToHsv, FromHsv, Channel](FEwigfelsAppearance& L, float V)
				{
					FLinearColor Hsv = ToHsv(Get(L));
					Hsv.Component(Channel) = V;
					Set(L, FromHsv(Hsv));
				},
				[Channel](float V) { return Channel == 0 ? FText::AsNumber(FMath::RoundToInt(V)) : FText::AsNumber(FMath::RoundToInt(V * 100.f)); });
		}
	}
}

void SEwigfelsCreatorWidget::AddChoices(const FText& Label, const TArray<FText>& Choices, TFunction<int32(const FEwigfelsAppearance&)> Get,
	TFunction<void(FEwigfelsAppearance&, int32)> Set)
{
	TSharedRef<SWrapBox> Wrap = SNew(SWrapBox).UseAllottedSize(true);
	for (int32 i = 0; i < Choices.Num(); ++i)
	{
		Wrap->AddSlot().Padding(2.f)
		[
			SNew(SButton)
			.ButtonColorAndOpacity_Lambda([this, Get, i]() { return C() && Get(C()->GetWorking()) == i ? CrGold : FLinearColor(0.25f, 0.27f, 0.36f, 0.95f); })
			.OnClicked_Lambda([this, Set, i]()
			{
				if (C())
				{
					C()->Edit([&Set, i](FEwigfelsAppearance& L) { Set(L, i); });
				}
				return FReply::Handled();
			})
			[
				SNew(STextBlock).Text(Choices[i]).Font(CrFont(13)).ColorAndOpacity(FLinearColor::White)
			]
		];
	}
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 6.f)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text(Label).Font(CrFont(12)).ColorAndOpacity(CrMuted)]
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 3.f)[Wrap]
	];
}

void SEwigfelsCreatorWidget::AddCheck(const FText& Label, TFunction<bool(const FEwigfelsAppearance&)> Get, TFunction<void(FEwigfelsAppearance&, bool)> Set)
{
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 4.f)
	[
		SNew(SCheckBox)
		.IsChecked_Lambda([this, Get]() { return C() && Get(C()->GetWorking()) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
		.OnCheckStateChanged_Lambda([this, Set](ECheckBoxState State)
		{
			if (C())
			{
				const bool bOn = State == ECheckBoxState::Checked;
				C()->Edit([&Set, bOn](FEwigfelsAppearance& L) { Set(L, bOn); });
			}
		})
		[
			SNew(STextBlock).Text(Label).Font(CrFont(12)).ColorAndOpacity(CrTextColor)
		]
	];
}

void SEwigfelsCreatorWidget::AddButton(const FText& Label, TFunction<void()> OnClick)
{
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 8.f).HAlign(HAlign_Left)
	[
		SNew(SButton)
		.ButtonColorAndOpacity(FLinearColor(0.25f, 0.27f, 0.36f, 0.95f))
		.OnClicked_Lambda([OnClick]() { OnClick(); return FReply::Handled(); })
		[
			SNew(STextBlock).Text(Label).Font(CrFont(12)).ColorAndOpacity(FLinearColor::White)
		]
	];
}

#undef LOCTEXT_NAMESPACE
