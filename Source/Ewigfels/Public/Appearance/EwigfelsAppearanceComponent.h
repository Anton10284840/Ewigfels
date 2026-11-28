// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Appearance/EwigfelsAppearanceTypes.h"
#include "EwigfelsAppearanceComponent.generated.h"

class UMeshComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEwigfelsLookApplied, const FEwigfelsAppearance&, Look);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEwigfelsLookChangeResult, bool, bAccepted, int32, Price, const FText&, Reason);

/**
 * Aussehen der Spielfigur: speichert es (repliziert), prüft Änderungen auf dem Server und baut es auf die Figur.
 *
 * Ablauf: Editor/Spiegel ruft RequestChange → Server prüft (Katalog, Laden, Freischaltung, Bezahlen) → setzt
 * Appearance → alle Rechner bauen die Optik (OnRep). Vorschau im Editor (SetPreview) bleibt lokal.
 *
 * Was angewendet wird (Namen für Materialien/Meshes: docs/CHARACTER_CREATOR.md §4):
 *  - Körper-Mesh je Körpertyp, Größe als Skalierung
 *  - fertiger Kopf als eigenes Mesh (Gesichts-Kind "Face" oder eigenes Teil), optionale Morph-Targets
 *  - Material-Parameter (Haut, Augen, Haare, Male, Farbzonen, Quarzfarbe)
 *  - Kleidung, Frisur, Bart, Brauen als eigene Skeletal Meshes, die dem Körper folgen (Leader Pose)
 *  - Stimmringe als Static Meshes an den Finger-Sockets
 *  - ohne Assets: Farben + Größe auf den Platzhalter-Körper
 * Grooms (echte Haarsträhnen) setzt ein Blueprint über OnAppearanceApplied.
 */
UCLASS(ClassGroup = (Ewigfels), Config = Game, meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsAppearanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsAppearanceComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Look")
	const FEwigfelsAppearance& GetAppearance() const { return Appearance; }

	UFUNCTION(BlueprintPure, Category = "Ewigfels|Look")
	bool IsCreated() const { return Appearance.bCreated; }

	/** Änderung anfragen (vom besitzenden Spieler). ShopId None = Charakter-Editor. Antwort kommt über OnChangeResult. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Look")
	void RequestChange(const FEwigfelsAppearance& NewLook, FName ShopId);

	/** Nur lokal: Aussehen anzeigen, ohne es zu übernehmen (Editor-Vorschau). */
	void SetPreview(const FEwigfelsAppearance& Look);
	/** Vorschau beenden. bKeepVisuals: Optik stehen lassen (zeigt schon das übernommene Aussehen), außer der Server hat inzwischen etwas anderes geschickt. */
	void ClearPreview(bool bKeepVisuals = false);
	bool IsPreviewing() const { return bPreviewing; }

	/** Nur Server: direkt setzen, ohne Laden-Regeln (Spielstand laden, Tests). Wird trotzdem bereinigt. */
	void SetAppearanceAuthority(const FEwigfelsAppearance& Look);

	/** Letzte Antwort des Servers (für Tests und UI ohne Delegate). false = abgelehnt oder noch keine. */
	bool GetLastResult(int32& OutPrice, FText& OutReason) const
	{
		OutPrice = LastResultPrice;
		OutReason = LastResultReason;
		return bLastResultAccepted;
	}

	/** Farbe der eigenen Magie (Quarz des ersten Stimmrings). */
	UFUNCTION(BlueprintPure, Category = "Ewigfels|Look")
	FLinearColor GetMagicColor() const { return Appearance.GetMagicColor(); }

	/** Nach jedem Aufbau der Optik (auch Vorschau). Für Blueprint-Extras wie Grooms. */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Look")
	FEwigfelsLookApplied OnAppearanceApplied;

	/** Das gültige Aussehen hat sich geändert (Server-Entscheidung, Spielstand, Replikation) – auch während einer Vorschau. */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Look")
	FEwigfelsLookApplied OnAppearanceReplaced;

	/** Antwort des Servers auf RequestChange (nur beim anfragenden Spieler). */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Look")
	FEwigfelsLookChangeResult OnChangeResult;

	// --- Einstellungen (DefaultGame.ini: [/Script/Ewigfels.EwigfelsAppearanceComponent]) ---

	/** Charakter-Editor öffnen, wenn die Figur noch nicht erstellt ist. false = Profil/Standard automatisch nehmen (Tests). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Look")
	bool bOpenCreatorOnStart = true;

	/** Mesh eines Stimmrings (Metall + Quarz über Material-Parameter). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Look")
	FSoftObjectPath RingMesh = FSoftObjectPath(TEXT("/Game/Characters/Player/Rings/SM_Stimmring.SM_Stimmring"));

	/** Sockets für Ring 1–4 am Körper-Skelett (CHARACTER_BIBLE §1.1). */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Look")
	TArray<FName> RingSockets = { TEXT("ring_01_r"), TEXT("ring_02_r"), TEXT("ring_01_l"), TEXT("ring_02_l") };

	/** Max. Abstand zum Spiegel eines Ladens, damit der Server eine Laden-Änderung annimmt. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Look")
	float ShopMirrorRange = 800.f;

protected:
	UFUNCTION()
	void OnRep_Appearance();

	UFUNCTION(Server, Reliable)
	void Server_RequestChange(const FEwigfelsAppearance& NewLook, FName ShopId);

	UFUNCTION(Client, Reliable)
	void Client_ChangeResult(bool bAccepted, int32 Price, const FText& Reason);

private:
	void HandleChangeOnServer(const FEwigfelsAppearance& NewLook, FName ShopId);
	void ApplyVisuals(const FEwigfelsAppearance& Look);
	void ApplyToPlaceholder(const FEwigfelsAppearance& Look);
	void ApplyMorphs(USkeletalMeshComponent* Mesh, const FEwigfelsAppearance& Look) const;
	void ApplyBodyParams(UMeshComponent* Mesh, const FEwigfelsAppearance& Look);
	USkeletalMeshComponent* UpdateAttachedPiece(FName Key, const FSoftObjectPath& Asset, USkeletalMeshComponent* Body);
	void UpdateRings(const FEwigfelsAppearance& Look, USkeletalMeshComponent* Body);
	TArray<UMaterialInstanceDynamic*> GetMids(UMeshComponent* Mesh);
	bool IsNearShopMirror(FName ShopId) const;
	void CheckStartup();

	UPROPERTY(ReplicatedUsing = OnRep_Appearance)
	FEwigfelsAppearance Appearance;

	/** Kopf/Kleidung/Haare/Bart/Brauen: Schlüssel = Slot bzw. "head", "hair", "beard", "brows". */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<USkeletalMeshComponent>> Pieces;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> RingComponents;

	bool bPreviewing = false;
	/** Während der Vorschau kam ein neues Aussehen an (Server/OnRep) und wurde noch nicht angezeigt. */
	bool bChangedDuringPreview = false;
	bool bBaseCaptured = false;
	FVector BaseMeshScale = FVector::OneVector;
	FVector BasePlaceholderLocation = FVector::ZeroVector;
	FTimerHandle StartupTimer;
	bool bLastResultAccepted = false;
	int32 LastResultPrice = 0;
	FText LastResultReason;
	int32 StartupChecks = 0;
};
