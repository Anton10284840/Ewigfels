// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Quest/EwigfelsQuestSubsystem.h"
#include "WorldState/EwigfelsWorldStateSubsystem.h"
#include "Ewigfels.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace
{
	/** Schutz gegen Endlosschleifen (Etappen ohne Ziele, die sich im Kreis verweisen). */
	constexpr int32 MaxReentryDepth = 16;

	FString QuestDataDir()
	{
		return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), TEXT("Quests"));
	}

	/** Hält ReentryDepth hoch, solange der Scope lebt. */
	struct FReentryScope
	{
		int32& Depth;
		explicit FReentryScope(int32& InDepth) : Depth(InDepth) { ++Depth; }
		~FReentryScope() { --Depth; }
	};
}

void UEwigfelsQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	WorldState = Collection.InitializeDependency<UEwigfelsWorldStateSubsystem>();
	if (WorldState)
	{
		WorldState->OnStateChanged.AddDynamic(this, &UEwigfelsQuestSubsystem::HandleWorldStateChanged);
	}
	ReloadDefinitions();
}

void UEwigfelsQuestSubsystem::Deinitialize()
{
	if (WorldState)
	{
		WorldState->OnStateChanged.RemoveDynamic(this, &UEwigfelsQuestSubsystem::HandleWorldStateChanged);
	}
	Super::Deinitialize();
}

// ---------------------------------------------------------------------------
// Definitionen laden

int32 UEwigfelsQuestSubsystem::ReloadDefinitions()
{
	Definitions.Reset();
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *QuestDataDir(), TEXT("*.json"), true, false);
	Files.Sort();

	int32 ErrorCount = 0;
	for (const FString& File : Files)
	{
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *File))
		{
			UE_LOG(LogEwigfels, Error, TEXT("Quest: Datei nicht lesbar: %s"), *File);
			++ErrorCount;
			continue;
		}
		TArray<FString> Errors;
		AddDefinitionFromJson(Text, FPaths::GetCleanFilename(File), Errors);
		for (const FString& Error : Errors)
		{
			UE_LOG(LogEwigfels, Error, TEXT("Quest: %s"), *Error);
		}
		ErrorCount += Errors.Num();
	}
	UE_LOG(LogEwigfels, Log, TEXT("Quest: %d Quests geladen aus %s (%d Fehler)"), Definitions.Num(), *QuestDataDir(), ErrorCount);
	return ErrorCount;
}

bool UEwigfelsQuestSubsystem::AddDefinitionFromJson(const FString& JsonText, const FString& SourceName, TArray<FString>& OutErrors)
{
	FEwigfelsQuestDef Quest;
	if (!EwigfelsQuest::ParseQuestJson(JsonText, SourceName, Quest, OutErrors))
	{
		return false; // fehlerhafte Quests werden gar nicht erst geladen
	}
	if (Definitions.Contains(Quest.Id))
	{
		OutErrors.Add(FString::Printf(TEXT("%s: Quest-ID '%s' gibt es doppelt"), *SourceName, *Quest.Id.ToString()));
		return false;
	}
	Definitions.Add(Quest.Id, MoveTemp(Quest));
	return true;
}

// ---------------------------------------------------------------------------
// Aktionen

