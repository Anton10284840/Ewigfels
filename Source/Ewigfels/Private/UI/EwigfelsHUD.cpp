// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "UI/EwigfelsHUD.h"
#include "AI/EwigfelsEnemy.h"
#include "Camera/EwigfelsDialogueCameraComponent.h"
#include "Character/EwigfelsCharacter.h"
#include "Character/EwigfelsLockOnComponent.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Combat/EwigfelsCombatDirector.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "Magic/EwigfelsSpellSubsystem.h"
#include "UI/EwigfelsSpellbookComponent.h"
#include "World/EwigfelsInteractable.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "RenderUtils.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"

#define LOCTEXT_NAMESPACE "EwigfelsHUD"

namespace
{
	// Farben: Nachthimmel-Violett als Grund, HudGold als Zierde, kräftige Edelstein-Töne
	const FLinearColor HudGold(1.f, 0.8f, 0.42f);
	const FLinearColor HudNight(0.07f, 0.035f, 0.16f, 0.82f);
	const FLinearColor HudNightEdge(0.02f, 0.01f, 0.06f, 0.9f);
	const FLinearColor HudRose(1.f, 0.3f, 0.48f);
	const FLinearColor HudRoseLight(1.f, 0.66f, 0.55f);
	const FLinearColor HudAqua(0.3f, 0.85f, 1.f);
	const FLinearColor HudAquaLight(0.7f, 1.f, 0.95f);
	const FLinearColor HudSoft(0.94f, 0.94f, 1.f);
	const FLinearColor HudClear(0.f, 0.f, 0.f, 0.f);

	UFont* Small()  { return GEngine->GetSmallFont(); }
	UFont* Medium() { return GEngine->GetMediumFont(); }
	UFont* Large()  { return GEngine->GetLargeFont(); }

	FLinearColor WithAlpha(const FLinearColor& C, float A) { return FLinearColor(C.R, C.G, C.B, A); }
	FLinearColor Scaled(const FLinearColor& C, float K) { return FLinearColor(C.R * K, C.G * K, C.B * K, C.A); }

	FString StatusLabel(const UEwigfelsSpellSubsystem* Spells, FName Id)
	{
		const FEwigfelsStatusDef* Def = Spells ? Spells->FindStatus(Id) : nullptr;
		return Def && !Def->Name.IsEmpty() ? Def->Name.ToString() : Id.ToString();
	}

	FVector2D OnCircle(const FVector2D& C, float R, float Frac)
	{
		const float A = Frac * 2.f * PI;
		return FVector2D(C.X + FMath::Sin(A) * R, C.Y - FMath::Cos(A) * R);
	}

	/** Symbol pro Gerät: Form 0 = runde Knopf-Taste, 1 = Schulter/Trigger-Pille, 2 = Tastenkappe. */
	struct FGlyphInfo
	{
		FString Label;
		FLinearColor Color;
		uint8 Shape = 2;
	};

	FGlyphInfo GlyphInfo(AEwigfelsHUD::EAction Action, bool bPad)
	{
		using A = AEwigfelsHUD::EAction;
		const FLinearColor PadGreen(0.35f, 0.9f, 0.4f), PadRed(1.f, 0.35f, 0.35f), PadBlue(0.35f, 0.6f, 1.f), PadYellow(1.f, 0.85f, 0.3f);
		const FLinearColor Pill(0.85f, 0.8f, 1.f);
		if (bPad)
		{
			switch (Action)
			{
			case A::Move:         return { TEXT("LS"), Pill, 0 };
			case A::Sprint:       return { TEXT("L3"), Pill, 0 };
			case A::Jump:         return { TEXT("A"), PadGreen, 0 };
			case A::Dodge:        return { TEXT("B"), PadRed, 0 };
			case A::Interact:     return { TEXT("X"), PadBlue, 0 };
			case A::CastSelected: return { TEXT("Y"), PadYellow, 0 };
			case A::Cast:         return { TEXT("RT"), Pill, 1 };
			case A::Shield:       return { TEXT("LT"), Pill, 1 };
			case A::Wheel:        return { TEXT("RB"), Pill, 1 };
			case A::Switch:       return { TEXT("LB"), Pill, 1 };
			case A::LockOn:       return { TEXT("R3"), Pill, 0 };
			case A::QuickSlots:   return { TEXT("Steuerkreuz"), Pill, 1 };
			case A::Help:         return { TEXT("View"), Pill, 1 };
			case A::Book:         return { TEXT("Menü"), Pill, 1 };
			case A::Confirm:      return { TEXT("A"), PadGreen, 0 };
			case A::Back:         return { TEXT("B"), PadRed, 0 };
			case A::Browse:       return { TEXT("LB RB"), Pill, 1 };
			}
		}
		switch (Action)
		{
		case A::Move:         return { TEXT("WASD"), HudSoft, 2 };
		case A::Sprint:       return { TEXT("Shift"), HudSoft, 2 };
		case A::Jump:         return { TEXT("Leertaste"), HudSoft, 2 };
		case A::Dodge:        return { TEXT("C"), HudSoft, 2 };
		case A::Interact:     return { TEXT("E"), HudSoft, 2 };
		case A::CastSelected: return { TEXT("F"), HudSoft, 2 };
		case A::Cast:         return { TEXT("L-Maus"), HudSoft, 2 };
		case A::Shield:       return { TEXT("R-Maus"), HudSoft, 2 };
		case A::Wheel:        return { TEXT("Q"), HudSoft, 2 };
		case A::Switch:       return { TEXT("Mausrad"), HudSoft, 2 };
		case A::LockOn:       return { TEXT("Tab"), HudSoft, 2 };
		case A::QuickSlots:   return { TEXT("1–6"), HudSoft, 2 };
		case A::Help:         return { TEXT("F1"), HudSoft, 2 };
		case A::Book:         return { TEXT("B"), HudSoft, 2 };
		case A::Confirm:      return { TEXT("Klick"), HudSoft, 2 };
		case A::Back:         return { TEXT("Esc"), HudSoft, 2 };
		case A::Browse:       return { TEXT("Mausrad"), HudSoft, 2 };
		}
		return { TEXT("?"), HudSoft, 2 };
	}
}

void AEwigfelsHUD::BeginPlay()
{
	Super::BeginPlay();
	if (UEwigfelsCombatDirector* Director = GetWorld()->GetSubsystem<UEwigfelsCombatDirector>())
	{
		HitHandle = Director->OnHit.AddUObject(this, &AEwigfelsHUD::HandleHit);
	}
}

void AEwigfelsHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UEwigfelsCombatDirector* Director = GetWorld() ? GetWorld()->GetSubsystem<UEwigfelsCombatDirector>() : nullptr)
	{
		Director->OnHit.Remove(HitHandle);
	}
	Super::EndPlay(EndPlayReason);
}

float AEwigfelsHUD::Now() const
{
	return GetWorld() ? GetWorld()->GetRealTimeSeconds() : 0.f;
}

void AEwigfelsHUD::HandleHit(const FEwigfelsHitEvent& Event)
{
	const APawn* Me = GetOwningPawn();
	FFloatingNumber N;
	N.Location = Event.Location + FVector(0.f, 0.f, 40.f);
	N.Start = GetWorld()->GetRealTimeSeconds();
	N.Drift = FVector2D(FMath::FRandRange(-30.f, 30.f), -60.f);
	const bool bOnMe = Event.Victim.Get() == Me;
	switch (Event.Result)
	{
	case EEwigfelsHitResult::Parried:
		N.Text = LOCTEXT("Parried", "Reinklang!").ToString();
		N.Color = FLinearColor(1.f, 0.96f, 0.6f);
		N.Scale = 1.5f;
		break;
	case EEwigfelsHitResult::Blocked:
		N.Text = FString::Printf(TEXT("%.0f"), Event.Amount);
		N.Color = FLinearColor(0.6f, 0.85f, 1.f);
		N.Scale = 0.9f;
		break;
	case EEwigfelsHitResult::Ignored:
		if (!bOnMe)
		{
			return;
		}
		N.Text = LOCTEXT("Miss", "Ausgewichen").ToString();
		N.Color = FLinearColor(0.6f, 0.9f, 1.f);
		N.Scale = 0.9f;
		break;
	default:
		if (Event.Amount < 0.5f)
		{
			return;
		}
		N.Text = FString::Printf(TEXT("%.0f"), Event.Amount);
		N.Color = bOnMe ? FLinearColor(1.f, 0.4f, 0.5f) : (Event.Amount >= 25.f ? FLinearColor(1.f, 0.82f, 0.3f) : FLinearColor::White);
		N.Scale = Event.Result == EEwigfelsHitResult::Killed ? 1.6f : (Event.Amount >= 25.f ? 1.35f : 1.f);
		break;
	}
	Numbers.Add(N);
	if (Numbers.Num() > 40)
	{
		Numbers.RemoveAt(0);
	}
}

