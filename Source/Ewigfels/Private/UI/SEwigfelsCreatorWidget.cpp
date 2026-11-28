// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "UI/SEwigfelsCreatorWidget.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Engine/Texture2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "EwigfelsCreator"

namespace
{
	// --- Farben: bunt, mit Nachthimmel als Grund ---
	const FLinearColor CrGold(1.f, 0.82f, 0.42f);
	const FLinearColor CrGlow(0.42f, 1.f, 0.92f);          // Fokus (Türkis)
	const FLinearColor CrHover(0.78f, 0.6f, 1.f);          // Maus darüber (Flieder)
	const FLinearColor CrTextColor(0.97f, 0.95f, 1.f);
	const FLinearColor CrMuted(0.74f, 0.72f, 0.9f);
	const FLinearColor CrWarn(1.f, 0.6f, 0.55f);
	const FLinearColor CrSkyTop(0.17f, 0.06f, 0.33f, 0.94f);   // tiefes Violett
	const FLinearColor CrSkyBottom(0.04f, 0.13f, 0.27f, 0.94f); // Nachtblau
	const FLinearColor CrCard(0.13f, 0.08f, 0.26f, 0.92f);

	FSlateFontInfo CrFont(int32 Size, bool bBold = false)
	{
		return FCoreStyle::GetDefaultFontStyle(bBold ? "Bold" : "Regular", Size);
	}

	const FSlateBrush* CrRound(float Radius)
	{
		static FSlateRoundedBoxBrush R6(FLinearColor::White, 6.f);
		static FSlateRoundedBoxBrush R12(FLinearColor::White, 12.f);
		static FSlateRoundedBoxBrush R18(FLinearColor::White, 18.f);
		static FSlateRoundedBoxBrush R40(FLinearColor::White, 40.f);
		return Radius <= 6.f ? &R6 : Radius <= 12.f ? &R12 : Radius <= 18.f ? &R18 : &R40;
	}

	const FButtonStyle* CrNoBorder()
	{
		return &FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder");
	}

	float CrPulse(float Speed = 3.f)
	{
		return 0.5f + 0.5f * FMath::Sin(static_cast<float>(FSlateApplication::Get().GetCurrentTime()) * Speed);
	}

	FLinearColor CrAlpha(const FLinearColor& Color, float Alpha) { return FLinearColor(Color.R, Color.G, Color.B, Alpha); }

	FLinearColor CrAccent(EEwigfelsCreatorTab Tab)
	{
		switch (Tab)
		{
		case EEwigfelsCreatorTab::Head: return FLinearColor(1.f, 0.5f, 0.74f);    // Rosa
		case EEwigfelsCreatorTab::Hair: return FLinearColor(1.f, 0.72f, 0.32f);   // Bernstein
		case EEwigfelsCreatorTab::Body: return FLinearColor(0.45f, 0.95f, 0.66f); // Minze
		case EEwigfelsCreatorTab::Marks: return FLinearColor(0.74f, 0.58f, 1.f);  // Flieder
		case EEwigfelsCreatorTab::Outfit: return FLinearColor(0.4f, 0.76f, 1.f);  // Himmelblau
		case EEwigfelsCreatorTab::Ring: return CrGold;
		case EEwigfelsCreatorTab::Voice: return FLinearColor(0.4f, 0.96f, 0.9f);  // Türkis
		case EEwigfelsCreatorTab::Name: return FLinearColor(1.f, 0.58f, 0.5f);    // Koralle
		case EEwigfelsCreatorTab::Count: break;
		}
		return CrGold;
	}

	FText CrTabName(EEwigfelsCreatorTab Tab)
	{
		switch (Tab)
		{
		case EEwigfelsCreatorTab::Head: return LOCTEXT("TabHead", "Kopf");
		case EEwigfelsCreatorTab::Hair: return LOCTEXT("TabHair", "Haare");
		case EEwigfelsCreatorTab::Body: return LOCTEXT("TabBody", "Statur");
		case EEwigfelsCreatorTab::Marks: return LOCTEXT("TabMarks", "Zeichen");
		case EEwigfelsCreatorTab::Outfit: return LOCTEXT("TabOutfit", "Kleidung");
		case EEwigfelsCreatorTab::Ring: return LOCTEXT("TabRing", "Ring");
		case EEwigfelsCreatorTab::Voice: return LOCTEXT("TabVoice", "Stimme");
		case EEwigfelsCreatorTab::Name: return LOCTEXT("TabName", "Name");
		case EEwigfelsCreatorTab::Count: break;
		}
		return FText::GetEmpty();
	}

	FText CrTabTitle(EEwigfelsCreatorTab Tab)
	{
		switch (Tab)
		{
		case EEwigfelsCreatorTab::Head: return LOCTEXT("TitleHead", "Dein Gesicht");
		case EEwigfelsCreatorTab::Hair: return LOCTEXT("TitleHair", "Deine Haare");
		case EEwigfelsCreatorTab::Body: return LOCTEXT("TitleBody", "Deine Statur");
		case EEwigfelsCreatorTab::Marks: return LOCTEXT("TitleMarks", "Kleine Zeichen");
		case EEwigfelsCreatorTab::Outfit: return LOCTEXT("TitleOutfit", "Kleiderschrank");
		case EEwigfelsCreatorTab::Ring: return LOCTEXT("TitleRing", "Dein Stimmring");
		case EEwigfelsCreatorTab::Voice: return LOCTEXT("TitleVoice", "Deine Stimme");
		case EEwigfelsCreatorTab::Name: return LOCTEXT("TitleName", "Wer bist du?");
		case EEwigfelsCreatorTab::Count: break;
		}
		return FText::GetEmpty();
	}

	FText CrTabInfo(EEwigfelsCreatorTab Tab)
	{
		switch (Tab)
		{
		case EEwigfelsCreatorTab::Head: return LOCTEXT("InfoHead", "Such dir einen Kopf aus. Hautton und Augenfarbe passt du darunter an.");
		case EEwigfelsCreatorTab::Hair: return LOCTEXT("InfoHair", "Frisur, Farbe, Brauen und Bart.");
		case EEwigfelsCreatorTab::Body: return LOCTEXT("InfoBody", "Wie bist du gebaut, und wie groß bist du?");
		case EEwigfelsCreatorTab::Marks: return LOCTEXT("InfoMarks", "Sommersprossen, Narben, Schminke. Antippen schaltet an und aus.");
		case EEwigfelsCreatorTab::Outfit: return LOCTEXT("InfoOutfit", "Erst das Teil aussuchen, dann die Farbe.");
		case EEwigfelsCreatorTab::Ring: return LOCTEXT("InfoRing", "Ein Erbstück von zu Hause. Sein Quarz gibt deiner Magie die Farbe: Jeder Funke aus deinen Fingern leuchtet so.");
		case EEwigfelsCreatorTab::Voice: return LOCTEXT("InfoVoice", "Du hörst sie beim Zaubern, Springen und bei Treffern. Antippen spielt sie vor.");
		case EEwigfelsCreatorTab::Name: return LOCTEXT("InfoName", "Dein Name und wie andere von dir sprechen.");
		case EEwigfelsCreatorTab::Count: break;
		}
		return FText::GetEmpty();
	}