bool UEwigfelsQuestSubsystem::StartQuest(FName QuestId, bool bForce)
{
	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	if (!Quest)
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Quest: StartQuest('%s') - unbekannte Quest"), *QuestId.ToString());
		return false;
	}
	if (GetQuestStatus(QuestId) != EEwigfelsQuestStatus::NotStarted)
	{
		return false;
	}
	if (!bForce && !CanStartQuest(QuestId))
	{
		return false;
	}

	FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	if (!Progress)
	{
		Progress = &SaveData.Quests.AddDefaulted_GetRef();
		Progress->QuestId = QuestId;
	}
	Progress->Status = EEwigfelsQuestStatus::Active;
	Progress->StartOrder = SaveData.NextStartOrder++;
	if (SaveData.TrackedQuest.IsNone())
	{
		SaveData.TrackedQuest = QuestId;
	}

	UE_LOG(LogEwigfels, Log, TEXT("Quest: gestartet %s"), *QuestId.ToString());
	OnQuestEvent.Broadcast(QuestId, EEwigfelsQuestEvent::Started, Quest->StartStage);
	SetStatusFlag(QuestId, EEwigfelsQuestStatus::Active);

	// Zeiger neu holen: SetStatusFlag kann über Events weitere Quests starten (Array wächst).
	if (FEwigfelsQuestProgress* Fresh = FindProgress(QuestId))
	{
		if (Fresh->Status == EEwigfelsQuestStatus::Active && Fresh->StageId.IsNone())
		{
			EnterStage(*Quest, Quest->StartStage);
		}
	}
	return true;
}

int32 UEwigfelsQuestSubsystem::ReportEvent(FName Kind, const FString& Target, int32 Amount)
{
	int32 Updated = 0;
	TArray<FName> ToAdvance;
	TArray<TPair<FName, FName>> Events; // erst nach der Schleife senden (Zuhörer dürfen Quests starten)
	for (FEwigfelsQuestProgress& Progress : SaveData.Quests)
	{
		if (Progress.Status != EEwigfelsQuestStatus::Active)
		{
			continue;
		}
		const FEwigfelsQuestDef* Quest = Definitions.Find(Progress.QuestId);
		const FEwigfelsStageDef* Stage = Quest ? Quest->FindStage(Progress.StageId) : nullptr;
		if (!Stage)
		{
			continue;
		}
		for (const FEwigfelsObjectiveDef& Objective : Stage->Objectives)
		{
			if (Objective.Kind != Kind || !Objective.Target.Equals(Target, ESearchCase::IgnoreCase))
			{
				continue;
			}
			int32& Count = Progress.ObjectiveCounts.FindOrAdd(Objective.Id);
			if (Count >= Objective.Count)
			{
				continue;
			}
			Count = FMath::Min(Objective.Count, Count + FMath::Max(1, Amount));
			++Updated;
			Events.Emplace(Progress.QuestId, Objective.Id);
			ToAdvance.AddUnique(Progress.QuestId);
		}
	}
	for (const TPair<FName, FName>& Event : Events)
	{
		OnQuestEvent.Broadcast(Event.Key, EEwigfelsQuestEvent::ObjectiveUpdated, Event.Value);
	}
	for (const FName& QuestId : ToAdvance)
	{
		TryAdvance(QuestId);
	}
	return Updated;
}

bool UEwigfelsQuestSubsystem::CompleteObjective(FName QuestId, FName ObjectiveId)
{
	FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	if (!Progress || !Quest || Progress->Status != EEwigfelsQuestStatus::Active)
	{
		return false;
	}
	const FEwigfelsStageDef* Stage = Quest->FindStage(Progress->StageId);
	const FEwigfelsObjectiveDef* Objective = Stage
		? Stage->Objectives.FindByPredicate([ObjectiveId](const FEwigfelsObjectiveDef& O) { return O.Id == ObjectiveId; })
		: nullptr;
	if (!Objective)
	{
		UE_LOG(LogEwigfels, Warning, TEXT("Quest: Ziel '%s' ist in %s gerade nicht aktiv"), *ObjectiveId.ToString(), *QuestId.ToString());
		return false;
	}
	Progress->ObjectiveCounts.Add(ObjectiveId, Objective->Count);
	OnQuestEvent.Broadcast(QuestId, EEwigfelsQuestEvent::ObjectiveUpdated, ObjectiveId);
	TryAdvance(QuestId);
	return true;
}

bool UEwigfelsQuestSubsystem::SetStage(FName QuestId, FName StageId)
{
	FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	if (!Progress || !Quest || Progress->Status != EEwigfelsQuestStatus::Active || !Quest->FindStage(StageId))
	{
		return false;
	}
	EnterStage(*Quest, StageId);
	return true;
}

