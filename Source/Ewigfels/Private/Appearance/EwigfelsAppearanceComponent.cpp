// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "Appearance/EwigfelsAppearanceComponent.h"
#include "Appearance/EwigfelsAppearanceSubsystem.h"
#include "Character/EwigfelsPlaceholderBody.h"
#include "Ewigfels.h"
#include "Magic/EwigfelsSpellVisuals.h"
#include "Save/EwigfelsSaveSubsystem.h"
#include "UI/EwigfelsCharacterCreator.h"
#include "World/EwigfelsMirror.h"
#include "WorldState/EwigfelsWorldStateSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#define LOCTEXT_NAMESPACE "EwigfelsLook"

namespace
{
	const FName LookPieceHair(TEXT("hair"));
	const FName LookPieceBeard(TEXT("beard"));
	const FName LookPieceBrows(TEXT("brows"));
	const FName LookFlagCreated(TEXT("Look_Created"));

	FLinearColor LookLin(const FColor& C) { return FLinearColor::FromSRGBColor(C); }

	UEwigfelsAppearanceSubsystem* LookSubsystemOf(const UActorComponent* Comp)
	{
		const UWorld* World = Comp->GetWorld();
		UGameInstance* GI = World ? World->GetGameInstance() : nullptr;
		return GI ? GI->GetSubsystem<UEwigfelsAppearanceSubsystem>() : nullptr;
	}

	int32 LookIndexOf(const FEwigfelsLookCatalog& Catalog, FName Category, FName Id)
	{
		return Catalog.GetOptions(Category).IndexOfByPredicate([Id](const FEwigfelsLookOption& O) { return O.Id == Id; });
	}

	FSoftObjectPath LookAssetOf(const FEwigfelsLookCatalog& Catalog, FName Category, FName Id)
	{
		const FEwigfelsLookOption* Option = Catalog.Find(Category, Id);
		return Option ? Option->Asset : FSoftObjectPath();
	}
}

UEwigfelsAppearanceComponent::UEwigfelsAppearanceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UEwigfelsAppearanceComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEwigfelsAppearanceComponent, Appearance);
}

void UEwigfelsAppearanceComponent::BeginPlay()
{
	Super::BeginPlay();

	// Noch nicht erstellte Figur sieht bis zur Wahl wie der Katalog-Standard aus
	if (!Appearance.bCreated)
	{
		if (const UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this))
		{
			if (GetOwner()->HasAuthority())
			{
				Appearance = Sub->GetCatalog().Defaults;
				EwigfelsLook::Sanitize(Appearance, Sub->GetCatalog());
				Appearance.bCreated = false;
			}
		}
	}
	ApplyVisuals(Appearance);

	// Der lokale Spieler entscheidet, ob der Editor aufgeht. Kurz warten: Possess und Replikation brauchen einen Moment.
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(StartupTimer, this, &UEwigfelsAppearanceComponent::CheckStartup, 0.5f, true, 0.3f);
	}
}

void UEwigfelsAppearanceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StartupTimer);
	}
	Super::EndPlay(EndPlayReason);
}