	FText CrSlotName(FName Slot)
	{
		static const TMap<FName, FText> Names = {
			{ TEXT("kopf"), LOCTEXT("SlotKopf", "Hut") }, { TEXT("gesicht"), LOCTEXT("SlotGesicht", "Gesicht") },
			{ TEXT("hals"), LOCTEXT("SlotHals", "Hals") }, { TEXT("oberkoerper"), LOCTEXT("SlotOber", "Oberteil") },
			{ TEXT("umhang"), LOCTEXT("SlotUmhang", "Umhang") }, { TEXT("guertel"), LOCTEXT("SlotGuertel", "Gürtel") },
			{ TEXT("handschuhe"), LOCTEXT("SlotHand", "Handschuhe") }, { TEXT("beine"), LOCTEXT("SlotBeine", "Hose") },
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

	FColor CrMarkColor(FName Kind)
	{
		return Kind == TEXT("makeup") ? FColor(168, 58, 74) : Kind == TEXT("tattoo") ? FColor(26, 26, 40)
			: Kind == TEXT("paint") ? FColor(63, 110, 168) : Kind == TEXT("rune") ? FColor(110, 200, 255) : FColor(26, 20, 20);
	}

	FName CrOutfitArea(FName Slot) { return FName(*(TEXT("outfit:") + Slot.ToString())); }
	FName CrOutfitColorArea(FName Slot) { return FName(*(TEXT("outfit_colors:") + Slot.ToString())); }

	/** Hintergrund des Bedienfelds: Farbverlauf Violett → Nachtblau mit schwebenden, funkelnden Lichtpunkten. */
	class SEwigfelsMagicBackdrop : public SLeafWidget
	{
	public:
		SLATE_BEGIN_ARGS(SEwigfelsMagicBackdrop) {}
		SLATE_END_ARGS()

		void Construct(const FArguments&)
		{
			ForceVolatile(true);
		}

		virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D::ZeroVector; }

		virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& Geometry, const FSlateRect& CullingRect,
			FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& Style, bool bParentEnabled) const override
		{
			const FVector2D Size = Geometry.GetLocalSize();
			TArray<FSlateGradientStop> Stops;
			Stops.Add(FSlateGradientStop(FVector2D(0.f, 0.f), CrSkyTop));
			Stops.Add(FSlateGradientStop(FVector2D(Size.X * 0.5f, Size.Y * 0.55f), FLinearColor(0.09f, 0.09f, 0.32f, 0.94f)));
			Stops.Add(FSlateGradientStop(FVector2D(Size.X, Size.Y), CrSkyBottom));
			FSlateDrawElement::MakeGradient(OutDrawElements, LayerId, Geometry.ToPaintGeometry(), Stops, Orient_Horizontal);

			// Funken: feste Startpunkte, steigen langsam auf und funkeln
			const float Time = static_cast<float>(FSlateApplication::Get().GetCurrentTime());
			const FLinearColor Colors[] = { CrGold, CrGlow, FLinearColor(1.f, 0.55f, 0.85f), CrHover };
			for (int32 i = 0; i < 46; ++i)
			{
				const float Seed = static_cast<float>(i) * 12.9898f;
				const float Fx = FMath::Frac(FMath::Sin(Seed) * 43758.5453f);
				const float Fy = FMath::Frac(FMath::Sin(Seed * 1.7f) * 24634.6345f);
				const float Speed = 0.012f + 0.02f * FMath::Frac(Fx * 7.3f);
				const float Y = FMath::Frac(Fy - Time * Speed) * Size.Y;
				const float X = Fx * Size.X + FMath::Sin(Time * 0.6f + Seed) * 8.f;
				const float Twinkle = FMath::Square(0.5f + 0.5f * FMath::Sin(Time * (1.4f + Fy * 2.f) + Seed));
				const float Dot = 2.f + 3.f * FMath::Frac(Fy * 5.1f);
				FLinearColor Color = Colors[i % UE_ARRAY_COUNT(Colors)];
				Color.A = 0.15f + 0.75f * Twinkle;
				FSlateDrawElement::MakeBox(OutDrawElements, LayerId + 1,
					Geometry.ToPaintGeometry(FVector2D(Dot, Dot), FSlateLayoutTransform(FVector2D(X, Y))),
					CrRound(6.f), ESlateDrawEffect::None, Color);
			}
			return LayerId + 1;
		}
	};

	/** Fläche hinter dem Bedienfeld: Ziehen dreht die Kamera, Mausrad zoomt. */
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

	/** Kleines farbiges Tasten-Schild (A, B, LB …). */
	TSharedRef<SWidget> CrBadge(const FText& Key)
	{
		const FString K = Key.ToString();
		const FLinearColor Color = K == TEXT("A") ? FLinearColor(0.3f, 0.85f, 0.4f) : K == TEXT("B") ? FLinearColor(0.95f, 0.35f, 0.35f)
			: K == TEXT("X") ? FLinearColor(0.3f, 0.55f, 1.f) : K == TEXT("Y") ? FLinearColor(1.f, 0.8f, 0.25f) : FLinearColor(0.55f, 0.55f, 0.7f);
		return SNew(SBorder)
			.BorderImage(CrRound(6.f))
			.BorderBackgroundColor(Color)
			.Padding(FMargin(6.f, 1.f))
			[
				SNew(STextBlock).Text(Key).Font(CrFont(10, true)).ColorAndOpacity(FLinearColor(0.05f, 0.03f, 0.1f))
			];
	}
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
		// Links: Bedienfeld
		+ SOverlay::Slot().HAlign(HAlign_Left).VAlign(VAlign_Fill)
		[
			SNew(SBox).WidthOverride(640.f)
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SEwigfelsMagicBackdrop)
				]
				+ SOverlay::Slot().Padding(FMargin(30.f, 26.f, 26.f, 20.f))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot().AutoHeight()
					[
						SNew(STextBlock).Text(Title).Font(CrFont(28, true))
						.ColorAndOpacity_Lambda([]() { return FSlateColor(FMath::Lerp(CrGold, FLinearColor(1.f, 0.93f, 0.7f), CrPulse(1.5f))); })
						.ShadowOffset(FVector2D(0.f, 2.f)).ShadowColorAndOpacity(FLinearColor(0.85f, 0.3f, 0.75f, 0.7f))
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 14.f)
					[
						SNew(STextBlock).Text(Subtitle).Font(CrFont(12)).ColorAndOpacity(CrMuted).AutoWrapText(true)
					]
					+ SVerticalBox::Slot().AutoHeight()
					[
						MakeTabBar()
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 10.f, 0.f, 0.f)
					[
						MakeStepDots()
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 12.f, 0.f, 0.f)
					[
						SNew(STextBlock).Font(CrFont(22, true))
						.Text_Lambda([this]() { return C() ? CrTabTitle(C()->GetTab()) : FText::GetEmpty(); })
						.ColorAndOpacity_Lambda([this]() { return FSlateColor(C() ? CrAccent(C()->GetTab()) : CrGold); })
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.f, 0.f, 8.f)
					[
						SNew(STextBlock).Font(CrFont(12)).ColorAndOpacity(CrMuted).AutoWrapText(true)
						.Text_Lambda([this]() { return C() ? CrTabInfo(C()->GetTab()) : FText::GetEmpty(); })
					]
					+ SVerticalBox::Slot().FillHeight(1.f)
					[
						SNew(SScrollBox)
						.ScrollWhenFocusChanges(EScrollWhenFocusChanges::AnimatedScroll)
						+ SScrollBox::Slot().Padding(FMargin(0.f, 0.f, 10.f, 0.f))
						[
							SAssignNew(PanelBox, SVerticalBox)
						]
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 10.f, 0.f, 0.f)
					[
						MakeBottomBar()
					]
					+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 10.f, 0.f, 0.f)
					[
						MakeHints()
					]
				]
			]
		]
		// Rechts unten neben der Figur: wie man sie dreht
		+ SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom).Padding(30.f)
		[
			SNew(SBorder).BorderImage(CrRound(12.f)).BorderBackgroundColor(FLinearColor(0.08f, 0.04f, 0.18f, 0.7f)).Padding(FMargin(12.f, 6.f))
			[
				SNew(STextBlock).Font(CrFont(11)).ColorAndOpacity(CrMuted)
				.Text_Lambda([this]()
				{
					return bGamepad ? LOCTEXT("OrbitPad", "Rechter Stick: Figur drehen  ·  LT / RT: näher und weiter")
						: LOCTEXT("OrbitMouse", "Maus ziehen: Figur drehen  ·  Mausrad: näher und weiter");
				})
			]
		]
	];

	SeenTab = Cr ? Cr->GetTab() : EEwigfelsCreatorTab::Count;
	RebuildPanel();
}