// --- Grundformen ---------------------------------------------------------

void AEwigfelsHUD::Ring(const FVector2D& C, float R0, float R1, float From, float To, const FLinearColor& Inner, const FLinearColor& Outer, bool bAdditive)
{
	if (!Canvas || R1 <= 0.f || To <= From)
	{
		return;
	}
	const int32 Segs = FMath::Clamp(FMath::CeilToInt((To - From) * FMath::Max(R1, 6.f) * 0.8f), 6, 120);
	TArray<FCanvasUVTri> Tris;
	Tris.Reserve(Segs * 2);
	for (int32 i = 0; i < Segs; ++i)
	{
		const float F0 = From + (To - From) * static_cast<float>(i) / Segs;
		const float F1 = From + (To - From) * static_cast<float>(i + 1) / Segs;
		const FVector2D A0 = OnCircle(C, R0, F0), A1 = OnCircle(C, R0, F1);
		const FVector2D B0 = OnCircle(C, R1, F0), B1 = OnCircle(C, R1, F1);
		FCanvasUVTri T1;
		T1.V0_Pos = A0; T1.V0_Color = Inner;
		T1.V1_Pos = B0; T1.V1_Color = Outer;
		T1.V2_Pos = B1; T1.V2_Color = Outer;
		Tris.Add(T1);
		if (R0 > 0.f)
		{
			FCanvasUVTri T2;
			T2.V0_Pos = A0; T2.V0_Color = Inner;
			T2.V1_Pos = B1; T2.V1_Color = Outer;
			T2.V2_Pos = A1; T2.V2_Color = Inner;
			Tris.Add(T2);
		}
	}
	FCanvasTriangleItem Item(Tris, GWhiteTexture);
	Item.BlendMode = bAdditive ? SE_BLEND_Additive : SE_BLEND_Translucent;
	Canvas->DrawItem(Item);
}

void AEwigfelsHUD::Glow(const FVector2D& C, float R, float Width, const FLinearColor& Color, float Strength)
{
	// Additiv: Farbe vormultipliziert, außen schwarz = weicher Rand
	const FLinearColor In(Color.R * Strength, Color.G * Strength, Color.B * Strength, 1.f);
	Ring(C, R, R + Width, 0.f, 1.f, In, FLinearColor::Black, true);
}

void AEwigfelsHUD::Sparkles(const FVector2D& C, float R, const FLinearColor& Color, int32 Count, float Seed)
{
	const float T = Now();
	for (int32 i = 0; i < Count; ++i)
	{
		const float Phase = Seed + i * 2.399963f; // goldener Winkel: gleichmäßig verteilt
		const float Speed = 0.06f + 0.05f * FMath::Frac(Phase * 7.31f);
		const float Frac = FMath::Frac(Phase / (2.f * PI) + T * Speed);
		const float Radius = R * (0.92f + 0.16f * FMath::Sin(T * 1.7f + Phase * 3.f));
		const float Twinkle = 0.5f + 0.5f * FMath::Sin(T * 5.f + Phase * 11.f);
		const FVector2D P = OnCircle(C, Radius, Frac);
		const float Size = S(1.2f + 1.8f * Twinkle);
		Glow(P, 0.f, Size * 3.f, Color, 0.5f * Twinkle);
		Ring(P, 0.f, Size, 0.f, 1.f, FLinearColor(1.f, 1.f, 1.f, Twinkle), WithAlpha(Color, 0.f), true);
	}
}

void AEwigfelsHUD::RoundRect(float X, float Y, float W, float H, float Radius, const FLinearColor& Color)
{
	const float R = FMath::Min3(Radius, W * 0.5f, H * 0.5f);
	DrawRect(Color, X + R, Y, W - 2.f * R, H);
	DrawRect(Color, X, Y + R, R, H - 2.f * R);
	DrawRect(Color, X + W - R, Y + R, R, H - 2.f * R);
	Ring(FVector2D(X + R, Y + R), 0.f, R, 0.75f, 1.f, Color, Color);
	Ring(FVector2D(X + W - R, Y + R), 0.f, R, 0.f, 0.25f, Color, Color);
	Ring(FVector2D(X + W - R, Y + H - R), 0.f, R, 0.25f, 0.5f, Color, Color);
	Ring(FVector2D(X + R, Y + H - R), 0.f, R, 0.5f, 0.75f, Color, Color);
}

float AEwigfelsHUD::GlyphWidth(EAction Action, bool bGamepad, float H)
{
	const FGlyphInfo G = GlyphInfo(Action, bGamepad);
	if (G.Shape == 0 && G.Label.Len() <= 2)
	{
		return H;
	}
	return FMath::Max(H, TextW(G.Label, Small(), 0.9f) + H * 0.6f);
}

float AEwigfelsHUD::Glyph(EAction Action, bool bGamepad, float X, float CY, float H)
{
	const FGlyphInfo G = GlyphInfo(Action, bGamepad);
	const float W = GlyphWidth(Action, bGamepad, H);
	if (G.Shape == 0 && G.Label.Len() <= 2)
	{
		const FVector2D C(X + H * 0.5f, CY);
		Disc(C, H * 0.5f, WithAlpha(G.Color, 0.95f));
		Disc(C, H * 0.5f - S(2.f), FLinearColor(0.05f, 0.04f, 0.1f, 0.95f));
		TextC(G.Label, C.X, C.Y, G.Color, Small(), 0.95f);
	}
	else if (G.Shape == 1 || G.Shape == 0)
	{
		RoundRect(X, CY - H * 0.5f, W, H, H * 0.5f, WithAlpha(G.Color, 0.9f));
		RoundRect(X + S(2.f), CY - H * 0.5f + S(2.f), W - S(4.f), H - S(4.f), H * 0.5f - S(2.f), FLinearColor(0.05f, 0.04f, 0.1f, 0.95f));
		TextC(G.Label, X + W * 0.5f, CY, G.Color, Small(), 0.9f);
	}
	else
	{
		// Tastenkappe: hell mit dunkler Schrift und kleinem Schatten
		RoundRect(X, CY - H * 0.5f + S(2.f), W, H, S(5.f), FLinearColor(0.3f, 0.25f, 0.45f, 0.95f));
		RoundRect(X, CY - H * 0.5f, W, H - S(1.f), S(5.f), FLinearColor(0.95f, 0.93f, 1.f, 0.97f));
		TextC(G.Label, X + W * 0.5f, CY - S(1.f), FLinearColor(0.12f, 0.08f, 0.25f), Small(), 0.9f);
	}
	return W;
}

float AEwigfelsHUD::TextW(const FString& Str, UFont* Font, float Scale)
{
	float W = 0.f, H = 0.f;
	GetTextSize(Str, W, H, Font, Scale * UIScale);
	return W;
}

void AEwigfelsHUD::Text(const FString& Str, float X, float Y, const FLinearColor& Color, UFont* Font, float Scale, bool bCenter)
{
	if (!Canvas || Str.IsEmpty())
	{
		return;
	}
	const float Sc = Scale * UIScale;
	if (bCenter)
	{
		float W = 0.f, H = 0.f;
		GetTextSize(Str, W, H, Font, Sc);
		X -= W * 0.5f;
	}
	FCanvasTextItem Item(FVector2D(X, Y), FText::FromString(Str), Font, Color);
	Item.Scale = FVector2D(Sc, Sc);
	Item.EnableShadow(FLinearColor(0.f, 0.f, 0.05f, 0.85f * Color.A), FVector2D(1.f, 1.5f));
	Item.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Item);
}

