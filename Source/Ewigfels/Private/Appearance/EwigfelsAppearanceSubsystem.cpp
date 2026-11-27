// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Appearance/EwigfelsAppearanceSubsystem.h"
#include "Appearance/EwigfelsAppearanceComponent.h"
#include "Ewigfels.h"
#include "UI/EwigfelsCharacterCreator.h"
#include "WorldState/EwigfelsWorldStateSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "SUDSSubsystem.h"

const TCHAR* UEwigfelsAppearanceSubsystem::ProfileSlotName = TEXT("EwigfelsLook");

FString UEwigfelsAppearanceSubsystem::AppearanceDataDir()
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Data"), TEXT("Appearance"));
}

void UEwigfelsAppearanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Collection.InitializeDependency<UEwigfelsWorldStateSubsystem>();
	Super::Initialize(Collection);
	ReloadCatalog();
}

int32 UEwigfelsAppearanceSubsystem::ReloadCatalog()
{
	Catalog = FEwigfelsLookCatalog();
	LoadErrors.Reset();

	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *AppearanceDataDir(), TEXT("*.json"), true, false);
	// Appearance.json zuerst (Slots), dann der Rest (Läden, Mods)
	Files.Sort([](const FString& A, const FString& B)
	{
		const bool bA = FPaths::GetCleanFilename(A) == TEXT("Appearance.json");
		const bool bB = FPaths::GetCleanFilename(B) == TEXT("Appearance.json");
		return bA != bB ? bA : A < B;
	});
	for (const FString& File : Files)
	{
		FString Text;
		if (!FFileHelper::LoadFileToString(Text, *File))
		{
			LoadErrors.Add(FString::Printf(TEXT("%s: nicht lesbar"), *File));
			continue;
		}
		AddCatalogFromJson(Text, FPaths::GetCleanFilename(File));
	}

	int32 Count = 0;
	for (const auto& Pair : Catalog.Options)
	{
		Count += Pair.Value.Num();
	}
	for (const FString& Error : LoadErrors)
	{
		UE_LOG(LogEwigfels, Error, TEXT("Look: %s"), *Error);
	}
	UE_LOG(LogEwigfels, Log, TEXT("Look: %d Optionen, %d Regler, %d Laeden aus %d Dateien"), Count, Catalog.Sliders.Num(), Catalog.Shops.Num(), Files.Num());
	return Count;
}

bool UEwigfelsAppearanceSubsystem::AddCatalogFromJson(const FString& JsonText, const FString& SourceName)
{
	const bool bOk = EwigfelsLook::ParseCatalogJson(JsonText, SourceName, Catalog, LoadErrors);
	// Standard-Aussehen gegen den (evtl. gerade ergänzten) Katalog prüfen
	TArray<FString> Fixes;
	FEwigfelsAppearance Defaults = Catalog.Defaults;
	if (Catalog.OutfitSlots.Num() > 0 && EwigfelsLook::Sanitize(Defaults, Catalog, &Fixes) > 0 && SourceName == TEXT("Appearance.json"))
	{
		for (const FString& Fix : Fixes)
		{
			LoadErrors.Add(FString::Printf(TEXT("%s/defaults: %s"), *SourceName, *Fix));
		}
	}
	Catalog.Defaults = Defaults;
	return bOk;
}

FEwigfelsLookContext UEwigfelsAppearanceSubsystem::MakeContext(FName ShopId) const
{
	FEwigfelsLookContext Context;
	Context.ShopId = ShopId;
	TWeakObjectPtr<const UEwigfelsWorldStateSubsystem> WorldState = GetGameInstance()->GetSubsystem<UEwigfelsWorldStateSubsystem>();
	Context.HasFlag = [WorldState](FName Flag)
	{
		return WorldState.IsValid() && WorldState->HasFlag(Flag);
	};
	return Context;
}

FEwigfelsAppearance UEwigfelsAppearanceSubsystem::GetStartingLook() const
{
	FEwigfelsAppearance Look;
	if (!LoadProfile(Look))
	{
		Look = Catalog.Defaults;
	}
	EwigfelsLook::Sanitize(Look, Catalog);
	Look.bCreated = false;
	return Look;
}

bool UEwigfelsAppearanceSubsystem::LoadProfile(FEwigfelsAppearance& OutLook) const
{
	if (!UGameplayStatics::DoesSaveGameExist(ProfileSlotName, 0))
	{
		return false;
	}
	const UEwigfelsLookProfile* Profile = Cast<UEwigfelsLookProfile>(UGameplayStatics::LoadGameFromSlot(ProfileSlotName, 0));
	if (!Profile)
	{
		return false;
	}
	OutLook = Profile->Look;
	return true;
}

void UEwigfelsAppearanceSubsystem::SaveProfile(const FEwigfelsAppearance& Look)
{
	UEwigfelsLookProfile* Profile = Cast<UEwigfelsLookProfile>(UGameplayStatics::CreateSaveGameObject(UEwigfelsLookProfile::StaticClass()));
	if (Profile)
	{
		Profile->Look = Look;
		UGameplayStatics::SaveGameToSlot(Profile, ProfileSlotName, 0);
	}
}

void UEwigfelsAppearanceSubsystem::DeleteProfile()
{
	UGameplayStatics::DeleteGameInSlot(ProfileSlotName, 0);
}