TSharedRef<SWidget> SEwigfelsCreatorWidget::MakeTabBar()
{
	UEwigfelsCreatorSubsystem* Cr = C();
	TSharedRef<SWrapBox> Chips = SNew(SWrapBox).UseAllottedSize(true);
	const TArray<EEwigfelsCreatorTab> Tabs = Cr ? Cr->GetVisibleTabs() : TArray<EEwigfelsCreatorTab>();
	const bool bGuided = Cr && Cr->IsGuided();
	for (int32 i = 0; i < Tabs.Num(); ++i)
	{
		const EEwigfelsCreatorTab Tab = Tabs[i];
		const FLinearColor Accent = CrAccent(Tab);
		const FText Label = bGuided ? FText::Format(LOCTEXT("ChipNum", "{0}  {1}"), i + 1, CrTabName(Tab)) : CrTabName(Tab);
		Chips->AddSlot().Padding(3.f)
		[
			SNew(SButton)
			.ButtonStyle(CrNoBorder())
			.ContentPadding(0.f)
			.IsFocusable(false) // Bereiche wechseln mit LB/RB bzw. Klick, das Steuerkreuz bleibt in den Kacheln
			.OnClicked_Lambda([this, Tab]()
			{
				if (C())
				{
					C()->SetTab(Tab);
				}
				return FReply::Handled();
			})
			[
				SNew(SBorder)
				.BorderImage(CrRound(12.f))
				.Padding(FMargin(12.f, 5.f))
				.BorderBackgroundColor_Lambda([this, Tab, Accent]()
				{
					const bool bActive = C() && C()->GetTab() == Tab;
					return FSlateColor(bActive ? CrAlpha(Accent, 0.75f + 0.25f * CrPulse(2.f)) : CrAlpha(Accent, 0.18f));
				})
				[
					SNew(STextBlock).Text(Label).Font(CrFont(12, true))
					.ColorAndOpacity_Lambda([this, Tab]()
					{
						return FSlateColor(C() && C()->GetTab() == Tab ? FLinearColor(0.08f, 0.03f, 0.15f) : CrTextColor);
					})
				]
			]
		];
	}
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
		[
			SNew(SBox).Visibility_Lambda([this]() { return bGamepad ? EVisibility::Visible : EVisibility::Collapsed; })
			[
				CrBadge(LOCTEXT("PadLB", "LB"))
			]
		]
		+ SHorizontalBox::Slot().FillWidth(1.f)
		[
			Chips
		]
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(6.f, 0.f, 0.f, 0.f)
		[
			SNew(SBox).Visibility_Lambda([this]() { return bGamepad ? EVisibility::Visible : EVisibility::Collapsed; })
			[
				CrBadge(LOCTEXT("PadRB", "RB"))
			]
		];
}

TSharedRef<SWidget> SEwigfelsCreatorWidget::MakeStepDots()
{
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr || !Cr->IsGuided())
	{
		return SNullWidget::NullWidget;
	}
	const TArray<EEwigfelsCreatorTab> Tabs = Cr->GetVisibleTabs();
	TSharedRef<SHorizontalBox> Row = SNew(SHorizontalBox);
	Row->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 10.f, 0.f)
	[
		SNew(STextBlock).Font(CrFont(11, true)).ColorAndOpacity(CrMuted)
		.Text_Lambda([this, Tabs]()
		{
			const int32 Index = C() ? Tabs.IndexOfByKey(C()->GetTab()) : 0;
			return FText::Format(LOCTEXT("StepOf", "Schritt {0} von {1}"), Index + 1, Tabs.Num());
		})
	];
	for (int32 i = 0; i < Tabs.Num(); ++i)
	{
		const EEwigfelsCreatorTab Tab = Tabs[i];
		Row->AddSlot().AutoWidth().VAlign(VAlign_Center).Padding(3.f, 0.f)
		[
			SNew(SBox)
			.WidthOverride_Lambda([this, Tab]() { return FOptionalSize(C() && C()->GetTab() == Tab ? 22.f : 10.f); })
			.HeightOverride(10.f)
			[
				SNew(SBorder).BorderImage(CrRound(6.f))
				.BorderBackgroundColor_Lambda([this, Tabs, i, Tab]()
				{
					const int32 Current = C() ? Tabs.IndexOfByKey(C()->GetTab()) : 0;
					return FSlateColor(i == Current ? CrAccent(Tab) : i < Current ? CrAlpha(CrAccent(Tab), 0.6f) : FLinearColor(1.f, 1.f, 1.f, 0.15f));
				})
			]
		];
	}
	return Row;
}

TSharedRef<SWidget> SEwigfelsCreatorWidget::MakePill(const FText& Label, const FText& PadKey, FLinearColor Color, TFunction<void()> OnClick,
	TFunction<bool()> IsEnabled, TFunction<bool()> IsVisible)
{
	TSharedRef<TWeakPtr<SButton>> Self = MakeShared<TWeakPtr<SButton>>();
	TSharedPtr<SButton> Button;
	TSharedRef<SWidget> Result = SNew(SBox)
		.Visibility_Lambda([IsVisible]() { return !IsVisible || IsVisible() ? EVisibility::Visible : EVisibility::Collapsed; })
		[
			SAssignNew(Button, SButton)
			.ButtonStyle(CrNoBorder())
			.ContentPadding(0.f)
			.IsEnabled_Lambda([IsEnabled]() { return !IsEnabled || IsEnabled(); })
			.OnClicked_Lambda([OnClick]() { OnClick(); return FReply::Handled(); })
			[
				SNew(SBorder)
				.BorderImage(CrRound(18.f))
				.Padding(FMargin(16.f, 8.f))
				.BorderBackgroundColor_Lambda([Self, Color, IsEnabled]()
				{
					const TSharedPtr<SButton> B = Self->Pin();
					const bool bLit = B.IsValid() && (B->IsHovered() || B->HasAnyUserFocus().IsSet());
					const float Alpha = IsEnabled && !IsEnabled() ? 0.3f : bLit ? 1.f : 0.82f;
					return FSlateColor(CrAlpha(bLit ? FMath::Lerp(Color, FLinearColor::White, 0.25f) : Color, Alpha));
				})
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 6.f, 0.f)
					[
						SNew(SBox).Visibility_Lambda([this, PadKey]() { return bGamepad && !PadKey.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed; })
						[
							CrBadge(PadKey)
						]
					]
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
					[
						SNew(STextBlock).Text(Label).Font(CrFont(13, true)).ColorAndOpacity(FLinearColor(0.08f, 0.03f, 0.15f))
					]
				]
			]
		];
	*Self = Button;
	return Result;
}