void AEwigfelsHUD::TextC(const FString& Str, float CX, float CY, const FLinearColor& Color, UFont* Font, float Scale)
{
	if (Str.IsEmpty())
	{
		return;
	}
	float W = 0.f, H = 0.f;
	GetTextSize(Str, W, H, Font, Scale * UIScale);
	Text(Str, CX - W * 0.5f, CY - H * 0.5f, Color, Font, Scale, false);
}

// --- Ablauf --------------------------------------------------------------

void AEwigfelsHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas)
	{
		return;
	}
	UIScale = FMath::Clamp(Canvas->ClipY / 1080.f, 0.6f, 2.f);

	AEwigfelsCharacter* Player = Cast<AEwigfelsCharacter>(GetOwningPawn());
	DrawFloatingNumbers();
	if (!Player)
	{
		TextC(LOCTEXT("NoPawn", "Nocturna").ToString(), Canvas->ClipX * 0.5f, S(40.f), HudGold, Large(), 1.f);
		return;
	}
	const bool bDialogue = Player->GetDialogueCamera() && Player->GetDialogueCamera()->IsDialogueActive();
	if (bDialogue)
	{
		DrawSubtitles(Player);
		return;
	}
	const float Dt = GetWorld()->GetDeltaSeconds();
	WheelBlend = FMath::FInterpConstantTo(WheelBlend, Player->IsSpellWheelOpen() ? 1.f : 0.f, Dt, 7.f);
	const UEwigfelsSpellbookComponent* Book = Player->GetSpellbook();
	BookBlend = FMath::FInterpConstantTo(BookBlend, Book && Book->IsOpen() ? 1.f : 0.f, Dt, 6.f);
	if (BookBlend > 0.99f)
	{
		DrawSpellbook(Player);
		DrawMessage(Player);
		return;
	}

	DrawEnemies(Player);
	if (WheelBlend < 0.99f)
	{
		DrawCrosshair(Player);
	}
	DrawVitals(Player);
	DrawSpellCluster(Player);
	DrawQuickSlots(Player);
	DrawInteractPrompt(Player);
	DrawHelp(Player);
	if (WheelBlend > 0.01f)
	{
		DrawWheel(Player);
	}
	if (BookBlend > 0.01f)
	{
		DrawSpellbook(Player);
	}
	DrawMessage(Player);
}

// --- Lebens-Amulett ------------------------------------------------------

void AEwigfelsHUD::DrawVitals(AEwigfelsCharacter* Player)
{
	const UEwigfelsCombatComponent* Combat = Player->GetCombat();
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	const FVector2D C(S(118.f), Canvas->ClipY - S(118.f));
	const float T = Now();

	const float HealthFrac = Combat->GetMaxHealth() > 0.f ? FMath::Clamp(Combat->GetHealth() / Combat->GetMaxHealth(), 0.f, 1.f) : 0.f;
	GhostHealth = FMath::FInterpTo(FMath::Max(GhostHealth, HealthFrac), HealthFrac, GetWorld()->GetDeltaSeconds(), 1.8f);
	const float AtemFrac = Caster->GetMaxAtem() > 0.f ? FMath::Clamp(Caster->GetAtem() / Caster->GetMaxAtem(), 0.f, 1.f) : 0.f;

	// Grundscheibe mit Sternenhimmel-Verlauf, goldener Zierrand
	Glow(C, S(72.f), S(26.f), FLinearColor(0.45f, 0.2f, 0.9f), 0.35f);
	Ring(C, 0.f, S(74.f), 0.f, 1.f, FLinearColor(0.16f, 0.07f, 0.32f, 0.9f), HudNightEdge);
	Ring(C, S(74.f), S(76.5f), 0.f, 1.f, WithAlpha(HudGold, 0.85f), WithAlpha(HudGold, 0.5f));

	// Leben: äußerer Ring, Rosé → Pfirsich, mit Nachlauf
	Ring(C, S(58.f), S(70.f), 0.f, 1.f, FLinearColor(0.25f, 0.04f, 0.1f, 0.7f), FLinearColor(0.25f, 0.04f, 0.1f, 0.7f));
	if (GhostHealth > HealthFrac)
	{
		Ring(C, S(58.f), S(70.f), HealthFrac, GhostHealth, FLinearColor(1.f, 0.95f, 0.9f, 0.7f), FLinearColor(1.f, 0.95f, 0.9f, 0.7f));
	}
	Ring(C, S(58.f), S(70.f), 0.f, HealthFrac, HudRose, HudRoseLight);
	const bool bLow = HealthFrac < 0.3f && HealthFrac > 0.f;
	if (bLow)
	{
		Glow(C, S(70.f), S(22.f), HudRose, 0.4f + 0.35f * FMath::Sin(T * 7.f));
	}
	if (HealthFrac > 0.01f)
	{
		const FVector2D Tip = OnCircle(C, S(64.f), HealthFrac);
		Glow(Tip, 0.f, S(14.f), HudRoseLight, 0.8f);
		Disc(Tip, S(3.f), FLinearColor(1.f, 1.f, 1.f, 0.95f));
	}

	// Atem: innerer Ring, Türkis
	Ring(C, S(47.f), S(54.f), 0.f, 1.f, FLinearColor(0.02f, 0.1f, 0.18f, 0.75f), FLinearColor(0.02f, 0.1f, 0.18f, 0.75f));
	Ring(C, S(47.f), S(54.f), 0.f, AtemFrac, HudAqua, HudAquaLight);
	if (AtemFrac > 0.01f)
	{
		const FVector2D Tip = OnCircle(C, S(50.5f), AtemFrac);
		Glow(Tip, 0.f, S(10.f), HudAquaLight, 0.7f);
	}

	// Mitte: Lebenszahl groß, darunter klein
	Ring(C, 0.f, S(44.f), 0.f, 1.f, FLinearColor(0.22f, 0.1f, 0.4f, 0.6f), FLinearColor(0.05f, 0.02f, 0.12f, 0.2f));
	TextC(FString::Printf(TEXT("%.0f"), Combat->GetHealth()), C.X, C.Y - S(4.f), bLow ? HudRoseLight : HudSoft, Large(), 1.15f);
	TextC(LOCTEXT("Life", "Leben").ToString(), C.X, C.Y + S(22.f), WithAlpha(HudGold, 0.9f), Small(), 0.85f);
	Sparkles(C, S(80.f), FLinearColor(0.8f, 0.6f, 1.f), 5, 1.3f);

	DrawStatuses(Player, C.X + S(96.f), C.Y + S(30.f));
}

void AEwigfelsHUD::DrawStatuses(AEwigfelsCharacter* Player, float X, float Y)
{
	const UGameInstance* GI = GetGameInstance();
	const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	const UEwigfelsCombatComponent* Combat = Player->GetCombat();
	const float ServerNow = Combat->GetServerTime();
	float Cursor = X;
	for (const FEwigfelsActiveStatus& Status : Combat->GetActiveStatuses())
	{
		const FEwigfelsStatusDef* Def = Spells ? Spells->FindStatus(Status.Id) : nullptr;
		const FLinearColor Color = Def ? Def->Color : FLinearColor::White;
		const float Left = FMath::Max(0.f, Status.EndTime - ServerNow);
		const FString Label = FString::Printf(TEXT("%s  %.0f"), *StatusLabel(Spells, Status.Id), FMath::CeilToFloat(Left));
		const float H = S(24.f);
		const float W = TextW(Label, Small(), 0.9f) + S(26.f);
		RoundRect(Cursor, Y, W, H, H * 0.5f, WithAlpha(Scaled(Color, 0.35f), 0.9f));
		const float Frac = FMath::Clamp(Left / FMath::Max(Status.Duration, 0.01f), 0.f, 1.f);
		RoundRect(Cursor, Y, FMath::Max(H, W * Frac), H, H * 0.5f, WithAlpha(Scaled(Color, 0.75f), 0.55f));
		Disc(FVector2D(Cursor + H * 0.5f, Y + H * 0.5f), S(4.f), Color);
		TextC(Label, Cursor + W * 0.5f + S(5.f), Y + H * 0.5f, FLinearColor::White, Small(), 0.9f);
		Cursor += W + S(8.f);
	}
}

