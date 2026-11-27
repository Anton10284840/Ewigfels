// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "UI/EwigfelsSpellbookComponent.h"
#include "Character/EwigfelsCharacter.h"
#include "Magic/EwigfelsSpellCasterComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

#define LOCTEXT_NAMESPACE "EwigfelsSpellbook"

namespace
{
	UInputAction* MakeMenuAction(UObject* Outer, const TCHAR* Name)
	{
		UInputAction* Action = NewObject<UInputAction>(Outer, Name);
		Action->ValueType = EInputActionValueType::Boolean;
		Action->bConsumeInput = true; // Tasten nicht an die Spielsteuerung weiterreichen
		return Action;
	}

	void MapAll(UInputMappingContext* Ctx, UInputAction* Action, std::initializer_list<FKey> Keys)
	{
		for (const FKey& Key : Keys)
		{
			Ctx->MapKey(Action, Key);
		}
	}
}

UEwigfelsSpellbookComponent::UEwigfelsSpellbookComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UEwigfelsSpellbookComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateActions();
}

void UEwigfelsSpellbookComponent::CreateActions()
{
	if (MenuContext)
	{
		return;
	}
	MenuContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Ewigfels_Zauberbuch"));
	ActLeft    = MakeMenuAction(this, TEXT("IA_Book_Left"));
	ActRight   = MakeMenuAction(this, TEXT("IA_Book_Right"));
	ActUp      = MakeMenuAction(this, TEXT("IA_Book_Up"));
	ActDown    = MakeMenuAction(this, TEXT("IA_Book_Down"));
	ActPrev    = MakeMenuAction(this, TEXT("IA_Book_Prev"));
	ActNext    = MakeMenuAction(this, TEXT("IA_Book_Next"));
	ActConfirm = MakeMenuAction(this, TEXT("IA_Book_Confirm"));
	ActBack    = MakeMenuAction(this, TEXT("IA_Book_Back"));
	ActClick   = MakeMenuAction(this, TEXT("IA_Book_Click"));
	ActSwallow = MakeMenuAction(this, TEXT("IA_Book_Swallow"));

	UInputMappingContext* C = MenuContext;
	MapAll(C, ActLeft,  { EKeys::Gamepad_DPad_Left,  EKeys::Gamepad_LeftStick_Left,  EKeys::Left,  EKeys::A });
	MapAll(C, ActRight, { EKeys::Gamepad_DPad_Right, EKeys::Gamepad_LeftStick_Right, EKeys::Right, EKeys::D });
	MapAll(C, ActUp,    { EKeys::Gamepad_DPad_Up,    EKeys::Gamepad_LeftStick_Up,    EKeys::Up,    EKeys::W });
	MapAll(C, ActDown,  { EKeys::Gamepad_DPad_Down,  EKeys::Gamepad_LeftStick_Down,  EKeys::Down,  EKeys::S });
	MapAll(C, ActPrev,  { EKeys::Gamepad_LeftShoulder,  EKeys::MouseScrollUp });
	MapAll(C, ActNext,  { EKeys::Gamepad_RightShoulder, EKeys::MouseScrollDown });
	MapAll(C, ActConfirm, { EKeys::Gamepad_FaceButton_Bottom, EKeys::Enter, EKeys::SpaceBar });
	MapAll(C, ActBack,    { EKeys::Gamepad_FaceButton_Right, EKeys::Escape, EKeys::BackSpace, EKeys::RightMouseButton });
	MapAll(C, ActClick,   { EKeys::LeftMouseButton });
	// Alles andere, was im Spiel etwas tut, schlucken (Zaubern, Ausweichen, Laufen …)
	MapAll(C, ActSwallow, { EKeys::Gamepad_RightTrigger, EKeys::Gamepad_LeftTrigger, EKeys::Gamepad_FaceButton_Left,
		EKeys::Gamepad_FaceButton_Top, EKeys::Gamepad_LeftThumbstick, EKeys::Gamepad_RightThumbstick, EKeys::Gamepad_Left2D,
		EKeys::Gamepad_Right2D, EKeys::Mouse2D, EKeys::MiddleMouseButton, EKeys::MouseWheelAxis, EKeys::E, EKeys::C, EKeys::Q,
		EKeys::F, EKeys::Tab, EKeys::LeftShift, EKeys::LeftAlt, EKeys::Seven, EKeys::Eight });
	const FKey Numbers[] = { EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six };
	for (int32 i = 0; i < UEwigfelsSpellCasterComponent::WheelSlotCount && i < static_cast<int32>(UE_ARRAY_COUNT(Numbers)); ++i)
	{
		UInputAction* Slot = MakeMenuAction(this, *FString::Printf(TEXT("IA_Book_Slot%d"), i + 1));
		C->MapKey(Slot, Numbers[i]);
		ActSlots.Add(Slot);
	}
}

