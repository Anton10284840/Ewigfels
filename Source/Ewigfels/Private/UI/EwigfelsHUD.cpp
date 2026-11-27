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
#include "World/EwigfelsInteractable.h"
#include "CanvasItem.h"
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
	const FLinearColor Gold(1.f, 0.78f, 0.42f);
	const FLinearColor Ink(0.02f, 0.025f, 0.05f, 0.72f);
	const FLinearColor HealthColor(0.86f, 0.24f, 0.22f);
	const FLinearColor AtemColor(0.32f, 0.62f, 1.f);
	const FLinearColor Soft(0.9f, 0.92f, 1.f);

	UFont* Small()  { return GEngine->GetSmallFont(); }
	UFont* Medium() { return GEngine->GetMediumFont(); }
	UFont* Large()  { return GEngine->GetLargeFont(); }

	FString StatusLabel(const UEwigfelsSpellSubsystem* Spells, FName Id)
	{
		const FEwigfelsStatusDef* Def = Spells ? Spells->FindStatus(Id) : nullptr;
		return Def && !Def->Name.IsEmpty() ? Def->Name.ToString() : Id.ToString();
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
		N.Text = LOCTEXT("Parried", "Reinklang").ToString();
		N.Color = FLinearColor(1.f, 0.96f, 0.75f);
		N.Scale = 1.4f;
		break;
	case EEwigfelsHitResult::Blocked:
		N.Text = FString::Printf(TEXT("%.0f"), Event.Amount);
		N.Color = FLinearColor(0.7f, 0.8f, 1.f);
		N.Scale = 0.9f;
		break;
	case EEwigfelsHitResult::Ignored:
		if (!bOnMe)
		{
			return;
		}
		N.Text = LOCTEXT("Miss", "Ausgewichen").ToString();
		N.Color = FLinearColor(0.7f, 0.85f, 1.f);
		N.Scale = 0.9f;
		break;
	default:
		if (Event.Amount < 0.5f)
		{
			return;
		}
		N.Text = FString::Printf(TEXT("%.0f"), Event.Amount);
		N.Color = bOnMe ? FLinearColor(1.f, 0.45f, 0.4f) : (Event.Amount >= 25.f ? FLinearColor(1.f, 0.85f, 0.35f) : FLinearColor::White);
		N.Scale = Event.Result == EEwigfelsHitResult::Killed ? 1.5f : (Event.Amount >= 25.f ? 1.3f : 1.f);
		break;
	}
	Numbers.Add(N);
	if (Numbers.Num() > 40)
	{
		Numbers.RemoveAt(0);
	}
}

// ---------------------------------------------------------------------------

void AEwigfelsHUD::Bar(float X, float Y, float W, float H, float Fraction, const FLinearColor& Fill, const FLinearColor& Back, float Ghost)
{
	DrawRect(Back, X, Y, W, H);
	if (Ghost > Fraction)
	{
		DrawRect(FLinearColor(1.f, 0.95f, 0.85f, 0.65f), X, Y, W * FMath::Clamp(Ghost, 0.f, 1.f), H);
	}
	DrawRect(Fill, X, Y, W * FMath::Clamp(Fraction, 0.f, 1.f), H);
	// Glanzkante oben
	DrawRect(FLinearColor(1.f, 1.f, 1.f, 0.12f), X, Y, W * FMath::Clamp(Fraction, 0.f, 1.f), FMath::Max(1.f, H * 0.25f));
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
	Item.EnableShadow(FLinearColor(0.f, 0.f, 0.f, 0.8f * Color.A), FVector2D(1.f, 1.f));
	Item.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Item);
}

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
		Text(LOCTEXT("NoPawn", "Nocturna – keine Spielfigur").ToString(), Canvas->ClipX * 0.5f, S(40.f), Soft, Medium(), 1.f, true);
		return;
	}
	const bool bDialogue = Player->GetDialogueCamera() && Player->GetDialogueCamera()->IsDialogueActive();
	if (bDialogue)
	{
		DrawSubtitles(Player);
		return;
	}
	DrawEnemies(Player);
	DrawCrosshair(Player);
	DrawBars(Player);
	DrawSpellBar(Player);
	DrawInteractPrompt(Player);
	DrawMessage(Player);
	DrawHelp(Player);
}