// --- Weisen --------------------------------------------------------------

void AEwigfelsHUD::SpellOrb(AEwigfelsCharacter* Player, const FEwigfelsSpellDef* Spell, const FVector2D& C, float R, bool bShine)
{
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	if (!Spell)
	{
		Ring(C, 0.f, R, 0.f, 1.f, FLinearColor(0.1f, 0.06f, 0.2f, 0.6f), FLinearColor(0.05f, 0.03f, 0.1f, 0.6f));
		return;
	}
	const float T = Now();
	const EEwigfelsCastFailReason Reason = Caster->CanCast(Spell->Id);
	const bool bNoAtem = Reason == EEwigfelsCastFailReason::NoAtem;
	const float Cd = Caster->GetCooldownRemaining(Spell->Id);
	const bool bRecast = Caster->IsRecastReady(Spell->Id);
	const bool bActive = (Caster->IsCasting() && Caster->GetCastState().SpellId == Spell->Id)
		|| (Spell->Delivery.Type == EEwigfelsSpellDelivery::Shield && Caster->IsShieldUp());

	const FLinearColor Core = Spell->Visual.Color;
	const FLinearColor GlowC = Spell->Visual.GlowColor;
	if (bShine || bActive || bRecast)
	{
		const float Pulse = bRecast ? 0.6f + 0.4f * FMath::Sin(T * 9.f) : (bActive ? 1.f : 0.55f);
		Glow(C, R * 0.9f, R * 0.75f, GlowC, Pulse);
	}
	// Kugel: heller Kern, satter Rand, kleiner Glanzpunkt oben links
	Ring(C, 0.f, R, 0.f, 1.f, WithAlpha(FLinearColor::LerpUsingHSV(Core, FLinearColor::White, 0.35f), 0.98f), WithAlpha(Scaled(GlowC, 0.45f), 0.98f));
	Ring(C, R, R + S(2.5f), 0.f, 1.f, WithAlpha(HudGold, 0.9f), WithAlpha(HudGold, 0.6f));
	Disc(C + FVector2D(-R * 0.32f, -R * 0.36f), R * 0.22f, FLinearColor(1.f, 1.f, 1.f, 0.28f));
	// Akkord: drei Noten-Perlen unten auf dem Rand
	for (int32 n = 0; n < Spell->Chord.Num() && n < 3; ++n)
	{
		const FVector2D P = OnCircle(C, R, 0.5f + (n - 1) * 0.07f);
		Disc(P, FMath::Max(S(2.5f), R * 0.1f), EwigfelsSpells::ChordNoteColor(Spell->Chord[n]));
	}
	if (bNoAtem)
	{
		Ring(C, 0.f, R, 0.f, 1.f, FLinearColor(0.05f, 0.1f, 0.25f, 0.65f), FLinearColor(0.05f, 0.1f, 0.25f, 0.65f));
	}
	if (bRecast)
	{
		TextC(LOCTEXT("Again", "nochmal!").ToString(), C.X, C.Y, FLinearColor::White, R > S(35.f) ? Medium() : Small(), 1.f);
	}
	else if (Cd > 0.f && Spell->Cooldown > 0.f)
	{
		const float Frac = FMath::Clamp(Cd / Spell->Cooldown, 0.f, 1.f);
		Ring(C, 0.f, R, 1.f - Frac, 1.f, FLinearColor(0.02f, 0.01f, 0.06f, 0.7f), FLinearColor(0.02f, 0.01f, 0.06f, 0.7f));
		TextC(Cd >= 10.f ? FString::Printf(TEXT("%.0f"), Cd) : FString::Printf(TEXT("%.1f"), Cd), C.X, C.Y, FLinearColor::White, R > S(35.f) ? Large() : Small(), 1.f);
	}
	if (bShine)
	{
		Sparkles(C, R * 1.25f, Spell->Visual.SparkColor, 7, static_cast<float>(GetTypeHash(Spell->Id) % 97));
	}
}

void AEwigfelsHUD::DrawSpellCluster(AEwigfelsCharacter* Player)
{
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	const bool bPad = Player->IsUsingGamepad();
	const float GH = S(28.f);

	// Gewählte Weise groß
	const FVector2D C(Canvas->ClipX - S(150.f), Canvas->ClipY - S(150.f));
	const float R = S(56.f);
	const FEwigfelsSpellDef* Selected = Caster->FindSpellDef(Caster->GetSlotSpell(Player->GetSelectedSlot()));
	SpellOrb(Player, Selected, C, R, true);
	Glyph(EAction::CastSelected, bPad, C.X + R * 0.55f, C.Y - R * 0.85f, GH);
	if (Selected)
	{
		TextC(Selected->Name.ToString(), C.X, C.Y + R + S(20.f), HudSoft, Medium(), 1.f);
		if (!Selected->Incantation.IsEmpty())
		{
			TextC(Selected->Incantation.ToString(), C.X, C.Y + R + S(44.f), Selected->Visual.GlowColor, Small(), 1.f);
		}
	}

	// Zauberrad-Hinweis oben
	{
		const FString Hint = LOCTEXT("WheelHint", "Zauberrad").ToString();
		const float W = GlyphWidth(EAction::Wheel, bPad, GH) + S(8.f) + TextW(Hint, Small(), 0.95f);
		const float X = C.X - W * 0.5f;
		const float Y = C.Y - R - S(40.f);
		const float GW = Glyph(EAction::Wheel, bPad, X, Y, GH);
		Text(Hint, X + GW + S(8.f), Y - S(9.f), WithAlpha(HudGold, 0.95f), Small(), 0.95f);
	}

	// Schild (oben) und Basis-Weise (unten) links daneben
	const float SR = S(28.f);
	const FVector2D ShieldC(C.X - S(122.f), C.Y - S(30.f));
	const FVector2D BasicC(C.X - S(122.f), C.Y + S(42.f));
	SpellOrb(Player, Caster->FindSpellDef(Caster->ShieldSpell), ShieldC, SR, false);
	SpellOrb(Player, Caster->FindSpellDef(Caster->BasicSpell), BasicC, SR, false);
	const float ShieldGW = GlyphWidth(EAction::Shield, bPad, GH);
	const float BasicGW = GlyphWidth(EAction::Cast, bPad, GH);
	Glyph(EAction::Shield, bPad, ShieldC.X - SR - S(10.f) - ShieldGW, ShieldC.Y, GH);
	Glyph(EAction::Cast, bPad, BasicC.X - SR - S(10.f) - BasicGW, BasicC.Y, GH);
}