void UEwigfelsQuestSubsystem::FailQuest(FName QuestId)
{
	FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	if (Progress && Quest && Progress->Status == EEwigfelsQuestStatus::Active)
	{
		FinishQuest(*Quest, *Progress, EEwigfelsQuestStatus::Failed);
	}
}

void UEwigfelsQuestSubsystem::SetTrackedQuest(FName QuestId)
{
	if (QuestId.IsNone() || GetQuestStatus(QuestId) == EEwigfelsQuestStatus::Active)
	{
		SaveData.TrackedQuest = QuestId;
	}
}

// ---------------------------------------------------------------------------
// Ablauf

void UEwigfelsQuestSubsystem::EnterStage(const FEwigfelsQuestDef& Quest, FName StageId)
{
	if (ReentryDepth > MaxReentryDepth)
	{
		UE_LOG(LogEwigfels, Error, TEXT("Quest: %s haengt in einer Schleife (Etappe %s). Daten pruefen!"), *Quest.Id.ToString(), *StageId.ToString());
		return;
	}
	FReentryScope Scope(ReentryDepth);

	if (FEwigfelsQuestProgress* Progress = FindProgress(Quest.Id))
	{
		Progress->StageId = StageId;
		Progress->ObjectiveCounts.Reset();
	}
	OnQuestEvent.Broadcast(Quest.Id, EEwigfelsQuestEvent::StageChanged, StageId);
	UpdateConditionObjectives(Quest);
	TryAdvance(Quest.Id); // Etappen ohne offene Ziele (reine Verzweigungen) sofort weiterschalten
}

void UEwigfelsQuestSubsystem::TryAdvance(FName QuestId)
{
	if (ReentryDepth > MaxReentryDepth)
	{
		return;
	}
	FReentryScope Scope(ReentryDepth);

	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	if (!Quest || !Progress || Progress->Status != EEwigfelsQuestStatus::Active)
	{
		return;
	}
	const FEwigfelsStageDef* Stage = Quest->FindStage(Progress->StageId);
	if (!Stage || !IsStageDone(*Stage, *Progress))
	{
		return;
	}

	// Etappe als „wird gerade verlassen“ markieren, damit Effekt-Events nicht doppelt weiterschalten.
	const FName LeavingStage = Progress->StageId;
	Progress->StageId = NAME_None;

	if (WorldState)
	{
		WorldState->ApplyEffects(Stage->OnComplete);
	}

	Progress = FindProgress(QuestId); // Array kann durch Effekte gewachsen sein
	if (!Progress || Progress->Status != EEwigfelsQuestStatus::Active)
	{
		return;
	}

	for (const FEwigfelsStageTransition& Transition : Stage->Next)
	{
		if (WorldState && !WorldState->CheckAllConditions(Transition.Conditions))
		{
			continue;
		}
		if (!Transition.NextStage.IsNone())
		{
			EnterStage(*Quest, Transition.NextStage);
		}
		else
		{
			FinishQuest(*Quest, *Progress, Transition.EndStatus);
		}
		return;
	}

	UE_LOG(LogEwigfels, Warning, TEXT("Quest: %s Etappe %s ist fertig, aber keine 'next'-Regel passt. Quest bleibt stehen."),
		*QuestId.ToString(), *LeavingStage.ToString());
	Progress->StageId = LeavingStage;
}