void AEwigfelsHUD::DrawBars(AEwigfelsCharacter* Player)
{
	const UEwigfelsCombatComponent* Combat = Player->GetCombat();
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	const float X = S(48.f);
	const float Y = Canvas->ClipY - S(120.f);
	const float W = S(340.f);

	const float HealthFrac = Combat->GetMaxHealth() > 0.f ? Combat->GetHealth() / Combat->GetMaxHealth() : 0.f;
	GhostHealth = FMath::FInterpTo(FMath::Max(GhostHealth, HealthFrac), HealthFrac, GetWorld()->GetDeltaSeconds(), 1.8f);
	DrawRect(Ink, X - S(10.f), Y - S(34.f), W + S(20.f), S(96.f));
	Text(FString::Printf(TEXT("%.0f / %.0f"), Combat->GetHealth(), Combat->GetMaxHealth()), X, Y - S(28.f), Soft, Small(), 1.1f);
	Bar(X, Y, W, S(14.f), HealthFrac, HealthColor, FLinearColor(0.15f, 0.04f, 0.05f, 0.9f), GhostHealth);

	const float AtemFrac = Caster->GetMaxAtem() > 0.f ? Caster->GetAtem() / Caster->GetMaxAtem() : 0.f;
	Bar(X, Y + S(22.f), W * 0.85f, S(9.f), AtemFrac, AtemColor, FLinearColor(0.03f, 0.07f, 0.16f, 0.9f));
	Text(LOCTEXT("Atem", "Atem").ToString(), X + W * 0.85f + S(8.f), Y + S(17.f), AtemColor, Small(), 1.f);

	DrawStatuses(Player, X, Y + S(40.f));
}

void AEwigfelsHUD::DrawStatuses(AEwigfelsCharacter* Player, float X, float Y)
{
	const UGameInstance* GI = GetGameInstance();
	const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
	const UEwigfelsCombatComponent* Combat = Player->GetCombat();
	const float Now = Combat->GetServerTime();
	float Cursor = X;
	for (const FEwigfelsActiveStatus& Status : Combat->GetActiveStatuses())
	{
		const FEwigfelsStatusDef* Def = Spells ? Spells->FindStatus(Status.Id) : nullptr;
		const FLinearColor Color = Def ? Def->Color : FLinearColor::White;
		const float Left = FMath::Max(0.f, Status.EndTime - Now);
		const FString Label = FString::Printf(TEXT("%s %.0fs"), *StatusLabel(Spells, Status.Id), FMath::CeilToFloat(Left));
		float TW = 0.f, TH = 0.f;
		GetTextSize(Label, TW, TH, Small(), UIScale);
		DrawRect(FLinearColor(Color.R * 0.25f, Color.G * 0.25f, Color.B * 0.25f, 0.85f), Cursor, Y, TW + S(12.f), S(18.f));
		DrawRect(Color, Cursor, Y + S(16.f), (TW + S(12.f)) * FMath::Clamp(Left / FMath::Max(Status.Duration, 0.01f), 0.f, 1.f), S(2.f));
		Text(Label, Cursor + S(6.f), Y + S(1.f), Color, Small(), 1.f);
		Cursor += TW + S(18.f);
	}
}

void AEwigfelsHUD::DrawSpellBar(AEwigfelsCharacter* Player)
{
	const UEwigfelsSpellCasterComponent* Caster = Player->GetCaster();
	// Reihenfolge wie die Tasten: LMB, 1–4, RMB
	TArray<TPair<FString, FName>> Slots;
	Slots.Emplace(TEXT("LMB"), Caster->BasicSpell);
	int32 Index = 0;
	for (const FName Id : Caster->GetLoadout())
	{
		if (Id == Caster->BasicSpell || Id == Caster->ShieldSpell)
		{
			continue;
		}
		if (++Index > 4)
		{
			break;
		}
		Slots.Emplace(FString::FromInt(Index), Id);
	}
	Slots.Emplace(TEXT("RMB"), Caster->ShieldSpell);

	const float Size = S(68.f);
	const float Gap = S(10.f);
	const float Total = Slots.Num() * Size + (Slots.Num() - 1) * Gap;
	float X = (Canvas->ClipX - Total) * 0.5f;
	const float Y = Canvas->ClipY - Size - S(40.f);

	for (const TPair<FString, FName>& SlotPair : Slots)
	{
		const FEwigfelsSpellDef* Spell = Caster->FindSpellDef(SlotPair.Value);
		if (!Spell)
		{
			X += Size + Gap;
			continue;
		}
		const EEwigfelsCastFailReason Reason = Caster->CanCast(Spell->Id);
		const float Cd = Caster->GetCooldownRemaining(Spell->Id);
		const bool bActive = Caster->GetCastState().SpellId == Spell->Id && Caster->IsCasting();
		const bool bShieldActive = Spell->Delivery.Type == EEwigfelsSpellDelivery::Shield && Caster->IsShieldUp();
		const bool bNoAtem = Reason == EEwigfelsCastFailReason::NoAtem;

		const FLinearColor Frame = (bActive || bShieldActive) ? Spell->Visual.Color : FLinearColor(0.35f, 0.33f, 0.3f, 0.9f);
		DrawRect(Frame, X - S(2.f), Y - S(2.f), Size + S(4.f), Size + S(4.f));
		DrawRect(Ink, X, Y, Size, Size);
		// Farbkern der Weise
		const FLinearColor Core = bNoAtem ? FLinearColor(0.15f, 0.2f, 0.35f, 0.8f) : FLinearColor(Spell->Visual.Color.R, Spell->Visual.Color.G, Spell->Visual.Color.B, 0.35f);
		DrawRect(Core, X + S(6.f), Y + S(6.f), Size - S(12.f), Size - S(26.f));
		// Abklingzeit als von oben ablaufende Abdeckung
		if (Cd > 0.f && Spell->Cooldown > 0.f)
		{
			const float Frac = FMath::Clamp(Cd / Spell->Cooldown, 0.f, 1.f);
			DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.65f), X, Y, Size, Size * Frac);
			Text(FString::Printf(TEXT("%.1f"), Cd), X + Size * 0.5f, Y + Size * 0.28f, Soft, Medium(), 1.f, true);
		}
		// Drei Grundton-Noten
		for (int32 n = 0; n < Spell->Chord.Num() && n < 3; ++n)
		{
			const FLinearColor NoteColor = EwigfelsSpells::ChordNoteColor(Spell->Chord[n]);
			DrawRect(NoteColor, X + S(10.f) + n * S(18.f), Y + Size - S(16.f), S(12.f), S(8.f));
		}
		Text(SlotPair.Key, X + S(4.f), Y + S(2.f), Gold, Small(), 0.9f);
		if (Spell->Cost > 0.f)
		{
			Text(FString::Printf(TEXT("%.0f"), Spell->Cost), X + Size - S(22.f), Y + S(2.f), bNoAtem ? FLinearColor(1.f, 0.4f, 0.4f) : AtemColor, Small(), 0.9f);
		}
		Text(Spell->Name.ToString(), X + Size * 0.5f, Y + Size + S(4.f), Soft, Small(), 0.85f, true);
		X += Size + Gap;
	}
}