void UEwigfelsAppearanceComponent::CheckStartup()
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	++StartupChecks;
	if (!Pawn || StartupChecks > 20)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartupTimer);
		return;
	}
	if (!Pawn->IsLocallyControlled() || !Pawn->IsPlayerControlled())
	{
		if (Pawn->HasAuthority() && Pawn->IsPlayerControlled())
		{
			// Server, fremder Spieler: der entscheidet selbst
			GetWorld()->GetTimerManager().ClearTimer(StartupTimer);
		}
		return; // auf Possess warten (oder KI-Figur)
	}
	GetWorld()->GetTimerManager().ClearTimer(StartupTimer);

	UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this);
	if (Appearance.bCreated)
	{
		if (Sub)
		{
			Sub->PushDialogueVariables(Appearance);
		}
		return;
	}

	// Spielstand geladen, bevor es die Figur gab? Dann dessen Aussehen nehmen (nur Server/Einzelspieler).
	if (GetOwner()->HasAuthority())
	{
		FEwigfelsAppearance Loaded;
		const UEwigfelsSaveSubsystem* Save = GetWorld()->GetGameInstance()->GetSubsystem<UEwigfelsSaveSubsystem>();
		if (Save && Save->GetLoadedPlayerAppearance(Loaded) && Loaded.bCreated)
		{
			SetAppearanceAuthority(Loaded);
			if (Sub)
			{
				Sub->PushDialogueVariables(Appearance);
			}
			return;
		}
	}

	const APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	ULocalPlayer* LocalPlayer = PC ? PC->GetLocalPlayer() : nullptr;
	UEwigfelsCreatorSubsystem* Creator = LocalPlayer ? LocalPlayer->GetSubsystem<UEwigfelsCreatorSubsystem>() : nullptr;
	if (bOpenCreatorOnStart && Creator)
	{
		Creator->Open(this, NAME_None);
		return;
	}

	// Ohne Editor: Profil oder Standard übernehmen
	FEwigfelsAppearance Look = Sub ? Sub->GetStartingLook() : Appearance;
	if (Look.Name.TrimStartAndEnd().IsEmpty())
	{
		Look.Name = LOCTEXT("DefaultName", "Novize").ToString();
	}
	RequestChange(Look, NAME_None);
}

// ---------------------------------------------------------------------------
// Ändern

void UEwigfelsAppearanceComponent::RequestChange(const FEwigfelsAppearance& NewLook, FName ShopId)
{
	if (GetOwner()->HasAuthority())
	{
		HandleChangeOnServer(NewLook, ShopId);
	}
	else
	{
		Server_RequestChange(NewLook, ShopId);
	}
}

void UEwigfelsAppearanceComponent::Server_RequestChange_Implementation(const FEwigfelsAppearance& NewLook, FName ShopId)
{
	HandleChangeOnServer(NewLook, ShopId);
}

void UEwigfelsAppearanceComponent::HandleChangeOnServer(const FEwigfelsAppearance& NewLook, FName ShopId)
{
	UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this);
	if (!Sub)
	{
		Client_ChangeResult(false, 0, LOCTEXT("NoCatalog", "Kein Katalog geladen."));
		return;
	}
	const FEwigfelsLookCatalog& Catalog = Sub->GetCatalog();

	FEwigfelsAppearance Clean = NewLook;
	Clean.bCreated = Appearance.bCreated;
	EwigfelsLook::Sanitize(Clean, Catalog);

	int32 Price = 0;
	FText Reason;
	if (!EwigfelsLook::CheckChange(Appearance, Clean, Catalog, Sub->MakeContext(ShopId), Price, Reason))
	{
		Client_ChangeResult(false, 0, Reason);
		return;
	}
	if (!ShopId.IsNone() && !IsNearShopMirror(ShopId))
	{
		Client_ChangeResult(false, 0, LOCTEXT("TooFar", "Du bist zu weit vom Spiegel weg."));
		return;
	}
	if (const FEwigfelsLookShop* Shop = Catalog.Shops.Find(ShopId))
	{
		const bool bCosts = Price > 0 || (!Shop->PriceItem.IsNone() && Shop->PriceItemCount > 0 && Clean != Appearance);
		if (bCosts)
		{
			if (Sub->PaymentHandler)
			{
				if (!Sub->PaymentHandler(Cast<APawn>(GetOwner()), *Shop, Price))
				{
					Client_ChangeResult(false, Price, LOCTEXT("NoMoney", "Das kannst du dir noch nicht leisten."));
					return;
				}
			}
			else
			{
				UE_LOG(LogEwigfels, Log, TEXT("Look: %s kostet %d Kronen - noch kein Inventar, gratis"), *ShopId.ToString(), Price);
			}
		}
	}

	const bool bWasCreated = Appearance.bCreated;
	Clean.bCreated = true;
	Appearance = Clean;
	OnAppearanceReplaced.Broadcast(Appearance);
	if (bPreviewing)
	{
		bChangedDuringPreview = true;
	}
	else
	{
		ApplyVisuals(Appearance);
	}
	if (!bWasCreated)
	{
		if (UEwigfelsWorldStateSubsystem* WorldState = GetWorld()->GetGameInstance()->GetSubsystem<UEwigfelsWorldStateSubsystem>())
		{
			WorldState->SetFlag(LookFlagCreated, true);
		}
	}
	UE_LOG(LogEwigfels, Log, TEXT("Look: %s %s (%d Kronen)"), *Appearance.Name, ShopId.IsNone() ? TEXT("erstellt") : *ShopId.ToString(), Price);
	Client_ChangeResult(true, Price, FText::GetEmpty());
}