void AEwigfelsHUD::DrawQuickSlots(AEwigfelsCharacter* Player)
{
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	const bool bPad = Player->IsUsingGamepad();
	const float R = S(21.f);
	const int32 Selected = Player->GetSelectedSlot();

	auto DrawSlot = [&](int32 Slot, const FVector2D& P)
	{
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetSlotSpell(Slot));
		if (!Spell)
		{
			return;
		}
		if (Slot == Selected)
		{
			Glow(P, R + S(2.f), S(10.f), Spell->Visual.GlowColor, 0.6f);
		}
		SpellOrb(Player, Spell, P, R, false);
	};

	if (bPad)
	{
		// Steuerkreuz: 4 Kugeln als Raute um ein kleines Kreuz
		const FVector2D C(Canvas->ClipX * 0.5f, Canvas->ClipY - S(80.f));
		const float D = S(46.f);
		DrawRect(FLinearColor(0.85f, 0.8f, 1.f, 0.7f), C.X - S(4.f), C.Y - S(11.f), S(8.f), S(22.f));
		DrawRect(FLinearColor(0.85f, 0.8f, 1.f, 0.7f), C.X - S(11.f), C.Y - S(4.f), S(22.f), S(8.f));
		DrawSlot(1, C + FVector2D(0.f, -D));
		DrawSlot(2, C + FVector2D(D, 0.f));
		DrawSlot(3, C + FVector2D(0.f, D));
		DrawSlot(4, C + FVector2D(-D, 0.f));
		return;
	}
	// Tastatur: Reihe 1–6
	const int32 Count = UEwigfelsSpellCasterComponent::WheelSlotCount;
	const float Gap = S(14.f);
	const float Total = Count * 2.f * R + (Count - 1) * Gap;
	float X = (Canvas->ClipX - Total) * 0.5f + R;
	const float Y = Canvas->ClipY - S(52.f);
	RoundRect(X - R - S(12.f), Y - R - S(10.f), Total + S(24.f), 2.f * R + S(20.f), R + S(10.f), WithAlpha(HudNight, 0.55f));
	for (int32 Slot = 1; Slot <= Count; ++Slot)
	{
		const FVector2D P(X, Y);
		DrawSlot(Slot, P);
		if (!Caster->GetSlotSpell(Slot).IsNone())
		{
			// Ziffer als kleine Kappe oben rechts
			const FVector2D K = P + FVector2D(R * 0.72f, -R * 0.72f);
			Disc(K, S(8.f), FLinearColor(0.95f, 0.93f, 1.f, 0.95f));
			TextC(FString::FromInt(Slot), K.X, K.Y, FLinearColor(0.12f, 0.08f, 0.25f), Small(), 0.75f);
		}
		X += 2.f * R + Gap;
	}
}

void AEwigfelsHUD::DrawWheel(AEwigfelsCharacter* Player)
{
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	const bool bPad = Player->IsUsingGamepad();
	const float B = FMath::SmoothStep(0.f, 1.f, WheelBlend);
	const float Dt = GetWorld()->GetDeltaSeconds();
	const FVector2D C(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const float RW = S(205.f) * (0.85f + 0.15f * B);

	// Welt abdunkeln, violetter Schimmer in der Mitte
	DrawRect(FLinearColor(0.03f, 0.01f, 0.08f, 0.55f * B), 0.f, 0.f, Canvas->ClipX, Canvas->ClipY);
	Glow(C, 0.f, RW * 1.6f, FLinearColor(0.4f, 0.2f, 0.8f), 0.35f * B);

	// Ringband mit goldenen Kanten
	Ring(C, RW - S(68.f), RW + S(68.f), 0.f, 1.f, FLinearColor(0.12f, 0.05f, 0.26f, 0.8f * B), FLinearColor(0.05f, 0.02f, 0.12f, 0.85f * B));
	Ring(C, RW + S(68.f), RW + S(70.5f), 0.f, 1.f, WithAlpha(HudGold, 0.8f * B), WithAlpha(HudGold, 0.4f * B));
	Ring(C, RW - S(70.5f), RW - S(68.f), 0.f, 1.f, WithAlpha(HudGold, 0.4f * B), WithAlpha(HudGold, 0.8f * B));

	const int32 Hover = Player->GetWheelHover();
	const int32 Selected = Player->GetSelectedSlot();
	const int32 Shown = Hover > 0 ? Hover : Selected;
	const int32 Count = UEwigfelsSpellCasterComponent::WheelSlotCount;
	const float Half = 0.5f / Count;
	for (int32 Slot = 1; Slot <= Count; ++Slot)
	{
		HoverGrow[Slot] = FMath::FInterpTo(HoverGrow[Slot], Slot == Hover ? 1.f : 0.f, Dt, 14.f);
		const float Frac = static_cast<float>(Slot - 1) / Count;
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetSlotSpell(Slot));
		if (Slot == Hover && Spell)
		{
			// Leuchtendes Tortenstück hinter dem gewählten Feld
			Ring(C, RW - S(68.f), RW + S(68.f), Frac - Half, Frac + Half,
				WithAlpha(Spell->Visual.GlowColor, 0.15f * B), WithAlpha(Spell->Visual.GlowColor, 0.45f * B));
		}
		const FVector2D P = OnCircle(C, RW, Frac);
		const float R = S(44.f) + S(12.f) * HoverGrow[Slot];
		SpellOrb(Player, Spell, P, R, Slot == Hover);
		if (Slot == Selected)
		{
			Disc(OnCircle(C, RW + S(60.f), Frac), S(4.f), HudGold);
		}
		if (!bPad && Spell)
		{
			const FVector2D K = P + FVector2D(R * 0.72f, -R * 0.72f);
			Disc(K, S(9.f), FLinearColor(0.95f, 0.93f, 1.f, 0.95f * B));
			TextC(FString::FromInt(Slot), K.X, K.Y, FLinearColor(0.12f, 0.08f, 0.25f), Small(), 0.8f);
		}
	}

	// Mitte: Name und Zauberwort der Weise unter dem Zeiger
	if (const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetSlotSpell(Shown)))
	{
		Glow(C, S(40.f), S(60.f), Spell->Visual.GlowColor, 0.3f * B);
		TextC(Spell->Name.ToString(), C.X, C.Y - S(22.f), WithAlpha(FLinearColor::White, B), Large(), 1.1f);
		if (!Spell->Incantation.IsEmpty())
		{
			TextC(Spell->Incantation.ToString(), C.X, C.Y + S(10.f), WithAlpha(Spell->Visual.GlowColor, B), Medium(), 1.f);
		}
		if (Spell->Cost > 0.f)
		{
			TextC(FText::Format(LOCTEXT("Cost", "{0} Atem"), FText::AsNumber(FMath::RoundToInt(Spell->Cost))).ToString(),
				C.X, C.Y + S(36.f), WithAlpha(HudAqua, B), Small(), 0.9f);
		}
	}
	// Zeiger: Funkenpunkt in Zeigerrichtung
	const FVector2D Ptr = Player->GetWheelPointer();
	if (Ptr.SizeSquared() > 0.01f)
	{
		const FVector2D P = C + FVector2D(Ptr.X, -Ptr.Y) * (RW - S(80.f));
		Glow(P, 0.f, S(16.f), HudGold, 0.9f * B);
		Disc(P, S(3.5f), WithAlpha(FLinearColor::White, B));
	}
	// Hinweis unten
	const FString Hint = bPad ? LOCTEXT("WheelPad", "Rechter Stick wählen · loslassen").ToString()
		: LOCTEXT("WheelMouse", "Maus wählen · loslassen").ToString();
	const float GH = S(28.f);
	const float W = GlyphWidth(EAction::Wheel, bPad, GH) + S(10.f) + TextW(Hint, Small(), 1.f);
	const float X = C.X - W * 0.5f;
	const float Y = C.Y + RW + S(105.f);
	const float GW = Glyph(EAction::Wheel, bPad, X, Y, GH);
	Text(Hint, X + GW + S(10.f), Y - S(9.f), WithAlpha(HudSoft, B), Small(), 1.f);
}

// --- Zielhilfe und Gegner ------------------------------------------------

void AEwigfelsHUD::DrawCrosshair(AEwigfelsCharacter* Player)
{
	if (Player->GetLockOn()->GetTarget())
	{
		return;
	}
	const FVector2D C(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetSlotSpell(Player->GetSelectedSlot()));
	const FLinearColor Tint = Spell ? Spell->Visual.GlowColor : HudGold;
	// Leuchtpunkt + vier feine Bögen
	Glow(C, 0.f, S(8.f), Tint, 0.6f);
	Disc(C, S(2.2f), FLinearColor(1.f, 1.f, 1.f, 0.95f));
	for (int32 i = 0; i < 4; ++i)
	{
		const float F = i * 0.25f;
		Ring(C, S(11.f), S(12.6f), F + 0.04f, F + 0.21f, FLinearColor(1.f, 1.f, 1.f, 0.6f), FLinearColor(1.f, 1.f, 1.f, 0.6f));
	}
}