void AEwigfelsHUD::DrawCrosshair(AEwigfelsCharacter* Player)
{
	const float CX = Canvas->ClipX * 0.5f;
	const float CY = Canvas->ClipY * 0.5f;
	const bool bLocked = Player->GetLockOn()->GetTarget() != nullptr;
	if (!bLocked)
	{
		const FLinearColor C(1.f, 1.f, 1.f, 0.75f);
		DrawRect(C, CX - S(1.f), CY - S(1.f), S(3.f), S(3.f));
		const float R = S(10.f);
		DrawRect(C, CX - R - S(6.f), CY, S(6.f), S(1.5f));
		DrawRect(C, CX + R, CY, S(6.f), S(1.5f));
		DrawRect(C, CX, CY - R - S(6.f), S(1.5f), S(6.f));
		DrawRect(C, CX, CY + R, S(1.5f), S(6.f));
	}
}

void AEwigfelsHUD::DrawEnemies(AEwigfelsCharacter* Player)
{
	const AActor* Locked = Player->GetLockOn()->GetTarget();
	const float Now = GetWorld()->GetTimeSeconds();
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
		const bool bRecentlyHit = Now - Enemy->GetLastHitTime() < 4.f;
		if (Dist > 3000.f || (!bIsLocked && !bRecentlyHit && !Enemy->IsTelegraphing() && Dist > 1200.f))
		{
			continue;
		}
		const FVector Screen = Project(Enemy->GetActorLocation() + FVector(0.f, 0.f, 125.f));
		if (Screen.Z <= 0.f)
		{
			continue; // hinter der Kamera
		}
		const float W = S(bIsLocked ? 110.f : 80.f);
		const float Frac = Combat->GetMaxHealth() > 0.f ? Combat->GetHealth() / Combat->GetMaxHealth() : 0.f;
		Bar(Screen.X - W * 0.5f, Screen.Y, W, S(6.f), Frac, FLinearColor(0.9f, 0.3f, 0.3f), FLinearColor(0.f, 0.f, 0.f, 0.6f));
		if (bIsLocked)
		{
			Text(Enemy->GetDisplayName().ToString(), Screen.X, Screen.Y - S(20.f), Soft, Small(), 1.f, true);
			// Lock-On-Marker auf der Brust
			const FVector Chest = Project(Enemy->GetActorLocation() + FVector(0.f, 0.f, 20.f));
			const float R = S(14.f);
			DrawRect(Gold, Chest.X - R, Chest.Y - S(1.5f), S(8.f), S(3.f));
			DrawRect(Gold, Chest.X + R - S(8.f), Chest.Y - S(1.5f), S(8.f), S(3.f));
			DrawRect(Gold, Chest.X - S(1.5f), Chest.Y - R, S(3.f), S(8.f));
			DrawRect(Gold, Chest.X - S(1.5f), Chest.Y + R - S(8.f), S(3.f), S(8.f));
		}
		// Status-Punkte (Brennen, Gefroren …)
		float DotX = Screen.X - W * 0.5f;
		const UGameInstance* GI = GetGameInstance();
		const UEwigfelsSpellSubsystem* Spells = GI ? GI->GetSubsystem<UEwigfelsSpellSubsystem>() : nullptr;
		for (const FEwigfelsActiveStatus& Status : Combat->GetActiveStatuses())
		{
			const FEwigfelsStatusDef* Def = Spells ? Spells->FindStatus(Status.Id) : nullptr;
			DrawRect(Def ? Def->Color : FLinearColor::White, DotX, Screen.Y + S(9.f), S(10.f), S(5.f));
			DotX += S(13.f);
		}
		// Vorwarnung: Ausrufezeichen über dem Kopf
		if (Enemy->IsTelegraphing())
		{
			Text(TEXT("!"), Screen.X, Screen.Y - S(bIsLocked ? 48.f : 34.f), FLinearColor(1.f, 0.85f, 0.4f), Large(), 1.1f, true);
		}
	}
}