void UEwigfelsAppearanceComponent::Client_ChangeResult_Implementation(bool bAccepted, int32 Price, const FText& Reason)
{
	bLastResultAccepted = bAccepted;
	LastResultPrice = Price;
	LastResultReason = Reason;
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (bAccepted && Pawn && Pawn->IsLocallyControlled() && Pawn->IsPlayerControlled())
	{
		if (UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this))
		{
			Sub->SaveProfile(Appearance);
			Sub->PushDialogueVariables(Appearance);
		}
	}
	OnChangeResult.Broadcast(bAccepted, Price, Reason);
}

void UEwigfelsAppearanceComponent::SetAppearanceAuthority(const FEwigfelsAppearance& Look)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	Appearance = Look;
	if (const UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this))
	{
		EwigfelsLook::Sanitize(Appearance, Sub->GetCatalog());
	}
	OnAppearanceReplaced.Broadcast(Appearance);
	if (bPreviewing)
	{
		bChangedDuringPreview = true;
	}
	else
	{
		ApplyVisuals(Appearance);
	}
}

void UEwigfelsAppearanceComponent::OnRep_Appearance()
{
	OnAppearanceReplaced.Broadcast(Appearance);
	if (bPreviewing)
	{
		bChangedDuringPreview = true;
		return;
	}
	ApplyVisuals(Appearance);
}

void UEwigfelsAppearanceComponent::SetPreview(const FEwigfelsAppearance& Look)
{
	bPreviewing = true;
	ApplyVisuals(Look);
}

void UEwigfelsAppearanceComponent::ClearPreview(bool bKeepVisuals)
{
	if (!bPreviewing)
	{
		return;
	}
	bPreviewing = false;
	if (!bKeepVisuals || bChangedDuringPreview)
	{
		ApplyVisuals(Appearance);
	}
	bChangedDuringPreview = false;
}

bool UEwigfelsAppearanceComponent::IsNearShopMirror(FName ShopId) const
{
	const AActor* Owner = GetOwner();
	for (TActorIterator<AEwigfelsMirror> It(GetWorld()); It; ++It)
	{
		if (It->ShopId == ShopId && FVector::Dist(It->GetActorLocation(), Owner->GetActorLocation()) <= ShopMirrorRange)
		{
			return true;
		}
	}
	return false;
}

// ---------------------------------------------------------------------------
// Optik