void UEwigfelsQuestSubsystem::FinishQuest(const FEwigfelsQuestDef& Quest, FEwigfelsQuestProgress& Progress, EEwigfelsQuestStatus EndStatus)
{
	Progress.Status = EndStatus;
	Progress.ObjectiveCounts.Reset();
	const FName QuestId = Quest.Id;

	if (SaveData.TrackedQuest == QuestId)
	{
		const TArray<FName> Active = GetQuestsWithStatus(EEwigfelsQuestStatus::Active);
		SaveData.TrackedQuest = Active.Num() > 0 ? Active[0] : NAME_None;
	}

	UE_LOG(LogEwigfels, Log, TEXT("Quest: %s %s"), *QuestId.ToString(),
		EndStatus == EEwigfelsQuestStatus::Completed ? TEXT("abgeschlossen") : TEXT("gescheitert"));

	if (EndStatus == EEwigfelsQuestStatus::Completed)
	{
		if (WorldState)
		{
			WorldState->ApplyEffects(Quest.Rewards.Effects);
		}
		OnQuestRewarded.Broadcast(QuestId, Quest.Rewards);
	}
	OnQuestEvent.Broadcast(QuestId,
		EndStatus == EEwigfelsQuestStatus::Completed ? EEwigfelsQuestEvent::Completed : EEwigfelsQuestEvent::Failed, NAME_None);

	// Zuletzt: Status-Flag setzen. Das löst Bedingungen "quest:ID" aus und startet ggf. Folgequests.
	SetStatusFlag(QuestId, EndStatus);
}

bool UEwigfelsQuestSubsystem::IsStageDone(const FEwigfelsStageDef& Stage, const FEwigfelsQuestProgress& Progress) const
{
	for (const FEwigfelsObjectiveDef& Objective : Stage.Objectives)
	{
		if (Objective.bOptional)
		{
			continue;
		}
		const int32* Count = Progress.ObjectiveCounts.Find(Objective.Id);
		if (!Count || *Count < Objective.Count)
		{
			return false;
		}
	}
	return true;
}

bool UEwigfelsQuestSubsystem::UpdateConditionObjectives(const FEwigfelsQuestDef& Quest)
{
	FEwigfelsQuestProgress* Progress = FindProgress(Quest.Id);
	const FEwigfelsStageDef* Stage = Progress ? Quest.FindStage(Progress->StageId) : nullptr;
	if (!Stage || !WorldState)
	{
		return false;
	}
	static const FName ConditionKind(TEXT("condition"));
	TArray<FName> Changed;
	for (const FEwigfelsObjectiveDef& Objective : Stage->Objectives)
	{
		if (Objective.Kind != ConditionKind)
		{
			continue;
		}
		int32& Count = Progress->ObjectiveCounts.FindOrAdd(Objective.Id);
		const int32 Wanted = WorldState->CheckCondition(Objective.Condition) ? Objective.Count : 0;
		if (Count != Wanted)
		{
			Count = Wanted;
			Changed.Add(Objective.Id);
		}
	}
	for (const FName& ObjectiveId : Changed)
	{
		OnQuestEvent.Broadcast(Quest.Id, EEwigfelsQuestEvent::ObjectiveUpdated, ObjectiveId);
	}
	return Changed.Num() > 0;
}

void UEwigfelsQuestSubsystem::SetStatusFlag(FName QuestId, EEwigfelsQuestStatus Status)
{
	if (WorldState)
	{
		WorldState->SetValue(EEwigfelsStateDomain::Flag, EwigfelsQuest::StatusFlag(QuestId), static_cast<int32>(Status));
	}
}

void UEwigfelsQuestSubsystem::CheckAutoStart()
{
	TArray<FName> ToStart;
	for (const TPair<FName, FEwigfelsQuestDef>& Pair : Definitions)
	{
		if (Pair.Value.bAutoStart && GetQuestStatus(Pair.Key) == EEwigfelsQuestStatus::NotStarted && CanStartQuest(Pair.Key))
		{
			ToStart.Add(Pair.Key);
		}
	}
	for (const FName& QuestId : ToStart)
	{
		StartQuest(QuestId);
	}
}