void AEwigfelsHUD::DrawFloatingNumbers()
{
	const float Now = GetWorld()->GetRealTimeSeconds();
	Numbers.RemoveAll([Now](const FFloatingNumber& N) { return Now - N.Start > 1.1f; });
	for (const FFloatingNumber& N : Numbers)
	{
		const float Age = Now - N.Start;
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

void AEwigfelsHUD::DrawMessage(AEwigfelsCharacter* Player)
{
	FText Msg;
	FLinearColor Color;
	float Alpha = 0.f;
	if (Player->GetMessage(Msg, Color, Alpha))
	{
		Color.A = Alpha;
		Text(Msg.ToString(), Canvas->ClipX * 0.5f, Canvas->ClipY * 0.32f, Color, Large(), 1.2f, true);
	}
}

void AEwigfelsHUD::DrawInteractPrompt(AEwigfelsCharacter* Player)
{
	AActor* Focus = Player->GetFocusedInteractable();
	if (!Focus || !Focus->Implements<UEwigfelsInteractable>())
	{
		return;
	}
	const FText Prompt = IEwigfelsInteractable::Execute_GetInteractPrompt(Focus);
	const FString Line = FString::Printf(TEXT("[E]  %s"), *Prompt.ToString());
	Text(Line, Canvas->ClipX * 0.5f, Canvas->ClipY * 0.62f, Gold, Medium(), 1.f, true);
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
	Text(Line.Speaker.ToString(), Canvas->ClipX * 0.5f, Y, Gold, Medium(), 1.1f, true);
	Text(Line.Text.ToString(), Canvas->ClipX * 0.5f, Y + S(30.f), FLinearColor::White, Medium(), 1.15f, true);
	Text(LOCTEXT("Continue", "[E] weiter").ToString(), Canvas->ClipX - S(140.f), Canvas->ClipY - Band * 0.6f, FLinearColor(1.f, 1.f, 1.f, 0.6f), Small(), 1.f);
}

void AEwigfelsHUD::DrawHelp(AEwigfelsCharacter* Player)
{
	if (!Player->IsHelpVisible())
	{
		Text(LOCTEXT("HelpHint", "F1: Steuerung").ToString(), S(20.f), S(16.f), FLinearColor(1.f, 1.f, 1.f, 0.5f), Small(), 1.f);
		return;
	}
	static const TCHAR* Lines[] = {
		TEXT("Nocturna – Testarena (Phase 3)"),
		TEXT("WASD  Laufen      Shift  Sprinten      Leertaste  Springen      Alt/C  Ausweichen"),
		TEXT("Linke Maustaste  Funkenstoß (3er-Kette)      Rechte Maustaste halten  Glockenschild"),
		TEXT("Schild genau vor dem Treffer heben = Reinklang (Konter)"),
		TEXT("1 Hammerton   2 Glutweise   3 Frostweise      Tab/Mausrad-Klick  Ziel aufschalten, Mausrad  Ziel wechseln"),
		TEXT("E  Sprechen      F1  Hilfe aus      Konsole: Ewigfels.Arena.Wave"),
		TEXT("Combos: Frost x3 = Gefroren, dann Hammerton (x2,5)  ·  Glut taut Eis"),
	};
	float Y = S(16.f);
	DrawRect(Ink, S(10.f), S(10.f), S(760.f), S(18.f) * static_cast<float>(UE_ARRAY_COUNT(Lines)) + S(14.f));
	bool bFirst = true;
	for (const TCHAR* HelpLine : Lines)
	{
		Text(HelpLine, S(20.f), Y, bFirst ? Gold : Soft, Small(), 1.f);
		bFirst = false;
		Y += S(18.f);
	}
}

#undef LOCTEXT_NAMESPACE