void UEwigfelsAppearanceComponent::ApplyVisuals(const FEwigfelsAppearance& Look)
{
	const UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this);
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Sub || !Character || IsRunningDedicatedServer())
	{
		OnAppearanceApplied.Broadcast(Look);
		return;
	}
	const FEwigfelsLookCatalog& Catalog = Sub->GetCatalog();
	USkeletalMeshComponent* Body = Character->GetMesh();

	if (!bBaseCaptured && Body)
	{
		BaseMeshScale = Body->GetRelativeScale3D();
		if (const UEwigfelsPlaceholderBody* Placeholder = Character->FindComponentByClass<UEwigfelsPlaceholderBody>())
		{
			BasePlaceholderLocation = Placeholder->GetRelativeLocation();
		}
		bBaseCaptured = true;
	}

	if (Body)
	{
		// Körpertyp: nur tauschen, wenn das Asset da ist (sonst bleibt Mannequin/Platzhalter)
		if (USkeletalMesh* BodyAsset = Cast<USkeletalMesh>(EwigfelsSpellVisuals::LoadIfExists(LookAssetOf(Catalog, EwigfelsLook::Cat::Body, Look.BodyType))))
		{
			if (Body->GetSkeletalMeshAsset() != BodyAsset)
			{
				Body->SetSkeletalMeshAsset(BodyAsset);
			}
		}
		if (Body->GetSkeletalMeshAsset())
		{
			Body->SetRelativeScale3D(BaseMeshScale * Look.Height);
		}

		ApplyMorphs(Body, Look);
		ApplyBodyParams(Body, Look);

		// Kinder des Körpers, die nicht von uns stammen (z. B. MetaHuman-Gesicht): Morphs + Haut/Augen
		TArray<USceneComponent*> Children;
		Body->GetChildrenComponents(false, Children);
		for (USceneComponent* Child : Children)
		{
			USkeletalMeshComponent* ChildMesh = Cast<USkeletalMeshComponent>(Child);
			if (ChildMesh && !Pieces.FindKey(ChildMesh))
			{
				ApplyMorphs(ChildMesh, Look);
				ApplyBodyParams(ChildMesh, Look);
			}
		}

		// Haare, Bart, Brauen
		const TPair<FName, FSoftObjectPath> HairParts[] = {
			{ LookPieceHair, LookAssetOf(Catalog, EwigfelsLook::Cat::Hair, Look.HairStyle) },
			{ LookPieceBeard, LookAssetOf(Catalog, EwigfelsLook::Cat::Beard, Look.Beard) },
			{ LookPieceBrows, LookAssetOf(Catalog, EwigfelsLook::Cat::Brows, Look.Brows) } };
		for (const TPair<FName, FSoftObjectPath>& Part : HairParts)
		{
			if (USkeletalMeshComponent* Mesh = UpdateAttachedPiece(Part.Key, Part.Value, Body))
			{
				for (UMaterialInstanceDynamic* Mid : GetMids(Mesh))
				{
					Mid->SetVectorParameterValue(TEXT("HairColor"), LookLin(Look.HairColor));
					Mid->SetVectorParameterValue(TEXT("HairTipColor"), LookLin(Look.HairTipColor));
				}
				ApplyMorphs(Mesh, Look);
			}
		}

		// Kleidung
		for (const FEwigfelsOutfitPiece& Piece : Look.Outfit)
		{
			if (USkeletalMeshComponent* Mesh = UpdateAttachedPiece(Piece.Slot, LookAssetOf(Catalog, EwigfelsLook::Cat::Outfit, Piece.Item), Body))
			{
				for (UMaterialInstanceDynamic* Mid : GetMids(Mesh))
				{
					Mid->SetVectorParameterValue(TEXT("Dye1"), LookLin(Piece.Color1));
					Mid->SetVectorParameterValue(TEXT("Dye2"), LookLin(Piece.Color2));
					Mid->SetVectorParameterValue(TEXT("Dye3"), LookLin(Piece.Color3));
					Mid->SetScalarParameterValue(TEXT("PatternIndex"), static_cast<float>(FMath::Max(0, LookIndexOf(Catalog, EwigfelsLook::Cat::Pattern, Piece.Pattern))));
					Mid->SetScalarParameterValue(TEXT("FabricIndex"), static_cast<float>(FMath::Max(0, LookIndexOf(Catalog, EwigfelsLook::Cat::Fabric, Piece.Fabric))));
				}
				ApplyMorphs(Mesh, Look); // Körperregler auch auf Kleidung (gleiche Morph-Namen)
			}
		}

		UpdateRings(Look, Body);
	}

	ApplyToPlaceholder(Look);
	OnAppearanceApplied.Broadcast(Look);
}

void UEwigfelsAppearanceComponent::ApplyMorphs(USkeletalMeshComponent* Mesh, const FEwigfelsAppearance& Look) const
{
	const UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this);
	if (!Mesh || !Mesh->GetSkeletalMeshAsset() || !Sub)
	{
		return;
	}
	const FEwigfelsLookCatalog& Catalog = Sub->GetCatalog();

	// Grundgesichter: Preset_02 … Preset_08 sind Morphs relativ zu Gesicht 1
	for (const FEwigfelsLookOption& Face : Catalog.GetOptions(EwigfelsLook::Cat::Face))
	{
		if (Face.Morph.IsNone())
		{
			continue;
		}
		float Weight = 0.f;
		Weight += Look.FaceA == Face.Id ? 1.f - Look.FaceBlend : 0.f;
		Weight += Look.FaceB == Face.Id ? Look.FaceBlend : 0.f;
		Mesh->SetMorphTarget(Face.Morph, Weight, false);
	}
	for (const FEwigfelsSliderDef& Slider : Catalog.Sliders)
	{
		const float Value = Look.GetSlider(Slider.Id);
		Mesh->SetMorphTarget(Slider.MorphPos, FMath::Max(Value, 0.f), false);
		Mesh->SetMorphTarget(Slider.MorphNeg, FMath::Max(-Value, 0.f), false);
	}
}