void UEwigfelsSpellbookComponent::BindInput(UEnhancedInputComponent* Input)
{
	if (!Input)
	{
		return;
	}
	CreateActions(); // Eingabe wird oft vor BeginPlay gebunden
	Input->BindAction(ActLeft, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputLeft);
	Input->BindAction(ActRight, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputRight);
	Input->BindAction(ActUp, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputUp);
	Input->BindAction(ActDown, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputDown);
	Input->BindAction(ActPrev, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputPrev);
	Input->BindAction(ActNext, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputNext);
	Input->BindAction(ActConfirm, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputConfirm);
	Input->BindAction(ActBack, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputBack);
	Input->BindAction(ActClick, ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputClick);
	for (int32 i = 0; i < ActSlots.Num(); ++i)
	{
		Input->BindAction(ActSlots[i], ETriggerEvent::Started, this, &UEwigfelsSpellbookComponent::InputSlotKey, i + 1);
	}
}

AEwigfelsCharacter* UEwigfelsSpellbookComponent::GetCharacter() const
{
	return Cast<AEwigfelsCharacter>(GetOwner());
}

UEwigfelsSpellCasterComponent* UEwigfelsSpellbookComponent::GetCaster() const
{
	const AEwigfelsCharacter* Character = GetCharacter();
	return Character ? Character->GetCaster() : nullptr;
}

APlayerController* UEwigfelsSpellbookComponent::GetPC() const
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	return Pawn ? Cast<APlayerController>(Pawn->GetController()) : nullptr;
}

TArray<FName> UEwigfelsSpellbookComponent::GetCards() const
{
	TArray<FName> Cards;
	if (const UEwigfelsSpellCasterComponent* Caster = GetCaster())
	{
		for (const FName Id : Caster->GetKnownSpells())
		{
			if (Id != Caster->BasicSpell && Id != Caster->ShieldSpell && Caster->FindSpellDef(Id))
			{
				Cards.Add(Id);
			}
		}
	}
	return Cards;
}

void UEwigfelsSpellbookComponent::Toggle()
{
	bOpen ? Close() : Open();
}

void UEwigfelsSpellbookComponent::Open()
{
	AEwigfelsCharacter* Character = GetCharacter();
	if (bOpen || !Character || !Character->IsLocallyControlled() || Character->IsGameplayInputBlocked())
	{
		return; // nicht über Dialog oder Charakter-Editor
	}
	bOpen = true;
	FocusRow = 0;
	FocusSlot = FMath::Clamp(Character->GetSelectedSlot(), 1, UEwigfelsSpellCasterComponent::WheelSlotCount);
	FocusCard = 0;
	Character->SetGameplayInputBlocked(true);
	SetMenuMode(true);
	SetComponentTickEnabled(true);
}

void UEwigfelsSpellbookComponent::Close()
{
	if (!bOpen)
	{
		return;
	}
	bOpen = false;
	SetMenuMode(false);
	if (AEwigfelsCharacter* Character = GetCharacter())
	{
		Character->SetGameplayInputBlocked(false);
	}
	SetComponentTickEnabled(false);
}

void UEwigfelsSpellbookComponent::SetMenuMode(bool bMenu)
{
	APlayerController* PC = GetPC();
	if (!PC)
	{
		return;
	}
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		if (bMenu)
		{
			Subsystem->AddMappingContext(MenuContext, 100);
		}
		else
		{
			Subsystem->RemoveMappingContext(MenuContext);
		}
	}
	PC->SetIgnoreMoveInput(bMenu);
	PC->SetIgnoreLookInput(bMenu);
	const AEwigfelsCharacter* Character = GetCharacter();
	PC->SetShowMouseCursor(bMenu && !(Character && Character->IsUsingGamepad()));
	if (bMenu)
	{
		FInputModeGameAndUI Mode;
		Mode.SetHideCursorDuringCapture(false);
		Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(Mode);
	}
	else
	{
		PC->SetInputMode(FInputModeGameOnly());
	}
	LastMouse = FVector2D(-1.f, -1.f);
}

void UEwigfelsSpellbookComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bOpen)
	{
		return;
	}
	const AEwigfelsCharacter* Character = GetCharacter();
	if (APlayerController* PC = GetPC())
	{
		// Mauszeiger nur zeigen, wenn mit Maus gespielt wird
		const bool bWantCursor = !(Character && Character->IsUsingGamepad());
		if (PC->ShouldShowMouseCursor() != bWantCursor)
		{
			PC->SetShowMouseCursor(bWantCursor);
		}
	}
	UpdateHoverFromMouse();
	const int32 NumCards = GetCards().Num();
	FocusCard = NumCards > 0 ? FMath::Clamp(FocusCard, 0, NumCards - 1) : 0;
}