TSharedRef<SWidget> SEwigfelsCreatorWidget::MakeBottomBar()
{
	UEwigfelsCreatorSubsystem* Cr = C();
	const bool bCreation = Cr && Cr->IsCreation();
	const bool bGuided = Cr && Cr->IsGuided();
	const FLinearColor PillSoft(0.62f, 0.55f, 0.95f);

	TSharedRef<SHorizontalBox> Buttons = SNew(SHorizontalBox);
	if (bCreation)
	{
		Buttons->AddSlot().AutoWidth().Padding(0.f, 0.f, 8.f, 0.f)
		[
			MakePill(LOCTEXT("Random", "Zufall"), LOCTEXT("PadY", "Y"), FLinearColor(1.f, 0.78f, 0.35f), [this]() { if (C()) { C()->Randomize(); } })
		];
	}
	Buttons->AddSlot().AutoWidth().Padding(0.f, 0.f, 8.f, 0.f)
	[
		MakePill(LOCTEXT("Undo", "Rückgängig"), LOCTEXT("PadX", "X"), PillSoft, [this]() { if (C()) { C()->Undo(); } },
			[this]() { return C() && C()->CanUndo(); })
	];
	Buttons->AddSlot().FillWidth(1.f)[SNullWidget::NullWidget];
	if (bGuided)
	{
		Buttons->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
		[
			MakePill(LOCTEXT("Back", "‹ Zurück"), LOCTEXT("PadB", "B"), PillSoft, [this]() { StepTab(-1); },
				[this]() { return C() && !C()->IsFirstTab(); })
		];
	}
	else
	{
		Buttons->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
		[
			MakePill(LOCTEXT("Cancel", "Abbrechen"), LOCTEXT("PadB", "B"), PillSoft, [this]() { if (C()) { C()->Cancel(); } })
		];
	}
	// Hauptknopf: Weiter (geführt, nicht letzter Schritt), sonst Fertig / Übernehmen
	Buttons->AddSlot().AutoWidth().Padding(8.f, 0.f, 0.f, 0.f)
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			MakePill(LOCTEXT("Next", "Weiter ›"), LOCTEXT("PadStart", "Start"), CrGlow, [this]() { PrimaryAction(); },
				[this]() { return C() && !C()->IsWaitingForServer(); },
				[this]() { return C() && C()->IsGuided() && !C()->IsLastTab(); })
		]
		+ SOverlay::Slot()
		[
			MakePill(bCreation ? LOCTEXT("Done", "Fertig!") : LOCTEXT("Apply", "Übernehmen"), LOCTEXT("PadStart", "Start"), CrGold,
				[this]() { PrimaryAction(); },
				[this]() { return C() && !C()->IsWaitingForServer(); },
				[this]() { return C() && (!C()->IsGuided() || C()->IsLastTab()); })
		]
	];

	return SNew(SVerticalBox)
		// Status: Fehler, Preis, Geld
		+ SVerticalBox::Slot().AutoHeight().Padding(2.f, 0.f, 2.f, 8.f)
		[
			SNew(STextBlock).Font(CrFont(13, true)).AutoWrapText(true)
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
					if (!Cr2->IsLastTab())
					{
						return FText::GetEmpty();
					}
					return bOk ? FText::Format(LOCTEXT("ReadyCreate", "Bereit, {0}? Dann „Fertig!“"), FText::FromString(Cr2->GetWorking().Name)) : Reason;
				}
				if (!bOk)
				{
					return Reason;
				}
				const FEwigfelsLookShop* Shop = Cr2->GetShop();
				const FText PriceText = Shop && !Shop->PriceItem.IsNone() && Shop->PriceItemCount > 0
					? FText::Format(LOCTEXT("PriceItem", "Preis: {0} × {1}"), Shop->PriceItemCount, FText::FromName(Shop->PriceItem))
					: FText::Format(LOCTEXT("PriceCrowns", "Preis: {0} Kronen"), Price);
				const int32 Money = Cr2->GetMoney();
				return Money >= 0 ? FText::Format(LOCTEXT("PriceMoney", "{0}  ·  Du hast {1}"), PriceText, Money) : PriceText;
			})
		]
		+ SVerticalBox::Slot().AutoHeight()
		[
			Buttons
		];
}

TSharedRef<SWidget> SEwigfelsCreatorWidget::MakeHints()
{
	auto Hint = [](const FText& Key, const FText& Label) -> TSharedRef<SWidget>
	{
		return SNew(SHorizontalBox)
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 4.f, 0.f)[CrBadge(Key)]
			+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 14.f, 0.f)
			[
				SNew(STextBlock).Text(Label).Font(CrFont(11)).ColorAndOpacity(CrMuted)
			];
	};
	TSharedRef<SWrapBox> Pad = SNew(SWrapBox).UseAllottedSize(true);
	Pad->AddSlot()[Hint(LOCTEXT("PadA", "A"), LOCTEXT("HintPick", "Wählen"))];
	Pad->AddSlot()[Hint(LOCTEXT("PadB", "B"), LOCTEXT("HintBack", "Zurück"))];
	Pad->AddSlot()[Hint(LOCTEXT("PadLBRB", "LB / RB"), LOCTEXT("HintTabs", "Bereich"))];

	TSharedRef<SWrapBox> Mouse = SNew(SWrapBox).UseAllottedSize(true);
	Mouse->AddSlot()[Hint(LOCTEXT("KeyClick", "Klick"), LOCTEXT("HintPick", "Wählen"))];
	Mouse->AddSlot()[Hint(LOCTEXT("KeyQE", "Q / E"), LOCTEXT("HintTabs", "Bereich"))];
	Mouse->AddSlot()[Hint(LOCTEXT("KeyUndo", "Strg+Z"), LOCTEXT("HintUndo", "Rückgängig"))];
	Mouse->AddSlot()[Hint(LOCTEXT("KeyEsc", "Esc"), LOCTEXT("HintBack", "Zurück"))];

	return SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SBox).Visibility_Lambda([this]() { return bGamepad ? EVisibility::Visible : EVisibility::Collapsed; })[Pad]
		]
		+ SOverlay::Slot()
		[
			SNew(SBox).Visibility_Lambda([this]() { return bGamepad ? EVisibility::Collapsed : EVisibility::Visible; })[Mouse]
		];
}

// ---------------------------------------------------------------------------
// Eingabe

void SEwigfelsCreatorWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr)
	{
		return;
	}
	if (Cr->GetTab() != SeenTab)
	{
		SeenTab = Cr->GetTab();
		bRebuildPending = true;
	}
	if (bRebuildPending)
	{
		bRebuildPending = false;
		RebuildPanel(); // nach dem Klick, nicht mitten im Klick des Knopfs, der gerade neu gebaut wird
	}
	if (bFocusPending)
	{
		bFocusPending = false;
		const TSharedPtr<SWidget> Target = FocusSelected.IsValid() ? FocusSelected : FocusFirst;
		if (Target.IsValid())
		{
			Cr->FocusWidget(Target.ToSharedRef());
		}
	}

	// Rechter Stick dreht die Figur, Schultertasten hinten zoomen
	const float Dead = 0.18f;
	const float Yaw = FMath::Abs(RightStick.X) > Dead ? RightStick.X : 0.f;
	const float Pitch = FMath::Abs(RightStick.Y) > Dead ? RightStick.Y : 0.f;
	if (Yaw != 0.f || Pitch != 0.f)
	{
		Cr->Orbit(Yaw * 140.f * InDeltaTime, Pitch * 70.f * InDeltaTime);
	}
	const float ZoomInput = TriggerZoom[1] - TriggerZoom[0];
	if (FMath::Abs(ZoomInput) > 0.05f)
	{
		Cr->Zoom(ZoomInput * 1.2f * InDeltaTime);
	}
}

FReply SEwigfelsCreatorWidget::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	// Das Fenster selbst bekommt beim Öffnen den Fokus → weiter an die gewählte Kachel
	const TSharedPtr<SWidget> Target = FocusSelected.IsValid() ? FocusSelected : FocusFirst;
	if (Target.IsValid())
	{
		return FReply::Handled().SetUserFocus(Target.ToSharedRef(), EFocusCause::Navigation);
	}
	return FReply::Handled();
}

FReply SEwigfelsCreatorWidget::OnPreviewKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	bGamepad = InKeyEvent.GetKey().IsGamepadKey();
	return FReply::Unhandled();
}

FReply SEwigfelsCreatorWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!MouseEvent.GetCursorDelta().IsNearlyZero(2.f))
	{
		bGamepad = false;
	}
	return FReply::Unhandled();
}

FReply SEwigfelsCreatorWidget::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bGamepad = false;
	return FReply::Unhandled();
}

FReply SEwigfelsCreatorWidget::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogInputEvent)
{
	const FKey Key = InAnalogInputEvent.GetKey();
	const float Value = InAnalogInputEvent.GetAnalogValue();
	if (Key == EKeys::Gamepad_RightX) { RightStick.X = Value; }
	else if (Key == EKeys::Gamepad_RightY) { RightStick.Y = Value; }
	else if (Key == EKeys::Gamepad_LeftTriggerAxis) { TriggerZoom[0] = Value; }
	else if (Key == EKeys::Gamepad_RightTriggerAxis) { TriggerZoom[1] = Value; }
	else
	{
		return FReply::Unhandled(); // linker Stick: Slate-Navigation durch die Kacheln
	}
	if (FMath::Abs(Value) > 0.3f)
	{
		bGamepad = true;
	}
	return FReply::Handled();
}