void UEwigfelsAppearanceComponent::ApplyBodyParams(UMeshComponent* Mesh, const FEwigfelsAppearance& Look)
{
	const UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this);
	if (!Mesh || !Sub)
	{
		return;
	}
	const FEwigfelsLookCatalog& Catalog = Sub->GetCatalog();
	for (UMaterialInstanceDynamic* Mid : GetMids(Mesh))
	{
		Mid->SetVectorParameterValue(TEXT("SkinColor"), LookLin(Look.SkinColor));
		Mid->SetVectorParameterValue(TEXT("EyeColorL"), LookLin(Look.EyeColorL));
		Mid->SetVectorParameterValue(TEXT("EyeColorR"), LookLin(Look.EyeColorR));
		Mid->SetVectorParameterValue(TEXT("HairColor"), LookLin(Look.HairColor));
		Mid->SetVectorParameterValue(TEXT("MagicColor"), Look.GetMagicColor());

		// Haut-Details: Ebenen Mark0 … Mark{max-1}
		for (int32 i = 0; i < Catalog.MaxMarks; ++i)
		{
			const FEwigfelsMarkLook* Mark = Look.Marks.IsValidIndex(i) ? &Look.Marks[i] : nullptr;
			const FString Prefix = FString::Printf(TEXT("Mark%d"), i);
			UTexture* Texture = Mark ? Cast<UTexture>(EwigfelsSpellVisuals::LoadIfExists(LookAssetOf(Catalog, EwigfelsLook::Cat::Marks, Mark->Id))) : nullptr;
			if (Texture)
			{
				Mid->SetTextureParameterValue(FName(*(Prefix + TEXT("Tex"))), Texture);
			}
			Mid->SetVectorParameterValue(FName(*(Prefix + TEXT("Color"))), Mark ? LookLin(Mark->Color) : FLinearColor::Black);
			Mid->SetScalarParameterValue(FName(*(Prefix + TEXT("Opacity"))), Mark && Texture ? Mark->Opacity : 0.f);
		}
	}
}

USkeletalMeshComponent* UEwigfelsAppearanceComponent::UpdateAttachedPiece(FName Key, const FSoftObjectPath& Asset, USkeletalMeshComponent* Body)
{
	USkeletalMesh* Mesh = Cast<USkeletalMesh>(EwigfelsSpellVisuals::LoadIfExists(Asset));
	TObjectPtr<USkeletalMeshComponent>* Existing = Pieces.Find(Key);
	if (!Mesh)
	{
		if (Existing && *Existing)
		{
			(*Existing)->SetVisibility(false);
			(*Existing)->SetSkeletalMeshAsset(nullptr);
		}
		return nullptr;
	}
	USkeletalMeshComponent* Comp = Existing ? Existing->Get() : nullptr;
	if (!Comp)
	{
		Comp = NewObject<USkeletalMeshComponent>(GetOwner(), *FString::Printf(TEXT("Look_%s"), *Key.ToString()));
		Comp->SetupAttachment(Body);
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->RegisterComponent();
		Pieces.Add(Key, Comp);
	}
	if (Comp->GetSkeletalMeshAsset() != Mesh)
	{
		Comp->SetSkeletalMeshAsset(Mesh);
	}
	Comp->SetLeaderPoseComponent(Body);
	Comp->SetVisibility(true);
	return Comp;
}