void UEwigfelsQuestSubsystem::HandleWorldStateChanged(EEwigfelsStateDomain Domain, FName Key, int32 OldValue, int32 NewValue)
{
	if (ReentryDepth > MaxReentryDepth)
	{
		return;
	}
	FReentryScope Scope(ReentryDepth);

	// IDs zuerst kopieren: Zuhörer können während der Schleife Quests starten (Array wächst).
	for (const FName& QuestId : GetQuestsWithStatus(EEwigfelsQuestStatus::Active))
	{
		const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
		if (Quest && UpdateConditionObjectives(*Quest))
		{
			TryAdvance(QuestId);
		}
	}
	CheckAutoStart();
}

// ---------------------------------------------------------------------------
// Abfragen

FEwigfelsQuestProgress* UEwigfelsQuestSubsystem::FindProgress(FName QuestId)
{
	return SaveData.Quests.FindByPredicate([QuestId](const FEwigfelsQuestProgress& P) { return P.QuestId == QuestId; });
}

const FEwigfelsQuestProgress* UEwigfelsQuestSubsystem::FindProgress(FName QuestId) const
{
	return SaveData.Quests.FindByPredicate([QuestId](const FEwigfelsQuestProgress& P) { return P.QuestId == QuestId; });
}

EEwigfelsQuestStatus UEwigfelsQuestSubsystem::GetQuestStatus(FName QuestId) const
{
	const FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	return Progress ? Progress->Status : EEwigfelsQuestStatus::NotStarted;
}

bool UEwigfelsQuestSubsystem::CanStartQuest(FName QuestId) const
{
	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	return Quest && GetQuestStatus(QuestId) == EEwigfelsQuestStatus::NotStarted
		&& (!WorldState || WorldState->CheckAllConditions(Quest->Requires));
}

bool UEwigfelsQuestSubsystem::GetQuestDefinition(FName QuestId, FEwigfelsQuestDef& OutQuest) const
{
	if (const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId))
	{
		OutQuest = *Quest;
		return true;
	}
	return false;
}

TArray<FName> UEwigfelsQuestSubsystem::GetQuestsWithStatus(EEwigfelsQuestStatus Status) const
{
	TArray<const FEwigfelsQuestProgress*> Matching;
	for (const FEwigfelsQuestProgress& Progress : SaveData.Quests)
	{
		if (Progress.Status == Status)
		{
			Matching.Add(&Progress);
		}
	}
	Matching.Sort([](const FEwigfelsQuestProgress& A, const FEwigfelsQuestProgress& B) { return A.StartOrder < B.StartOrder; });
	TArray<FName> Out;
	for (const FEwigfelsQuestProgress* Progress : Matching)
	{
		Out.Add(Progress->QuestId);
	}
	return Out;
}

TArray<FEwigfelsObjectiveView> UEwigfelsQuestSubsystem::GetCurrentObjectives(FName QuestId) const
{
	TArray<FEwigfelsObjectiveView> Out;
	const FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	const FEwigfelsStageDef* Stage = (Progress && Quest) ? Quest->FindStage(Progress->StageId) : nullptr;
	if (!Stage)
	{
		return Out;
	}
	for (const FEwigfelsObjectiveDef& Objective : Stage->Objectives)
	{
		FEwigfelsObjectiveView& View = Out.AddDefaulted_GetRef();
		View.Id = Objective.Id;
		View.Text = Objective.Text;
		View.Required = Objective.Count;
		View.Current = Progress->ObjectiveCounts.FindRef(Objective.Id);
		View.bDone = View.Current >= View.Required;
		View.bOptional = Objective.bOptional;
		View.Marker = Objective.Marker;
	}
	return Out;
}

FText UEwigfelsQuestSubsystem::GetCurrentStageText(FName QuestId) const
{
	const FEwigfelsQuestProgress* Progress = FindProgress(QuestId);
	const FEwigfelsQuestDef* Quest = Definitions.Find(QuestId);
	const FEwigfelsStageDef* Stage = (Progress && Quest) ? Quest->FindStage(Progress->StageId) : nullptr;
	return Stage ? Stage->Text : FText::GetEmpty();
}