FReply SEwigfelsCreatorWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr)
	{
		return FReply::Unhandled();
	}
	const FKey Key = InKeyEvent.GetKey();
	// Beim Tippen des Namens keine Buchstaben-Kürzel
	const bool bTyping = NameBox.IsValid() && NameBox->HasAnyUserFocusOrFocusedDescendants();

	if (Key == EKeys::Gamepad_LeftShoulder || (!bTyping && Key == EKeys::Q))
	{
		StepTab(-1);
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_RightShoulder || (!bTyping && Key == EKeys::E))
	{
		StepTab(1);
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_FaceButton_Left || (InKeyEvent.IsControlDown() && Key == EKeys::Z))
	{
		Cr->Undo();
		return FReply::Handled();
	}
	if (InKeyEvent.IsControlDown() && Key == EKeys::Y)
	{
		Cr->Redo();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_FaceButton_Top || (!bTyping && Key == EKeys::R))
	{
		Cr->Randomize();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_Special_Right)
	{
		PrimaryAction();
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_FaceButton_Right || Key == EKeys::Escape)
	{
		BackAction();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void SEwigfelsCreatorWidget::StepTab(int32 Direction)
{
	if (UEwigfelsCreatorSubsystem* Cr = C())
	{
		// Geführt: Weiter/Zurück ohne Umlauf, LB/RB im Laden laufen rundherum
		Cr->StepTab(Direction, !Cr->IsGuided());
	}
}

void SEwigfelsCreatorWidget::PrimaryAction()
{
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr || Cr->IsWaitingForServer())
	{
		return;
	}
	if (Cr->IsGuided() && !Cr->IsLastTab())
	{
		StepTab(1);
		return;
	}
	Cr->Confirm();
}

void SEwigfelsCreatorWidget::BackAction()
{
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr)
	{
		return;
	}
	if (Cr->IsGuided())
	{
		StepTab(-1);
		return;
	}
	Cr->Cancel();
}

// ---------------------------------------------------------------------------
// Inhalt

void SEwigfelsCreatorWidget::RebuildPanel()
{
	PanelBox->ClearChildren();
	NameBox.Reset();
	FocusFirst.Reset();
	FocusSelected.Reset();
	UEwigfelsCreatorSubsystem* Cr = C();
	if (!Cr)
	{
		return;
	}
	switch (Cr->GetTab())
	{
	case EEwigfelsCreatorTab::Head: BuildHead(); break;
	case EEwigfelsCreatorTab::Hair: BuildHair(); break;
	case EEwigfelsCreatorTab::Body: BuildBody(); break;
	case EEwigfelsCreatorTab::Marks: BuildMarks(); break;
	case EEwigfelsCreatorTab::Outfit: BuildOutfit(); break;
	case EEwigfelsCreatorTab::Ring: BuildRing(); break;
	case EEwigfelsCreatorTab::Voice: BuildVoice(); break;
	case EEwigfelsCreatorTab::Name: BuildName(); break;
	case EEwigfelsCreatorTab::Count: break;
	}
	bFocusPending = true;
}

void SEwigfelsCreatorWidget::BuildHead()
{
	using namespace EwigfelsLook;
	UEwigfelsCreatorSubsystem* Cr = C();
	if (Cr->IsAreaEditable(Change::Face))
	{
		const bool bSkinToo = Cr->IsAreaEditable(Change::Skin);
		AddSection(LOCTEXT("Heads", "Kopf"));
		TArray<FTile> Tiles;
		for (const FEwigfelsLookOption* Option : GetChoices(Cat::Face, NAME_None, Cr->GetWorking().FaceA))
		{
			const FName Id = Option->Id;
			const FColor Skin = Option->Tint;
			const bool bTint = Option->bHasTint && bSkinToo;
			FTile Tile;
			Tile.Label = Option->DisplayName;
			Tile.Note = PriceNote(*Option);
			Tile.Image = GetThumb(*Option);
			Tile.bSolidCircle = Option->bHasTint;
			Tile.Color = Option->bHasTint ? FLinearColor::FromSRGBColor(Skin) : CrAccent(EEwigfelsCreatorTab::Head);
			Tile.IsSelected = [this, Id]() { return C() && C()->GetWorking().FaceA == Id; };
			Tile.OnPick = [this, Id, Skin, bTint]()
			{
				C()->Edit([Id, Skin, bTint](FEwigfelsAppearance& L)
				{
					L.FaceA = Id;
					L.FaceB = Id;
					L.FaceBlend = 0.f;
					if (bTint)
					{
						L.SkinColor = Skin; // jeder Kopf bringt seinen Hautton mit, darunter frei änderbar
					}
				});
			};
			Tiles.Add(MoveTemp(Tile));
		}
		AddTiles(Tiles);
	}
	if (Cr->IsAreaEditable(Change::Skin))
	{
		AddSwatches(LOCTEXT("Skin", "Hautton"), TEXT("skin"),
			[](const FEwigfelsAppearance& L) { return L.SkinColor; }, [](FEwigfelsAppearance& L, FColor V) { L.SkinColor = V; });
	}
	if (Cr->IsAreaEditable(Change::Eyes))
	{
		AddSwatches(LOCTEXT("Eyes", "Augenfarbe"), TEXT("eyes"),
			[](const FEwigfelsAppearance& L) { return L.EyeColorL; },
			[](FEwigfelsAppearance& L, FColor V) { L.EyeColorL = V; L.EyeColorR = V; });
	}
}

void SEwigfelsCreatorWidget::BuildHair()
{
	using namespace EwigfelsLook;
	UEwigfelsCreatorSubsystem* Cr = C();
	if (Cr->IsAreaEditable(Change::Hair))
	{
		AddSection(LOCTEXT("HairStyle", "Frisur"));
		AddOptionTiles(Cat::Hair, NAME_None,
			[](const FEwigfelsAppearance& L) { return L.HairStyle; }, [](FEwigfelsAppearance& L, FName V) { L.HairStyle = V; });
		AddSwatches(LOCTEXT("HairColor", "Haarfarbe"), TEXT("hair"),
			[](const FEwigfelsAppearance& L) { return L.HairColor; },
			[](FEwigfelsAppearance& L, FColor V) { L.HairColor = V; L.HairTipColor = V; });
	}
	if (Cr->IsAreaEditable(Change::Brows))
	{
		AddSection(LOCTEXT("Brows", "Augenbrauen"));
		AddOptionTiles(Cat::Brows, NAME_None,
			[](const FEwigfelsAppearance& L) { return L.Brows; }, [](FEwigfelsAppearance& L, FName V) { L.Brows = V; });
	}
	if (Cr->IsAreaEditable(Change::Beard))
	{
		AddSection(LOCTEXT("Beard", "Bart"));
		AddOptionTiles(Cat::Beard, NAME_None,
			[](const FEwigfelsAppearance& L) { return L.Beard; }, [](FEwigfelsAppearance& L, FName V) { L.Beard = V; });
	}
}

