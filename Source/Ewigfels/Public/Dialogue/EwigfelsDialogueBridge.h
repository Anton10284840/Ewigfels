// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SUDSParticipant.h"
#include "SUDSValue.h"
#include "WorldState/EwigfelsWorldStateTypes.h"
#include "EwigfelsDialogueBridge.generated.h"

class USUDSDialogue;
class USUDSScript;
class UEwigfelsWorldStateSubsystem;
class UEwigfelsQuestSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEwigfelsDialogueCue,
	USUDSDialogue*, Dialogue, FName, Cue, const TArray<FString>&, Arguments);

/**
 * Verbindet SUDS-Dialoge mit Weltstatus und Quests. Wird automatisch als Teilnehmer an jeden Dialog gehängt,
 * der über UEwigfelsDialogueBridge::StartDialogue läuft.
 *
 * In .sud-Skripten (Content/Dialogues/*.sud) gilt:
 *   Lesen:   [if {rel.Lio} >= 30]  [if {flag.E1_TrustEdda} > 0]  [if {rep.Kollegium} >= 20]  [if {quest.M101} == 2]
 *            (alle Werte kommen als Ganzzahl an, darum Flags mit "> 0" prüfen; quest: 0 offen, 1 aktiv, 2 fertig, 3 gescheitert)
 *   Setzen:  [set flag.E1_TrustEdda true]  [set rel.Lio {rel.Lio} + 10]
 *   Events:  [event state `rel:Lio+10`]                 Weltstatus ändern (gleiche Syntax wie im Quest-JSON)
 *            [event quest.start `M102`]                  Quest starten
 *            [event quest.report `talk`, `Edda`]         Fortschritt melden
 *            [event quest.objective `M101`, `o_edda`]    Ziel abhaken
 *            [event quest.stage `M101`, `s2`]            Etappe setzen
 *            [event quest.fail `M101`]
 *            [event emotion `angry`]  [event camera `closeup`]   → OnCue (Mimik, Kamera; Phase 3)
 * Wenn ein Dialog endet, meldet die Brücke automatisch ReportEvent("dialogue", <Skriptname>).
 */
UCLASS(BlueprintType)
class EWIGFELS_API UEwigfelsDialogueBridge : public UObject, public ISUDSParticipant
{
	GENERATED_BODY()

public:
	/**
	 * Startet einen Dialog mit Brücke. Owner = meist der NPC (bestimmt die Lebensdauer).
	 * Weitere Teilnehmer (z. B. NPC-Actor mit ISUDSParticipant für Sprechernamen) optional.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Dialogue", meta = (DefaultToSelf = "Owner"))
	static USUDSDialogue* StartDialogue(UObject* Owner, USUDSScript* Script, const TArray<UObject*>& ExtraParticipants, FName StartLabel = NAME_None);

	/** Emotion, Kamera und alle unbekannten Events. Argumente als Text. */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Dialogue")
	FEwigfelsDialogueCue OnCue;

	void Init(UGameInstance* GameInstance);

	// ISUDSParticipant
	virtual void OnDialogueStarting_Implementation(USUDSDialogue* Dialogue, FName AtLabel) override;
	virtual void OnDialogueFinished_Implementation(USUDSDialogue* Dialogue) override;
	virtual void OnDialogueEvent_Implementation(USUDSDialogue* Dialogue, FName EventName, const TArray<FSUDSValue>& Arguments) override;
	virtual void OnDialogueVariableChanged_Implementation(USUDSDialogue* Dialogue, FName VariableName, const FSUDSValue& Value, bool bFromScript) override;
	virtual void OnDialogueVariableRequested_Implementation(USUDSDialogue* Dialogue, FName VariableName) override;
	virtual int GetDialogueParticipantPriority_Implementation() const override { return -10; } // NPC-Teilnehmer dürfen überschreiben

	/** "rel.Lio" → Bereich + Schlüssel. Gibt false zurück, wenn der Name kein Weltstatus-Name ist. Für Tests öffentlich. */
	static bool ParseVariableName(FName VariableName, bool& bOutIsQuest, EEwigfelsStateDomain& OutDomain, FName& OutKey);

	/** Wert eines SUDS-Arguments als Text (Name, Text, Zahl, Bool). */
	static FString ValueToString(const FSUDSValue& Value);

private:
	UPROPERTY()
	TWeakObjectPtr<UEwigfelsWorldStateSubsystem> WorldState;

	UPROPERTY()
	TWeakObjectPtr<UEwigfelsQuestSubsystem> Quests;

	/** Verhindert, dass unser eigenes SetVariable wieder als Änderung zurückgeschrieben wird. */
	bool bPushingToDialogue = false;
};