void AEwigfelsHUD::DrawEnemies(AEwigfelsCharacter* Player)
{
	const AActor* Locked = Player->GetLockOn()->GetTarget();
	const float WorldNow = GetWorld()->GetTimeSeconds();
	const UGameInstance* GI = GetGameInstance();
	const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	for (TActorIterator<AEwigfelsEnemy> It(GetWorld()); It; ++It)
	{
		AEwigfelsEnemy* Enemy = *It;
		const UEwigfelsCombatComponent* Combat = Enemy->GetCombat();
		if (!Combat || Combat->IsDead())
		{
			continue;
		}
		const float Dist = FVector::Dist(Enemy->GetActorLocation(), Player->GetActorLocation());
		const bool bIsLocked = Enemy == Locked;
		const bool bRecentlyHit = WorldNow - Enemy->GetLastHitTime() < 4.f;
		if (Dist > 3000.f || (!bIsLocked && !bRecentlyHit && !Enemy->IsTelegraphing() && Dist > 1200.f))
		{
			continue;
		}
		const FVector Screen = Project(Enemy->GetActorLocation() + FVector(0.f, 0.f, 125.f));
		if (Screen.Z <= 0.f)
		{
			continue; // hinter der Kamera
		}
		const float W = S(bIsLocked ? 120.f : 84.f);
		const float H = S(bIsLocked ? 9.f : 7.f);
		const float Frac = Combat->GetMaxHealth() > 0.f ? FMath::Clamp(Combat->GetHealth() / Combat->GetMaxHealth(), 0.f, 1.f) : 0.f;
		const float X = Screen.X - W * 0.5f;
		RoundRect(X - S(2.f), Screen.Y - S(2.f), W + S(4.f), H + S(4.f), H, FLinearColor(0.03f, 0.01f, 0.06f, 0.8f));
		if (Frac > 0.f)
		{
			RoundRect(X, Screen.Y, FMath::Max(H, W * Frac), H, H * 0.5f, FLinearColor(0.85f, 0.2f, 0.45f));
			DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.2f), X + H * 0.5f, Screen.Y + S(1.f), FMath::Max(0.f, W * Frac - H), H * 0.3f);
		}
		if (bIsLocked)
		{
			TextC(Enemy->GetDisplayName().ToString(), Screen.X, Screen.Y - S(14.f), HudSoft, Small(), 1.f);
			// Drehende goldene Bögen um die Brust
			const FVector Chest = Project(Enemy->GetActorLocation() + FVector(0.f, 0.f, 20.f));
			const FVector2D CC(Chest.X, Chest.Y);
			const float Spin = FMath::Frac(Now() * 0.25f);
			Glow(CC, S(16.f), S(10.f), HudGold, 0.35f);
			for (int32 i = 0; i < 3; ++i)
			{
				const float F = Spin + i / 3.f;
				Ring(CC, S(16.f), S(19.f), F, F + 0.2f, WithAlpha(HudGold, 0.95f), WithAlpha(HudGold, 0.95f));
			}
		}
		// Status-Perlen (Brennen, Gefroren …)
		float DotX = X + S(5.f);
		for (const FEwigfelsActiveStatus& Status : Combat->GetActiveStatuses())
		{
			const FEwigfelsStatusDef* Def = Spells ? Spells->FindStatus(Status.Id) : nullptr;
			const FLinearColor Col = Def ? Def->Color : FLinearColor::White;
			Glow(FVector2D(DotX, Screen.Y + H + S(10.f)), 0.f, S(9.f), Col, 0.6f);
			Disc(FVector2D(DotX, Screen.Y + H + S(10.f)), S(4.f), Col);
			DotX += S(13.f);
		}
		// Vorwarnung: pulsierendes Ausrufezeichen
		if (Enemy->IsTelegraphing())
		{
			const FVector2D P(Screen.X, Screen.Y - S(bIsLocked ? 44.f : 30.f));
			Glow(P, 0.f, S(22.f), FLinearColor(1.f, 0.6f, 0.2f), 0.6f + 0.4f * FMath::Sin(Now() * 14.f));
			TextC(TEXT("!"), P.X, P.Y, FLinearColor(1.f, 0.85f, 0.4f), Large(), 1.2f);
		}
	}
}

void AEwigfelsHUD::DrawFloatingNumbers()
{
	const float T = Now();
	Numbers.RemoveAll([T](const FFloatingNumber& N) { return T - N.Start > 1.1f; });
	for (const FFloatingNumber& N : Numbers)
	{
		const float Age = T - N.Start;
		const FVector Screen = Project(N.Location);
		if (Screen.Z <= 0.f)
		{
			continue;
		}
		const float Pop = Age < 0.1f ? 1.f + (0.1f - Age) * 4.f : 1.f;
		FLinearColor C = N.Color;
		C.A = FMath::Clamp((1.1f - Age) / 0.4f, 0.f, 1.f);
		Text(N.Text, Screen.X + N.Drift.X * Age * UIScale, Screen.Y + N.Drift.Y * Age * UIScale, C, Medium(), N.Scale * Pop, true);
	}
}

// --- Hinweise ------------------------------------------------------------

void AEwigfelsHUD::DrawMessage(AEwigfelsCharacter* Player)
{
	FText Msg;
	FLinearColor Color;
	float Alpha = 0.f;
	if (Player->GetMessage(Msg, Color, Alpha))
	{
		const FVector2D P(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.3f);
		Glow(P, 0.f, S(90.f), Color, 0.25f * Alpha);
		Color.A = Alpha;
		TextC(Msg.ToString(), P.X, P.Y, Color, Large(), 1.3f);
	}
}

void AEwigfelsHUD::DrawInteractPrompt(AEwigfelsCharacter* Player)
{
	AActor* Focus = Player->GetFocusedInteractable();
	if (!Focus || !Focus->Implements<UEwigfelsInteractable>())
	{
		return;
	}
	const bool bPad = Player->IsUsingGamepad();
	const FString Prompt = IEwigfelsInteractable::Execute_GetInteractPrompt(Focus).ToString();
	const float GH = S(30.f);
	const float GW = GlyphWidth(EAction::Interact, bPad, GH);
	const float W = GW + S(12.f) + TextW(Prompt, Medium(), 1.f) + S(28.f);
	const float H = S(46.f);
	const float X = (Canvas->ClipX - W) * 0.5f;
	const float Y = Canvas->ClipY * 0.63f;
	RoundRect(X, Y, W, H, H * 0.5f, HudNight);
	Glyph(EAction::Interact, bPad, X + S(8.f), Y + H * 0.5f, GH);
	TextC(Prompt, X + S(8.f) + GW + S(12.f) + (W - GW - S(28.f)) * 0.5f, Y + H * 0.5f, HudGold, Medium(), 1.f);
}

void AEwigfelsHUD::DrawSubtitles(AEwigfelsCharacter* Player)
{
	// Kino-Balken oben/unten
	const float Band = Canvas->ClipY * 0.09f;
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.85f), 0.f, 0.f, Canvas->ClipX, Band);
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.85f), 0.f, Canvas->ClipY - Band, Canvas->ClipX, Band);

	FEwigfelsDialogueLine Line;
	if (!Player->GetDialogueCamera()->GetCurrentLine(Line))
	{
		return;
	}
	const float Y = Canvas->ClipY - Band - S(90.f);
	Text(Line.Speaker.ToString(), Canvas->ClipX * 0.5f, Y, HudGold, Medium(), 1.1f, true);
	Text(Line.Text.ToString(), Canvas->ClipX * 0.5f, Y + S(30.f), FLinearColor::White, Medium(), 1.15f, true);
	const bool bPad = Player->IsUsingGamepad();
	const float GH = S(26.f);
	const float X = Canvas->ClipX - S(190.f);
	const float CY = Canvas->ClipY - Band * 0.5f;
	const float GW = Glyph(EAction::Interact, bPad, X, CY, GH);
	Text(LOCTEXT("Continue", "weiter").ToString(), X + GW + S(8.f), CY - S(9.f), FLinearColor(1.f, 1.f, 1.f, 0.75f), Small(), 1.f);
}