void SEwigfelsCreatorWidget::BuildBody()
{
	const FEwigfelsLookCatalog& Cat = C()->GetCatalog();
	AddSection(LOCTEXT("BodyType", "Körperbau"));
	AddOptionTiles(EwigfelsLook::Cat::Body, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.BodyType; }, [](FEwigfelsAppearance& L, FName V) { L.BodyType = V; });

	// Größe: drei Stufen statt Regler
	AddSection(LOCTEXT("Height", "Größe"));
	const float Heights[3] = { FMath::Lerp(1.f, Cat.HeightMin, 0.6f), 1.f, FMath::Lerp(1.f, Cat.HeightMax, 0.6f) };
	const FText Names[3] = { LOCTEXT("Smaller", "Kleiner"), LOCTEXT("Average", "Mittel"), LOCTEXT("Taller", "Größer") };
	TArray<FTile> Tiles;
	for (int32 i = 0; i < 3; ++i)
	{
		const float Value = Heights[i];
		const float A = Heights[0], B = Heights[1], Cc = Heights[2];
		FTile Tile;
		Tile.Label = Names[i];
		Tile.Note = FText::Format(LOCTEXT("Cm", "{0} cm"), FMath::RoundToInt(172.f * Value));
		Tile.Color = CrAccent(EEwigfelsCreatorTab::Body);
		Tile.IsSelected = [this, Value, A, B, Cc]()
		{
			if (!C())
			{
				return false;
			}
			// die nächstliegende Stufe gilt als gewählt (auch bei Zufallsgrößen)
			const float H = C()->GetWorking().Height;
			const float Best = FMath::Abs(H - A) <= FMath::Min(FMath::Abs(H - B), FMath::Abs(H - Cc)) ? A
				: FMath::Abs(H - B) <= FMath::Abs(H - Cc) ? B : Cc;
			return Best == Value;
		};
		Tile.OnPick = [this, Value]() { C()->Edit([Value](FEwigfelsAppearance& L) { L.Height = Value; }); };
		Tiles.Add(MoveTemp(Tile));
	}
	AddTiles(Tiles);
}

void SEwigfelsCreatorWidget::BuildMarks()
{
	const FEwigfelsLookCatalog& Cat = C()->GetCatalog();
	const int32 MaxMarks = Cat.MaxMarks;
	TMap<FName, TArray<FTile>> ByKind;
	TArray<FName> KindOrder;
	for (const FEwigfelsLookOption& Option : Cat.GetOptions(EwigfelsLook::Cat::Marks))
	{
		const FName Id = Option.Id;
		const FName Kind = Option.Kind;
		if (!C()->GetWorking().FindMark(Id) && !C()->IsOptionAvailable(Option))
		{
			continue;
		}
		const FColor MarkColor = CrMarkColor(Kind);
		FTile Tile;
		Tile.Label = Option.DisplayName;
		Tile.Note = PriceNote(Option);
		Tile.Image = GetThumb(Option);
		Tile.Color = CrAccent(EEwigfelsCreatorTab::Marks);
		Tile.IsSelected = [this, Id]() { return C() && C()->GetWorking().FindMark(Id) != nullptr; };
		Tile.OnPick = [this, Id, MaxMarks, MarkColor]()
		{
			C()->Edit([Id, MaxMarks, MarkColor](FEwigfelsAppearance& L)
			{
				if (L.Marks.RemoveAll([Id](const FEwigfelsMarkLook& M) { return M.Id == Id; }) == 0 && L.Marks.Num() < MaxMarks)
				{
					FEwigfelsMarkLook Mark;
					Mark.Id = Id;
					Mark.Color = MarkColor;
					Mark.Opacity = 0.8f;
					L.Marks.Add(Mark);
				}
			});
		};
		if (!ByKind.Contains(Kind))
		{
			KindOrder.Add(Kind);
		}
		ByKind.FindOrAdd(Kind).Add(MoveTemp(Tile));
	}
	for (const FName Kind : KindOrder)
	{
		AddSection(CrKindName(Kind));
		AddTiles(ByKind[Kind], true);
	}
	if (KindOrder.Num() == 0)
	{
		AddInfo(LOCTEXT("NoMarks", "Hier gibt es gerade nichts zum Aussuchen."));
	}
}

void SEwigfelsCreatorWidget::BuildOutfit()
{
	const FEwigfelsLookCatalog& Cat = C()->GetCatalog();
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

	// Welches Teil? (kleine Kacheln, Wechsel baut die Seite neu)
	if (Slots.Num() > 1)
	{
		TArray<FTile> SlotTiles;
		for (const FName Slot : Slots)
		{
			FTile Tile;
			Tile.Label = CrSlotName(Slot);
			Tile.Color = CrAccent(EEwigfelsCreatorTab::Outfit);
			Tile.IsSelected = [this, Slot]() { return OutfitSlot == Slot; };
			Tile.OnPick = [this, Slot]() { if (OutfitSlot != Slot) { OutfitSlot = Slot; bRebuildPending = true; } };
			SlotTiles.Add(MoveTemp(Tile));
		}
		AddSection(LOCTEXT("SlotHeader", "Was möchtest du ändern?"));
		AddTiles(SlotTiles, true);
	}

	const FName Slot = OutfitSlot;
	if (C()->IsAreaEditable(CrOutfitArea(Slot)))
	{
		AddSection(CrSlotName(Slot));
		AddOptionTiles(EwigfelsLook::Cat::Outfit, Slot,
			[Slot](const FEwigfelsAppearance& L) { const FEwigfelsOutfitPiece* P = L.FindPiece(Slot); return P ? P->Item : NAME_None; },
			[Slot](FEwigfelsAppearance& L, FName V) { L.FindOrAddPiece(Slot).Item = V; });
	}
	if (C()->IsAreaEditable(CrOutfitColorArea(Slot)))
	{
		AddSwatches(LOCTEXT("Dye1", "Farbe"), TEXT("dye"),
			[Slot](const FEwigfelsAppearance& L) { const FEwigfelsOutfitPiece* P = L.FindPiece(Slot); return P ? P->Color1 : FColor::Black; },
			[Slot](FEwigfelsAppearance& L, FColor V) { L.FindOrAddPiece(Slot).Color1 = V; });
		AddSwatches(LOCTEXT("Dye2", "Zweite Farbe"), TEXT("dye"),
			[Slot](const FEwigfelsAppearance& L) { const FEwigfelsOutfitPiece* P = L.FindPiece(Slot); return P ? P->Color2 : FColor::Black; },
			[Slot](FEwigfelsAppearance& L, FColor V) { L.FindOrAddPiece(Slot).Color2 = V; });

		TArray<FTile> Extra;
		FTile DyeAll;
		DyeAll.Label = LOCTEXT("DyeAll", "Diese Farben für alles");
		DyeAll.Color = CrGold;
		DyeAll.OnPick = [this, Slot]()
		{
			C()->Edit([Slot](FEwigfelsAppearance& L)
			{
				const FEwigfelsOutfitPiece Source = L.FindOrAddPiece(Slot);
				for (FEwigfelsOutfitPiece& P : L.Outfit)
				{
					P.Color1 = Source.Color1;
					P.Color2 = Source.Color2;
				}
			});
		};
		Extra.Add(MoveTemp(DyeAll));
		AddTiles(Extra, true);
	}
}

void SEwigfelsCreatorWidget::BuildRing()
{
	AddSection(LOCTEXT("Metal", "Metall"));
	AddOptionTiles(EwigfelsLook::Cat::Metal, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.Rings.Num() ? L.Rings[0].Metal : NAME_None; },
		[](FEwigfelsAppearance& L, FName V) { if (L.Rings.Num() == 0) { L.Rings.AddDefaulted(); } L.Rings[0].Metal = V; });
	AddSwatches(LOCTEXT("Quartz", "Farbe deiner Magie"), TEXT("quartz"),
		[](const FEwigfelsAppearance& L) { return L.Rings.Num() ? L.Rings[0].Quartz : FColor::White; },
		[](FEwigfelsAppearance& L, FColor V) { if (L.Rings.Num() == 0) { L.Rings.AddDefaulted(); } L.Rings[0].Quartz = V; }, true);
}

void SEwigfelsCreatorWidget::BuildVoice()
{
	AddSection(LOCTEXT("Voice", "Stimmlage"));
	AddOptionTiles(EwigfelsLook::Cat::Voice, NAME_None,
		[](const FEwigfelsAppearance& L) { return L.Voice; },
		[](FEwigfelsAppearance& L, FName V) { L.Voice = V; L.VoicePitch = 0.f; },
		[this]() { if (C()) { C()->PlayVoiceSample(); } });
}