void UEwigfelsSpellbookComponent::SetHitRects(const TArray<FBox2D>& InSlotRects, const TArray<FBox2D>& InCardRects)
{
	SlotRects = InSlotRects;
	CardRects = InCardRects;
}

void UEwigfelsSpellbookComponent::UpdateHoverFromMouse()
{
	APlayerController* PC = GetPC();
	float MX = 0.f, MY = 0.f;
	if (!PC || !PC->GetMousePosition(MX, MY))
	{
		return;
	}
	const FVector2D Mouse(MX, MY);
	if (FVector2D::DistSquared(Mouse, LastMouse) < 1.f)
	{
		return; // Maus ruht: Controller-/Tastenwahl nicht überschreiben
	}
	const bool bFirst = LastMouse.X < 0.f;
	LastMouse = Mouse;
	if (bFirst)
	{
		return;
	}
	for (int32 i = 0; i < SlotRects.Num(); ++i)
	{
		if (SlotRects[i].IsInside(Mouse))
		{
			FocusRow = 0;
			FocusSlot = i + 1;
			return;
		}
	}
	for (int32 i = 0; i < CardRects.Num(); ++i)
	{
		if (CardRects[i].IsInside(Mouse))
		{
			// Karten-Rechtecke kommen in Kartenreihenfolge vom HUD (unsichtbare als leere Box)
			FocusRow = 1;
			FocusCard = i;
			return;
		}
	}
}

void UEwigfelsSpellbookComponent::Move(int32 DX, int32 DY)
{
	if (!bOpen)
	{
		return;
	}
	if (DY != 0)
	{
		FocusRow = FMath::Clamp(FocusRow + DY, 0, 1);
		return;
	}
	if (FocusRow == 0)
	{
		const int32 N = UEwigfelsSpellCasterComponent::WheelSlotCount;
		FocusSlot = ((FocusSlot - 1 + DX) % N + N) % N + 1;
	}
	else
	{
		const int32 NumCards = GetCards().Num();
		if (NumCards > 0)
		{
			FocusCard = (FocusCard + DX + NumCards) % NumCards;
		}
	}
}

void UEwigfelsSpellbookComponent::InputConfirm()
{
	if (!bOpen)
	{
		return;
	}
	if (FocusRow == 0)
	{
		// Platz gewählt: zur Karte springen, die dort liegt
		FocusRow = 1;
		if (const UEwigfelsSpellCasterComponent* Caster = GetCaster())
		{
			const int32 Index = GetCards().IndexOfByKey(Caster->GetSlotSpell(FocusSlot));
			if (Index != INDEX_NONE)
			{
				FocusCard = Index;
			}
		}
		return;
	}
	Assign(FocusSlot, FocusCard);
	FocusRow = 0;
}

void UEwigfelsSpellbookComponent::InputBack()
{
	if (!bOpen)
	{
		return;
	}
	if (FocusRow == 1)
	{
		FocusRow = 0;
		return;
	}
	Close();
}

void UEwigfelsSpellbookComponent::InputClick()
{
	APlayerController* PC = GetPC();
	float MX = 0.f, MY = 0.f;
	if (!bOpen || !PC || !PC->GetMousePosition(MX, MY))
	{
		return;
	}
	const FVector2D Mouse(MX, MY);
	for (int32 i = 0; i < SlotRects.Num(); ++i)
	{
		if (SlotRects[i].IsInside(Mouse))
		{
			FocusSlot = i + 1;
			FocusRow = 1;
			return;
		}
	}
	for (int32 i = 0; i < CardRects.Num(); ++i)
	{
		if (CardRects[i].IsInside(Mouse))
		{
			// Das HUD legt die Karten-Rechtecke in Kartenreihenfolge ab (alle Karten, unsichtbare als leere Box)
			FocusCard = i;
			Assign(FocusSlot, i);
			FocusRow = 0;
			return;
		}
	}
}

void UEwigfelsSpellbookComponent::InputSlotKey(int32 Slot)
{
	if (!bOpen)
	{
		return;
	}
	if (FocusRow == 1)
	{
		Assign(Slot, FocusCard);
		FocusSlot = Slot;
		return;
	}
	FocusSlot = Slot;
	InputConfirm();
}

void UEwigfelsSpellbookComponent::Assign(int32 Slot, int32 CardIndex)
{
	const TArray<FName> Cards = GetCards();
	UEwigfelsSpellCasterComponent* Caster = GetCaster();
	if (!Caster || !Cards.IsValidIndex(CardIndex))
	{
		return;
	}
	Caster->RequestSetWheelSlot(Slot, Cards[CardIndex]);
	FlashSlot = Slot;
	FlashTime = GetWorld() ? GetWorld()->GetRealTimeSeconds() : 0.f;
}

#undef LOCTEXT_NAMESPACE
