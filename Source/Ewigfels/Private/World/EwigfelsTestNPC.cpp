// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "World/EwigfelsTestNPC.h"
#include "Camera/EwigfelsDialogueCameraComponent.h"
#include "Character/EwigfelsCharacter.h"
#include "Character/EwigfelsPlaceholderBody.h"
#include "Combat/EwigfelsCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#define LOCTEXT_NAMESPACE "EwigfelsTestNPC"

AEwigfelsTestNPC::AEwigfelsTestNPC()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(36.f, 88.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	DisplayName = LOCTEXT("Lio", "Lio");

	Combat = CreateDefaultSubobject<UEwigfelsCombatComponent>(TEXT("Combat"));
	Combat->SetTeam(EEwigfelsTeam::Neutral);

	PlaceholderBody = CreateDefaultSubobject<UEwigfelsPlaceholderBody>(TEXT("PlaceholderBody"));
	PlaceholderBody->SetupAttachment(RootComponent);
	// Lio: Turm-Farben grünlich-warm, kein Kapuzenträger (CHARACTER_BIBLE: offen, neugierig)
	PlaceholderBody->SetLook(FLinearColor(0.08f, 0.16f, 0.1f), FLinearColor(0.55f, 1.f, 0.6f), FLinearColor(0.85f, 1.f, 0.85f), 0.96f, false);
}

FText AEwigfelsTestNPC::GetInteractPrompt_Implementation() const
{
	return FText::Format(LOCTEXT("Talk", "Mit {0} sprechen"), DisplayName);
}

void AEwigfelsTestNPC::Interact_Implementation(APawn* InstigatorPawn)
{
	AEwigfelsCharacter* Player = Cast<AEwigfelsCharacter>(InstigatorPawn);
	if (!Player || !Player->GetDialogueCamera())
	{
		return;
	}
	TalkingTo = Player;
	const FText Me = LOCTEXT("Me", "Du");
	auto L = [](const FText& Speaker, const FText& Text, bool bPartner, float Intensity, FName Emotion = NAME_None)
	{
		FEwigfelsDialogueLine Line;
		Line.Speaker = Speaker;
		Line.Text = Text;
		Line.bPartnerSpeaks = bPartner;
		Line.Intensity = Intensity;
		Line.Emotion = Emotion;
		return Line;
	};
	TArray<FEwigfelsDialogueLine> Lines;
	Lines.Add(L(DisplayName, LOCTEXT("L1", "Da bist du ja! Ich dachte schon, die Puppen hätten dich erwischt."), true, 0.4f, TEXT("happy")));
	Lines.Add(L(Me, LOCTEXT("L2", "Die Puppen? Die schlagen ja nicht mal zurück."), false, 0.3f));
	Lines.Add(L(DisplayName, LOCTEXT("L3", "Die nicht. Aber hinten am Tor warten Novizen der Stillen Hand. Und ein Fluchsänger."), true, 0.5f, TEXT("worried")));
	Lines.Add(L(DisplayName, LOCTEXT("L4", "Wenn er singt, wird dir kalt bis in die Fingerspitzen. Halt den Glockenschild bereit."), true, 0.85f, TEXT("scared")));
	Lines.Add(L(Me, LOCTEXT("L5", "Und wenn ich den Schild genau im richtigen Moment hebe?"), false, 0.4f));
	Lines.Add(L(DisplayName, LOCTEXT("L6", "Dann klingt er rein. Und dann gehört der Takt dir."), true, 0.8f, TEXT("happy")));
	Player->GetDialogueCamera()->BeginDialogue(this, Lines);
}

void AEwigfelsTestNPC::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Im Gespräch zum Spieler drehen
	const APawn* Other = TalkingTo.Get();
	const AEwigfelsCharacter* Player = Cast<AEwigfelsCharacter>(Other);
	if (Player && Player->GetDialogueCamera() && Player->GetDialogueCamera()->IsDialogueActive())
	{
		const FRotator Want(0.f, (Other->GetActorLocation() - GetActorLocation()).Rotation().Yaw, 0.f);
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), Want, DeltaSeconds, 6.f));
	}
}

#undef LOCTEXT_NAMESPACE