void SEwigfelsCreatorWidget::BuildName()
{
	UEwigfelsCreatorSubsystem* Cr = C();
	const int32 MaxLen = Cr->GetCatalog().NameMax;
	if (Cr->IsAreaEditable(EwigfelsLook::Change::Name))
	{
		AddSection(LOCTEXT("NameHeader", "Dein Name"));
		PanelBox->AddSlot().AutoHeight().Padding(0.f, 4.f, 0.f, 6.f)
		[
			SNew(SBorder).BorderImage(CrRound(12.f)).Padding(3.f)
			.BorderBackgroundColor_Lambda([this]()
			{
				const bool bFocus = NameBox.IsValid() && NameBox->HasAnyUserFocusOrFocusedDescendants();
				return FSlateColor(bFocus ? CrGlow : CrAlpha(CrAccent(EEwigfelsCreatorTab::Name), 0.6f));
			})
			[
				SAssignNew(NameBox, SEditableTextBox)
				.Font(CrFont(20, true))
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
			]
		];
		if (Cr->GetCatalog().RandomNames.Num())
		{
			TArray<FTile> Dice;
			FTile Tile;
			Tile.Label = LOCTEXT("DiceName", "Namen würfeln");
			Tile.Color = CrGold;
			Tile.OnPick = [this]() { if (C()) { C()->RandomizeName(); } };
			Dice.Add(MoveTemp(Tile));
			AddTiles(Dice, true);
		}
	}

	AddSection(LOCTEXT("Pronouns", "Wie sollen andere von dir sprechen?"));
	const FText Choices[3] = { LOCTEXT("PronNeutral", "Nur mit Namen"), LOCTEXT("PronSie", "sie / ihr"), LOCTEXT("PronEr", "er / ihm") };
	TArray<FTile> Tiles;
	for (int32 i = 0; i < 3; ++i)
	{
		const EEwigfelsPronouns Value = static_cast<EEwigfelsPronouns>(i);
		FTile Tile;
		Tile.Label = Choices[i];
		Tile.Color = CrAccent(EEwigfelsCreatorTab::Name);
		Tile.IsSelected = [this, Value]() { return C() && C()->GetWorking().Pronouns == Value; };
		Tile.OnPick = [this, Value]() { C()->Edit([Value](FEwigfelsAppearance& L) { L.Pronouns = Value; }); };
		Tiles.Add(MoveTemp(Tile));
	}
	AddTiles(Tiles, true);
	AddInfo(LOCTEXT("PronInfo", "Die Anrede hängt nicht an Körper oder Stimme. Alles lässt sich frei kombinieren."));
	if (Cr->IsCreation())
	{
		AddInfo(LOCTEXT("CreateInfo", "Keine Sorge: Kleidung, Haare und Schmuck änderst du später in den Läden von Velmark. Das Gesicht bei der Spiegelhexe im Raunwald."));
	}
}

// ---------------------------------------------------------------------------
// Bausteine

void SEwigfelsCreatorWidget::AddSection(const FText& Title)
{
	const FLinearColor Accent = C() ? CrAccent(C()->GetTab()) : CrGold;
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 14.f, 0.f, 6.f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0.f, 0.f, 8.f, 0.f)
		[
			SNew(SBox).WidthOverride(8.f).HeightOverride(8.f)
			[
				SNew(SBorder).BorderImage(CrRound(6.f)).BorderBackgroundColor(Accent)
			]
		]
		+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(Title).Font(CrFont(15, true)).ColorAndOpacity(CrTextColor)
		]
	];
}