void AEwigfelsHUD::DrawHelp(AEwigfelsCharacter* Player)
{
	const bool bPad = Player->IsUsingGamepad();
	const float GH = S(26.f);
	if (!Player->IsHelpVisible())
	{
		const float GW = Glyph(EAction::Help, bPad, S(20.f), S(30.f), GH);
		Text(LOCTEXT("HelpHint", "Steuerung").ToString(), S(20.f) + GW + S(8.f), S(21.f), FLinearColor(1.f, 1.f, 1.f, 0.6f), Small(), 1.f);
		return;
	}
	struct FRow { EAction Action; FText Label; };
	const TArray<FRow> Rows = {
		{ EAction::Move,         LOCTEXT("HMove", "Laufen") },
		{ EAction::Jump,         LOCTEXT("HJump", "Springen") },
		{ EAction::Dodge,        LOCTEXT("HDodge", "Ausweichen") },
		{ EAction::Cast,         LOCTEXT("HCast", "Funkenstoß") },
		{ EAction::Shield,       LOCTEXT("HShield", "Schild (kurz vor Treffer = Konter)") },
		{ EAction::Wheel,        LOCTEXT("HWheel", "Zauberrad halten") },
		{ EAction::CastSelected, LOCTEXT("HSel", "Gewählte Weise wirken") },
		{ EAction::QuickSlots,   LOCTEXT("HQuick", "Weise direkt wirken") },
		{ EAction::Book,         LOCTEXT("HBook", "Zauberbuch: 6 Weisen wählen") },
		{ EAction::LockOn,       LOCTEXT("HLock", "Ziel aufschalten") },
		{ EAction::Interact,     LOCTEXT("HTalk", "Sprechen") },
		{ EAction::Help,         LOCTEXT("HHide", "Hilfe aus") },
	};
	const float RowH = S(34.f);
	const float W = S(400.f);
	const float H = S(58.f) + Rows.Num() * RowH + S(34.f);
	const float X = S(18.f), Y = S(18.f);
	RoundRect(X, Y, W, H, S(18.f), HudNight);
	RoundRect(X, Y, W, S(4.f), S(2.f), WithAlpha(HudGold, 0.7f));
	Text(LOCTEXT("HelpTitle", "Steuerung").ToString(), X + S(20.f), Y + S(14.f), HudGold, Medium(), 1.05f);
	float RowY = Y + S(60.f);
	const float Col = X + S(20.f);
	const float LabelX = X + S(150.f);
	for (const FRow& Row : Rows)
	{
		Glyph(Row.Action, bPad, Col, RowY + RowH * 0.5f - S(4.f), GH);
		Text(Row.Label.ToString(), LabelX, RowY + S(2.f), HudSoft, Small(), 1.f);
		RowY += RowH;
	}
	Text(LOCTEXT("HTip", "Tipp: einfrieren, dann Hammerton = Zerspringen").ToString(), Col, RowY + S(4.f), WithAlpha(HudAqua, 0.9f), Small(), 0.9f);
}

// --- Zauberbuch ----------------------------------------------------------

TArray<FString> AEwigfelsHUD::Wrap(const FString& Str, UFont* Font, float Scale, float MaxWidth, int32 MaxLines)
{
	TArray<FString> Words;
	Str.ParseIntoArrayWS(Words);
	TArray<FString> Lines;
	FString Line;
	for (const FString& Word : Words)
	{
		const FString Try = Line.IsEmpty() ? Word : Line + TEXT(" ") + Word;
		if (!Line.IsEmpty() && TextW(Try, Font, Scale) > MaxWidth)
		{
			Lines.Add(Line);
			Line = Word;
			if (Lines.Num() == MaxLines)
			{
				Lines.Last() += TEXT(" …");
				return Lines;
			}
		}
		else
		{
			Line = Try;
		}
	}
	if (!Line.IsEmpty() && Lines.Num() < MaxLines)
	{
		Lines.Add(Line);
	}
	return Lines;
}