void UEwigfelsAppearanceSubsystem::PushDialogueVariables(const FEwigfelsAppearance& Look)
{
	USUDSSubsystem* Suds = GetGameInstance()->GetSubsystem<USUDSSubsystem>();
	if (!Suds)
	{
		return;
	}
	// In Dialogen: {global.SpielerName}; Textformen über {global.SpielerGeschlecht}|gender(er,sie,es)
	Suds->SetGlobalVariableText(TEXT("SpielerName"), FText::FromString(Look.Name));
	const ETextGender Gender = Look.Pronouns == EEwigfelsPronouns::Er ? ETextGender::Masculine
		: Look.Pronouns == EEwigfelsPronouns::Sie ? ETextGender::Feminine : ETextGender::Neuter;
	Suds->SetGlobalVariableGender(TEXT("SpielerGeschlecht"), Gender);
}

// ---------------------------------------------------------------------------
// Konsole

namespace
{
	UEwigfelsAppearanceComponent* FindLocalLook(UWorld* World, ULocalPlayer** OutLocalPlayer = nullptr)
	{
		APlayerController* PC = World ? World->GetFirstPlayerController() : nullptr;
		if (OutLocalPlayer)
		{
			*OutLocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
		}
		APawn* Pawn = PC ? PC->GetPawn() : nullptr;
		return Pawn ? Pawn->FindComponentByClass<UEwigfelsAppearanceComponent>() : nullptr;
	}

	UEwigfelsAppearanceSubsystem* FindLookSubsystem(UWorld* World)
	{
		UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
		return GI ? GI->GetSubsystem<UEwigfelsAppearanceSubsystem>() : nullptr;
	}

	FAutoConsoleCommandWithWorldAndArgs GLookEditor(
		TEXT("Ewigfels.Look.Editor"),
		TEXT("Öffnet den Charakter-Editor. Mit Laden-Id (z. B. haarstube) als Laden-Spiegel."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			ULocalPlayer* LocalPlayer = nullptr;
			UEwigfelsAppearanceComponent* Look = FindLocalLook(World, &LocalPlayer);
			UEwigfelsCreatorSubsystem* Creator = LocalPlayer ? LocalPlayer->GetSubsystem<UEwigfelsCreatorSubsystem>() : nullptr;
			if (Look && Creator)
			{
				Creator->Open(Look, Args.Num() > 0 ? FName(*Args[0]) : NAME_None);
			}
		}));

	FAutoConsoleCommandWithWorld GLookRandom(
		TEXT("Ewigfels.Look.Random"),
		TEXT("Zufälliges Aussehen (nur im Editor-Fenster oder bei noch nicht erstellter Figur)."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			ULocalPlayer* LocalPlayer = nullptr;
			FindLocalLook(World, &LocalPlayer);
			if (UEwigfelsCreatorSubsystem* Creator = LocalPlayer ? LocalPlayer->GetSubsystem<UEwigfelsCreatorSubsystem>() : nullptr)
			{
				if (Creator->IsOpen())
				{
					Creator->Randomize();
				}
			}
		}));

	FAutoConsoleCommandWithWorld GLookCode(
		TEXT("Ewigfels.Look.Code"),
		TEXT("Schreibt den Look-Code der eigenen Figur ins Log und in die Zwischenablage."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (const UEwigfelsAppearanceComponent* Look = FindLocalLook(World))
			{
				const FString Code = EwigfelsLook::ToLookCode(Look->GetAppearance());
				FPlatformApplicationMisc::ClipboardCopy(*Code);
				UE_LOG(LogEwigfels, Display, TEXT("Look-Code (in der Zwischenablage): %s"), *Code);
			}
		}));

	FAutoConsoleCommandWithWorldAndArgs GLookImport(
		TEXT("Ewigfels.Look.Import"),
		TEXT("Lädt einen Look-Code ins offene Editor-Fenster: Ewigfels.Look.Import NOC1:…"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
		{
			ULocalPlayer* LocalPlayer = nullptr;
			FindLocalLook(World, &LocalPlayer);
			UEwigfelsCreatorSubsystem* Creator = LocalPlayer ? LocalPlayer->GetSubsystem<UEwigfelsCreatorSubsystem>() : nullptr;
			FEwigfelsAppearance Imported;
			if (!Creator || !Creator->IsOpen() || Args.Num() == 0 || !EwigfelsLook::FromLookCode(Args[0], Imported))
			{
				UE_LOG(LogEwigfels, Warning, TEXT("Look: Import geht nur mit offenem Editor und gültigem Code (NOC1:…)"));
				return;
			}
			Creator->Edit([&Imported](FEwigfelsAppearance& L)
			{
				const bool bCreated = L.bCreated;
				L = Imported;
				L.bCreated = bCreated;
			});
		}));

	FAutoConsoleCommandWithWorld GLookReload(
		TEXT("Ewigfels.Look.Reload"),
		TEXT("Lädt Content/Data/Appearance neu."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			if (UEwigfelsAppearanceSubsystem* Sub = FindLookSubsystem(World))
			{
				Sub->ReloadCatalog();
			}
		}));

	FAutoConsoleCommandWithWorld GLookReset(
		TEXT("Ewigfels.Look.Reset"),
		TEXT("Figur vergessen (nur Server/Einzelspieler): Editor öffnet sich wie bei einem neuen Spiel."),
		FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World)
		{
			ULocalPlayer* LocalPlayer = nullptr;
			UEwigfelsAppearanceComponent* Look = FindLocalLook(World, &LocalPlayer);
			UEwigfelsAppearanceSubsystem* Sub = FindLookSubsystem(World);
			if (!Look || !Sub || !Look->GetOwner()->HasAuthority())
			{
				return;
			}
			FEwigfelsAppearance Fresh = Look->GetAppearance();
			Fresh.bCreated = false;
			Look->SetAppearanceAuthority(Fresh);
			if (UEwigfelsCreatorSubsystem* Creator = LocalPlayer ? LocalPlayer->GetSubsystem<UEwigfelsCreatorSubsystem>() : nullptr)
			{
				Creator->Open(Look, NAME_None);
			}
		}));
}