void SEwigfelsCreatorWidget::AddInfo(const FText& Text)
{
	PanelBox->AddSlot().AutoHeight().Padding(0.f, 6.f, 0.f, 4.f)
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
		if (!SlotFilter.IsNone() && Option.Slot != SlotFilter)
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

FText SEwigfelsCreatorWidget::PriceNote(const FEwigfelsLookOption& Option) const
{
	return C() && !C()->IsCreation() && Option.Price > 0 ? FText::Format(LOCTEXT("PriceNote", "+{0} Kronen"), Option.Price) : FText::GetEmpty();
}

const FSlateBrush* SEwigfelsCreatorWidget::GetThumb(const FEwigfelsLookOption& Option)
{
	const FName Key(*FString::Printf(TEXT("%s/%s"), *Option.Category.ToString(), *Option.Id.ToString()));
	if (const TSharedPtr<FSlateBrush>* Found = ThumbBrushes.Find(Key))
	{
		return Found->Get();
	}
	// Bild aus dem Katalog, sonst nach Namensregel (Vorschaubilder kommen später aus dem Editor)
	const FString Id = Option.Id.ToString();
	const FSoftObjectPath Path = !Option.Thumb.IsNull() ? Option.Thumb
		: FSoftObjectPath(FString::Printf(TEXT("/Game/UI/Creator/Thumbs/T_%s.T_%s"), *Id, *Id));
	UTexture2D* Texture = Cast<UTexture2D>(EwigfelsSpellVisuals::LoadIfExists(Path));
	TSharedPtr<FSlateBrush> Brush;
	if (Texture)
	{
		Brush = MakeShared<FSlateBrush>();
		Brush->SetResourceObject(Texture);
		Brush->ImageSize = FVector2D(128.f, 128.f);
		ThumbTextures.Emplace(Texture);
	}
	ThumbBrushes.Add(Key, Brush);
	return Brush.Get();
}

void SEwigfelsCreatorWidget::AddOptionTiles(FName Category, FName SlotFilter, FGetName Get, FSetName Set, TFunction<void()> OnPicked)
{
	UEwigfelsCreatorSubsystem* Cr = C();
	const FLinearColor Accent = CrAccent(Cr->GetTab());
	TArray<FTile> Tiles;
	for (const FEwigfelsLookOption* Option : GetChoices(Category, SlotFilter, Get(Cr->GetWorking())))
	{
		const FName Id = Option->Id;
		FTile Tile;
		Tile.Label = Option->DisplayName;
		Tile.Note = PriceNote(*Option);
		Tile.Image = GetThumb(*Option);
		Tile.bSolidCircle = Option->bHasTint;
		Tile.Color = Option->bHasTint ? FLinearColor::FromSRGBColor(Option->Tint) : Accent;
		Tile.IsSelected = [this, Get, Id]() { return C() && Get(C()->GetWorking()) == Id; };
		Tile.OnPick = [this, Set, Id, OnPicked]()
		{
			C()->Edit([&Set, Id](FEwigfelsAppearance& L) { Set(L, Id); });
			if (OnPicked)
			{
				OnPicked();
			}
		};
		Tiles.Add(MoveTemp(Tile));
	}
	AddTiles(Tiles);
}

void SEwigfelsCreatorWidget::AddTiles(const TArray<FTile>& Tiles, bool bSmall)
{
	TSharedRef<SWrapBox> Wrap = SNew(SWrapBox).UseAllottedSize(true);
	for (const FTile& Tile : Tiles)
	{
		Wrap->AddSlot().Padding(5.f)[MakeTile(Tile, bSmall)];
	}
	PanelBox->AddSlot().AutoHeight()[Wrap];
}

TSharedRef<SWidget> SEwigfelsCreatorWidget::MakeTile(const FTile& Tile, bool bSmall)
{
	const float Width = bSmall ? 128.f : 124.f;
	const float Height = bSmall ? 52.f : 150.f;
	TSharedRef<TWeakPtr<SButton>> Self = MakeShared<TWeakPtr<SButton>>();
	const TFunction<bool()> IsSelected = Tile.IsSelected;
	const TFunction<void()> OnPick = Tile.OnPick;
	const FLinearColor Color = Tile.Color;

	auto IsLit = [Self]()
	{
		const TSharedPtr<SButton> B = Self->Pin();
		return B.IsValid() && B->HasAnyUserFocus().IsSet();
	};
	auto IsHover = [Self]()
	{
		const TSharedPtr<SButton> B = Self->Pin();
		return B.IsValid() && B->IsHovered();
	};

	// Bild: Vorschau, sonst Kreis (Hautton/Metall) oder Kreis mit Anfangsbuchstabe
	TSharedRef<SWidget> Picture = SNullWidget::NullWidget;
	if (!bSmall)
	{
		if (Tile.Image)
		{
			Picture = SNew(SBox).WidthOverride(96.f).HeightOverride(96.f)[SNew(SImage).Image(Tile.Image)];
		}
		else
		{
			const FString Label = Tile.Label.ToString();
			const FText Initial = Tile.bSolidCircle || Label.IsEmpty() ? FText::GetEmpty() : FText::FromString(Label.Left(1).ToUpper());
			Picture = SNew(SBox).WidthOverride(84.f).HeightOverride(84.f)
				[
					SNew(SBorder)
					.BorderImage(CrRound(40.f))
					.HAlign(HAlign_Center).VAlign(VAlign_Center)
					.BorderBackgroundColor_Lambda([Color, IsSelected]()
					{
						// gewählt: sanftes Leuchten
						const bool bSel = IsSelected && IsSelected();
						return FSlateColor(bSel ? FMath::Lerp(Color, FLinearColor::White, 0.12f * CrPulse(3.f)) : CrAlpha(Color, 0.85f));
					})
					[
						SNew(STextBlock).Text(Initial).Font(CrFont(30, true)).ColorAndOpacity(FLinearColor(0.08f, 0.03f, 0.15f, 0.8f))
					]
				];
		}
	}

	TSharedRef<SVerticalBox> Content = SNew(SVerticalBox);
	if (!bSmall)
	{
		Content->AddSlot().FillHeight(1.f).HAlign(HAlign_Center).VAlign(VAlign_Center)[Picture];
	}
	Content->AddSlot().AutoHeight().HAlign(HAlign_Center).Padding(4.f, bSmall ? 0.f : 4.f, 4.f, 0.f)
	[
		SNew(STextBlock).Text(Tile.Label).Font(CrFont(bSmall ? 12 : 12, true)).Justification(ETextJustify::Center).AutoWrapText(true)
		.ColorAndOpacity(CrTextColor)
	];
	if (!Tile.Note.IsEmpty())
	{
		Content->AddSlot().AutoHeight().HAlign(HAlign_Center)
		[
			SNew(STextBlock).Text(Tile.Note).Font(CrFont(10)).ColorAndOpacity(CrGold)
		];
	}

	TSharedPtr<SButton> Button;
	TSharedRef<SWidget> Result = SNew(SBox)
		.WidthOverride(Width)
		.HeightOverride(Height)
		.RenderTransformPivot(FVector2D(0.5f, 0.5f))
		.RenderTransform_Lambda([IsLit, IsHover]() -> TOptional<FSlateRenderTransform>
		{
			// Fokus/Maus: Kachel wächst ein wenig
			return FSlateRenderTransform(FScale2f(IsLit() ? 1.07f : IsHover() ? 1.04f : 1.f));
		})
		[
			SAssignNew(Button, SButton)
			.ButtonStyle(CrNoBorder())
			.ContentPadding(0.f)
			.OnClicked_Lambda([this, OnPick]()
			{
				if (OnPick && C() && !C()->IsWaitingForServer())
				{
					OnPick();
				}
				return FReply::Handled();
			})
			[
				// Rahmen: Gold = gewählt, Türkis = Fokus, Flieder = Maus
				SNew(SBorder)
				.BorderImage(CrRound(18.f))
				.Padding(3.f)
				.BorderBackgroundColor_Lambda([IsSelected, IsLit, IsHover]()
				{
					const bool bSel = IsSelected && IsSelected();
					if (IsLit())
					{
						return FSlateColor(FMath::Lerp(CrGlow, FLinearColor::White, 0.3f * CrPulse(5.f)));
					}
					if (bSel)
					{
						return FSlateColor(CrAlpha(CrGold, 0.75f + 0.25f * CrPulse(3.f)));
					}
					return FSlateColor(IsHover() ? CrHover : FLinearColor(0.55f, 0.45f, 0.9f, 0.28f));
				})
				[
					SNew(SBorder)
					.BorderImage(CrRound(16.f))
					.Padding(FMargin(6.f, bSmall ? 4.f : 8.f))
					.VAlign(VAlign_Center)
					.BorderBackgroundColor_Lambda([IsSelected, Color]()
					{
						const bool bSel = IsSelected && IsSelected();
						return FSlateColor(bSel ? FMath::Lerp(CrCard, CrAlpha(Color, 0.95f), 0.35f) : CrCard);
					})
					[
						Content
					]
				]
			]
		];
	*Self = Button;
	if (!FocusFirst.IsValid())
	{
		FocusFirst = Button;
	}
	if (!FocusSelected.IsValid() && IsSelected && IsSelected())
	{
		FocusSelected = Button;
	}
	return Result;
}

void SEwigfelsCreatorWidget::AddSwatches(const FText& Title, FName Palette, FGetColor Get, FSetColor Set, bool bGlow)
{
	AddSection(Title);
	const TArray<FColor>* Colors = C() ? C()->GetCatalog().Palettes.Find(Palette) : nullptr;
	if (!Colors)
	{
		return;
	}
	TSharedRef<SWrapBox> Wrap = SNew(SWrapBox).UseAllottedSize(true);
	for (const FColor& Color : *Colors)
	{
		const FLinearColor Lin = FLinearColor::FromSRGBColor(Color);
		TSharedRef<TWeakPtr<SButton>> Self = MakeShared<TWeakPtr<SButton>>();
		auto IsSelected = [this, Get, Color]() { return C() && Get(C()->GetWorking()) == Color; };
		TSharedPtr<SButton> Button;
		Wrap->AddSlot().Padding(4.f)
		[
			SNew(SBox).WidthOverride(46.f).HeightOverride(46.f)
			.RenderTransformPivot(FVector2D(0.5f, 0.5f))
			.RenderTransform_Lambda([Self]() -> TOptional<FSlateRenderTransform>
			{
				const TSharedPtr<SButton> B = Self->Pin();
				const bool bLit = B.IsValid() && (B->HasAnyUserFocus().IsSet() || B->IsHovered());
				return FSlateRenderTransform(FScale2f(bLit ? 1.12f : 1.f));
			})
			[
				SAssignNew(Button, SButton)
				.ButtonStyle(CrNoBorder())
				.ContentPadding(0.f)
				.OnClicked_Lambda([this, Set, Color]()
				{
					if (C())
					{
						C()->Edit([&Set, Color](FEwigfelsAppearance& L) { Set(L, Color); });
					}
					return FReply::Handled();
				})
				[
					// Ring: Gold = gewählt, Türkis = Fokus
					SNew(SBorder)
					.BorderImage(CrRound(40.f))
					.Padding(3.f)
					.BorderBackgroundColor_Lambda([Self, IsSelected, Lin, bGlow]()
					{
						const TSharedPtr<SButton> B = Self->Pin();
						if (B.IsValid() && B->HasAnyUserFocus().IsSet())
						{
							return FSlateColor(CrGlow);
						}
						if (IsSelected())
						{
							return FSlateColor(CrAlpha(CrGold, 0.8f + 0.2f * CrPulse(3.f)));
						}
						// Magie-Farben leuchten leicht in ihrer eigenen Farbe
						return FSlateColor(bGlow ? CrAlpha(Lin, 0.25f + 0.25f * CrPulse(2.f)) : FLinearColor(1.f, 1.f, 1.f, 0.12f));
					})
					[
						SNew(SBorder).BorderImage(CrRound(40.f)).BorderBackgroundColor(Lin)
					]
				]
			]
		];
		*Self = Button;
		if (!FocusFirst.IsValid())
		{
			FocusFirst = Button;
		}
	}
	PanelBox->AddSlot().AutoHeight()[Wrap];
}

#undef LOCTEXT_NAMESPACE