void AEwigfelsHUD::DrawSpellbook(AEwigfelsCharacter* Player)
{
	UEwigfelsSpellbookComponent* Book = Player->GetSpellbook();
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	if (!Book || !Caster)
	{
		return;
	}
	const bool bPad = Player->IsUsingGamepad();
	const float B = FMath::SmoothStep(0.f, 1.f, BookBlend);
	const float Dt = GetWorld()->GetDeltaSeconds();
	const float T = Now();
	const float CX = Canvas->ClipX * 0.5f;
	const bool bOpen = Book->IsOpen();

	// Hintergrund: tiefes Nachtviolett, Lichtschleier, Sternenstaub
	DrawRect(FLinearColor(0.04f, 0.015f, 0.1f, 0.9f * B), 0.f, 0.f, Canvas->ClipX, Canvas->ClipY);
	Glow(FVector2D(CX, Canvas->ClipY * 0.55f), 0.f, Canvas->ClipX * 0.45f, FLinearColor(0.35f, 0.15f, 0.7f), 0.4f * B);
	Glow(FVector2D(Canvas->ClipX * 0.15f, Canvas->ClipY * 0.2f), 0.f, S(380.f), FLinearColor(0.1f, 0.45f, 0.7f), 0.25f * B);
	Glow(FVector2D(Canvas->ClipX * 0.85f, Canvas->ClipY * 0.85f), 0.f, S(420.f), FLinearColor(0.7f, 0.2f, 0.5f), 0.25f * B);
	for (int32 i = 0; i < 40; ++i)
	{
		const float Px = FMath::Frac(i * 0.6180339f + 0.13f) * Canvas->ClipX;
		const float Py = FMath::Frac(i * 0.4142135f + 0.37f + T * 0.004f * (1 + i % 3)) * Canvas->ClipY;
		const float Tw = 0.4f + 0.6f * (0.5f + 0.5f * FMath::Sin(T * (1.5f + i % 5) + i));
		Glow(FVector2D(Px, Py), 0.f, S(5.f), FLinearColor(0.9f, 0.85f, 1.f), 0.6f * Tw * B);
	}

	// Titel
	TextC(LOCTEXT("BookTitle", "Zauberbuch").ToString(), CX, S(70.f), WithAlpha(HudGold, B), Large(), 1.6f);
	TextC(LOCTEXT("BookSub", "Wähle 6 Weisen für dein Zauberrad").ToString(), CX, S(112.f), WithAlpha(HudSoft, 0.85f * B), Medium(), 0.9f);

	// Obere Reihe: die 6 Rad-Plätze
	const int32 Count = UEwigfelsSpellCasterComponent::WheelSlotCount;
	const float SR = S(40.f);
	const float SGap = S(36.f);
	const float RowW = Count * 2.f * SR + (Count - 1) * SGap;
	const float SY = S(210.f);
	TArray<FBox2D> SlotRects;
	RoundRect(CX - RowW * 0.5f - S(30.f), SY - SR - S(22.f), RowW + S(60.f), 2.f * SR + S(64.f), S(30.f), WithAlpha(HudNight, 0.8f * B));
	for (int32 Slot = 1; Slot <= Count; ++Slot)
	{
		const FVector2D P(CX - RowW * 0.5f + SR + (Slot - 1) * (2.f * SR + SGap), SY);
		const bool bFocus = bOpen && Book->GetFocusedSlot() == Slot;
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Caster->GetSlotSpell(Slot));
		const float Flash = Book->GetFlashSlot() == Slot ? FMath::Clamp(1.f - (T - Book->GetFlashTime()) / 0.6f, 0.f, 1.f) : 0.f;
		if (bFocus)
		{
			const FLinearColor GlowC = Spell ? Spell->Visual.GlowColor : HudGold;
			Glow(P, SR, S(22.f), GlowC, Book->GetFocusRow() == 0 ? 0.9f : 0.45f);
		}
		if (Flash > 0.f)
		{
			Glow(P, SR * 0.5f, S(60.f) * (1.2f - Flash), FLinearColor::White, Flash);
		}
		SpellOrb(Player, Spell, P, SR * (bFocus ? 1.12f : 1.f), bFocus);
		// Platznummer
		const FVector2D K = P + FVector2D(SR * 0.8f, -SR * 0.8f);
		Disc(K, S(10.f), FLinearColor(0.95f, 0.93f, 1.f, 0.95f * B));
		TextC(FString::FromInt(Slot), K.X, K.Y, FLinearColor(0.12f, 0.08f, 0.25f), Small(), 0.8f);
		TextC(Spell ? Spell->Name.ToString() : LOCTEXT("Empty", "leer").ToString(), P.X, P.Y + SR + S(20.f),
			WithAlpha(bFocus ? FLinearColor::White : HudSoft, (bFocus ? 1.f : 0.75f) * B), Small(), 0.9f);
		SlotRects.Add(FBox2D(P - FVector2D(SR, SR), P + FVector2D(SR, SR)));
	}

	// Karten-Karussell (wie ein Kleiderschrank: groß, in der Mitte die gewählte)
	const TArray<FName> Cards = Book->GetCards();
	const int32 Focus = Book->GetFocusedCard();
	BookScroll = FMath::FInterpTo(BookScroll, static_cast<float>(Focus), Dt, 10.f);
	const float CardW = S(250.f), CardH = S(340.f), CardGap = S(34.f);
	const float CY = Canvas->ClipY * 0.6f;
	const bool bCardRow = bOpen && Book->GetFocusRow() == 1;
	TArray<FBox2D> CardRects;
	CardRects.Init(FBox2D(ForceInit), Cards.Num());
	// Hinten zuerst zeichnen, die gewählte zuletzt
	TArray<int32> Order;
	for (int32 i = 0; i < Cards.Num(); ++i)
	{
		Order.Add(i);
	}
	Order.Sort([this](int32 A, int32 Bi) { return FMath::Abs(A - BookScroll) > FMath::Abs(Bi - BookScroll); });
	for (const int32 i : Order)
	{
		const float Offset = i - BookScroll;
		if (FMath::Abs(Offset) > 3.5f)
		{
			continue;
		}
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(Cards[i]);
		if (!Spell)
		{
			continue;
		}
		const bool bFocusCard = i == Focus;
		const float Near = FMath::Clamp(1.f - FMath::Abs(Offset), 0.f, 1.f);
		const float Sc = 0.82f + 0.22f * Near;
		const float W = CardW * Sc, H = CardH * Sc;
		const float X = CX + Offset * (CardW + CardGap) - W * 0.5f;
		const float Y = CY - H * 0.5f;
		const float Fade = FMath::Clamp(1.f - (FMath::Abs(Offset) - 2.f) / 1.5f, 0.f, 1.f) * B;
		const FLinearColor Core = Spell->Visual.Color;
		const FLinearColor GlowC = Spell->Visual.GlowColor;

		if (bFocusCard)
		{
			Glow(FVector2D(X + W * 0.5f, Y + H * 0.5f), W * 0.45f, S(70.f), GlowC, (bCardRow ? 0.75f : 0.35f) * Fade);
		}
		// Rahmen in HudGold (gewählt) bzw. Edelsteinfarbe, Kartengrund mit Farbe der Weise
		RoundRect(X - S(3.f), Y - S(3.f), W + S(6.f), H + S(6.f), S(22.f), WithAlpha(bFocusCard && bCardRow ? HudGold : Scaled(GlowC, 0.6f), 0.95f * Fade));
		RoundRect(X, Y, W, H, S(20.f), WithAlpha(FLinearColor(0.08f, 0.04f, 0.17f), 0.97f * Fade));
		RoundRect(X, Y, W, H * 0.46f, S(20.f), WithAlpha(Scaled(Core, 0.35f), 0.9f * Fade));
		const FVector2D OrbC(X + W * 0.5f, Y + H * 0.24f);
		SpellOrb(Player, Spell, OrbC, S(46.f) * Sc, bFocusCard);

		// Name, Zauberwort, Text, Werte
		float TY = Y + H * 0.5f;
		TextC(Spell->Name.ToString(), X + W * 0.5f, TY, WithAlpha(FLinearColor::White, Fade), Medium(), 1.05f * Sc);
		TY += S(26.f) * Sc;
		if (!Spell->Incantation.IsEmpty())
		{
			TextC(Spell->Incantation.ToString(), X + W * 0.5f, TY, WithAlpha(GlowC, Fade), Small(), 1.05f * Sc);
			TY += S(24.f) * Sc;
		}
		for (const FString& L : Wrap(Spell->Description.ToString(), Small(), 0.85f * Sc, W - S(30.f), 4))
		{
			TextC(L, X + W * 0.5f, TY, WithAlpha(HudSoft, 0.85f * Fade), Small(), 0.85f * Sc);
			TY += S(17.f) * Sc;
		}
		const FString Stats = Spell->Cooldown > 0.f
			? FString::Printf(TEXT("%.0f Atem  ·  %.0f s"), Spell->Cost, Spell->Cooldown)
			: FString::Printf(TEXT("%.0f Atem"), Spell->Cost);
		TextC(Stats, X + W * 0.5f, Y + H - S(22.f) * Sc, WithAlpha(HudAqua, Fade), Small(), 0.85f * Sc);

		// Abzeichen: liegt schon im Rad auf Platz n
		for (int32 Slot = 1; Slot <= Count; ++Slot)
		{
			if (Caster->GetSlotSpell(Slot) == Spell->Id)
			{
				const FVector2D Badge(X + W - S(22.f), Y + S(22.f));
				Disc(Badge, S(13.f), WithAlpha(HudGold, Fade));
				TextC(FString::FromInt(Slot), Badge.X, Badge.Y, FLinearColor(0.15f, 0.08f, 0.02f, Fade), Small(), 0.85f);
				break;
			}
		}
		CardRects[i] = FBox2D(FVector2D(X, Y), FVector2D(X + W, Y + H));
	}
	if (Cards.Num() == 0)
	{
		TextC(LOCTEXT("NoCards", "Noch keine Weisen gelernt – geh in den Unterricht!").ToString(), CX, CY, WithAlpha(HudSoft, B), Medium(), 1.f);
	}
	if (bOpen)
	{
		Book->SetHitRects(SlotRects, CardRects);
	}

	// Hinweisleiste unten
	struct FHint { EAction Action; FText Label; };
	const TArray<FHint> Hints = bCardRow
		? TArray<FHint>{ { EAction::Browse, LOCTEXT("BBrowse", "blättern") }, { EAction::Confirm, LOCTEXT("BPut", "auf Platz legen") },
			{ EAction::QuickSlots, LOCTEXT("BDirect", "direkt auf Platz") }, { EAction::Back, LOCTEXT("BBack", "zurück") } }
		: TArray<FHint>{ { EAction::Confirm, LOCTEXT("BPick", "Platz wählen") }, { EAction::Back, LOCTEXT("BClose", "schließen") },
			{ EAction::Book, LOCTEXT("BClose2", "Buch zu") } };
	const float GH = S(30.f);
	float TotalW = 0.f;
	for (const FHint& Hint : Hints)
	{
		if (Hint.Action == EAction::QuickSlots && bPad)
		{
			continue;
		}
		TotalW += GlyphWidth(Hint.Action, bPad, GH) + S(10.f) + TextW(Hint.Label.ToString(), Small(), 1.f) + S(34.f);
	}
	float HX = CX - TotalW * 0.5f;
	const float HY = Canvas->ClipY - S(56.f);
	RoundRect(HX - S(20.f), HY - S(26.f), TotalW + S(40.f), S(52.f), S(26.f), WithAlpha(HudNight, 0.9f * B));
	for (const FHint& Hint : Hints)
	{
		if (Hint.Action == EAction::QuickSlots && bPad)
		{
			continue;
		}
		HX += Glyph(Hint.Action, bPad, HX, HY, GH) + S(10.f);
		Text(Hint.Label.ToString(), HX, HY - S(9.f), WithAlpha(HudSoft, B), Small(), 1.f);
		HX += TextW(Hint.Label.ToString(), Small(), 1.f) + S(34.f);
	}
}

#undef LOCTEXT_NAMESPACE