void UEwigfelsAppearanceComponent::UpdateRings(const FEwigfelsAppearance& Look, USkeletalMeshComponent* Body)
{
	UStaticMesh* Mesh = Cast<UStaticMesh>(EwigfelsSpellVisuals::LoadIfExists(RingMesh));
	const UEwigfelsAppearanceSubsystem* Sub = LookSubsystemOf(this);
	for (int32 i = 0; i < RingSockets.Num(); ++i)
	{
		const bool bWanted = Mesh && Look.Rings.IsValidIndex(i) && Body->DoesSocketExist(RingSockets[i]);
		if (!RingComponents.IsValidIndex(i))
		{
			RingComponents.SetNum(i + 1);
		}
		UStaticMeshComponent* Comp = RingComponents[i];
		if (!bWanted)
		{
			if (Comp)
			{
				Comp->SetVisibility(false);
			}
			continue;
		}
		if (!Comp)
		{
			Comp = NewObject<UStaticMeshComponent>(GetOwner(), *FString::Printf(TEXT("Look_Ring%d"), i + 1));
			Comp->SetupAttachment(Body, RingSockets[i]);
			Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Comp->RegisterComponent();
			RingComponents[i] = Comp;
		}
		Comp->SetStaticMesh(Mesh);
		Comp->SetVisibility(true);
		for (UMaterialInstanceDynamic* Mid : GetMids(Comp))
		{
			Mid->SetVectorParameterValue(TEXT("Quartz"), LookLin(Look.Rings[i].Quartz));
			const int32 MetalIndex = Sub ? LookIndexOf(Sub->GetCatalog(), EwigfelsLook::Cat::Metal, Look.Rings[i].Metal) : 0;
			Mid->SetScalarParameterValue(TEXT("MetalIndex"), static_cast<float>(FMath::Max(0, MetalIndex)));
		}
	}
}

TArray<UMaterialInstanceDynamic*> UEwigfelsAppearanceComponent::GetMids(UMeshComponent* Mesh)
{
	TArray<UMaterialInstanceDynamic*> Mids;
	if (!Mesh)
	{
		return Mids;
	}
	for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
	{
		UMaterialInterface* Current = Mesh->GetMaterial(i);
		if (!Current)
		{
			continue;
		}
		UMaterialInstanceDynamic* Mid = Cast<UMaterialInstanceDynamic>(Current);
		if (!Mid)
		{
			Mid = Mesh->CreateDynamicMaterialInstance(i, Current);
		}
		if (Mid)
		{
			Mids.Add(Mid);
		}
	}
	return Mids;
}

void UEwigfelsAppearanceComponent::ApplyToPlaceholder(const FEwigfelsAppearance& Look)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	UEwigfelsPlaceholderBody* Placeholder = Character ? Character->FindComponentByClass<UEwigfelsPlaceholderBody>() : nullptr;
	if (!Placeholder)
	{
		return;
	}
	// Robe = Umhang (falls getragen) sonst Oberteil, Akzent = Magiefarbe (leuchtende Hände), Augen = Augenfarbe
	const FEwigfelsOutfitPiece* Cloak = Look.FindPiece(TEXT("umhang"));
	const FEwigfelsOutfitPiece* Top = Look.FindPiece(TEXT("oberkoerper"));
	const bool bCloak = Cloak && !Cloak->Item.ToString().EndsWith(TEXT("_nichts"));
	const FEwigfelsOutfitPiece* Main = bCloak ? Cloak : Top;
	const FLinearColor RobeColor = Main ? LookLin(Main->Color1) : FLinearColor(0.035f, 0.05f, 0.11f);
	const FLinearColor EyeColor = FMath::Lerp(LookLin(Look.EyeColorL), FLinearColor::White, 0.35f);
	const FEwigfelsOutfitPiece* Head = Look.FindPiece(TEXT("kopf"));
	const bool bHood = Head && Head->Item.ToString().Contains(TEXT("kapuze"));
	Placeholder->SetLook(RobeColor, Look.GetMagicColor(), EyeColor, 1.f, bHood); // Größe über Skalierung unten

	// Größe live: Komponente skalieren, Füße bleiben auf dem Boden
	if (const UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
	{
		Placeholder->SetRelativeScale3D(FVector(Look.Height));
		Placeholder->SetRelativeLocation(BasePlaceholderLocation + FVector(0.f, 0.f, Capsule->GetUnscaledCapsuleHalfHeight() * (Look.Height - 1.f)));
	}
}

#undef LOCTEXT_NAMESPACE