TArray<FName> UEwigfelsQuestSubsystem::GetTrackedMarkers() const
{
	TArray<FName> Markers;
	for (const FEwigfelsObjectiveView& View : GetCurrentObjectives(SaveData.TrackedQuest))
	{
		if (!View.bDone && !View.Marker.IsNone())
		{
			Markers.AddUnique(View.Marker);
		}
	}
	return Markers;
}

// ---------------------------------------------------------------------------
// Speichern/Laden

void UEwigfelsQuestSubsystem::LoadSaveData(const FEwigfelsQuestSaveData& InData)
{
	SaveData = InData;
	// Quests, die es in den Daten nicht mehr gibt (umbenannt/gelöscht), nur melden, nicht löschen.
	for (const FEwigfelsQuestProgress& Progress : SaveData.Quests)
	{
		if (!Definitions.Contains(Progress.QuestId))
		{
			UE_LOG(LogEwigfels, Warning, TEXT("Quest: Spielstand enthaelt unbekannte Quest '%s'"), *Progress.QuestId.ToString());
		}
	}
}

void UEwigfelsQuestSubsystem::ResetProgress()
{
	SaveData = FEwigfelsQuestSaveData();
}

FString UEwigfelsQuestSubsystem::DescribeState() const
{
	static const TCHAR* StatusNames[] = { TEXT("offen"), TEXT("aktiv"), TEXT("fertig"), TEXT("gescheitert") };
	FString Out = FString::Printf(TEXT("%d Quests geladen, verfolgt: %s\n"), Definitions.Num(), *SaveData.TrackedQuest.ToString());
	TArray<FName> Ids;
	Definitions.GetKeys(Ids);
	Ids.Sort(FNameLexicalLess());
	for (const FName& Id : Ids)
	{
		const FEwigfelsQuestProgress* Progress = FindProgress(Id);
		const int32 StatusIndex = static_cast<int32>(GetQuestStatus(Id));
		Out += FString::Printf(TEXT("  %-8s %-11s %s\n"), *Id.ToString(), StatusNames[StatusIndex],
			Progress ? *Progress->StageId.ToString() : TEXT(""));
	}
	return Out;
}

// ---------------------------------------------------------------------------
// Konsolenbefehle

namespace
{
	UEwigfelsQuestSubsystem* FindQuests(UWorld* World)
	{
		UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
		return GameInstance ? GameInstance->GetSubsystem<UEwigfelsQuestSubsystem>() : nullptr;
	}

	FAutoConsoleCommandWithWorldAndArgs GQuestList(TEXT("Ewigfels.Quest.List"), TEXT("Alle Quests mit Status"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (UEwigfelsQuestSubsystem* Quests = FindQuests(World))
			{
				UE_LOG(LogEwigfels, Display, TEXT("%s"), *Quests->DescribeState());
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GQuestStart(TEXT("Ewigfels.Quest.Start"), TEXT("Quest starten (ohne Voraussetzungen): Ewigfels.Quest.Start M101"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			UEwigfelsQuestSubsystem* Quests = FindQuests(World);
			if (Quests && Args.Num() >= 1)
			{
				Quests->StartQuest(FName(*Args[0]), true);
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GQuestReport(TEXT("Ewigfels.Quest.Report"), TEXT("Ereignis melden: Ewigfels.Quest.Report talk Edda"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			UEwigfelsQuestSubsystem* Quests = FindQuests(World);
			if (Quests && Args.Num() >= 2)
			{
				const int32 Amount = Args.Num() >= 3 ? FCString::Atoi(*Args[2]) : 1;
				UE_LOG(LogEwigfels, Display, TEXT("%d Ziele aktualisiert"), Quests->ReportEvent(FName(*Args[0]), Args[1], Amount));
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GQuestReload(TEXT("Ewigfels.Quest.Reload"), TEXT("Quest-JSONs neu einlesen"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>&, UWorld* World)
		{
			if (UEwigfelsQuestSubsystem* Quests = FindQuests(World))
			{
				Quests->ReloadDefinitions();
			}
		}));
}
