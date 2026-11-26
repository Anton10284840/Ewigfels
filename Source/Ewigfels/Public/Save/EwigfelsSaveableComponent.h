// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EwigfelsSaveableComponent.generated.h"

struct FEwigfelsActorRecord;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEwigfelsSaveableRestored);

/**
 * An jeden Actor hängen, dessen Zustand den Spielstand überleben soll (Truhe offen, Tür zu, Sammelobjekt weg …).
 * Gespeichert werden: alle Variablen des Actors mit Häkchen „SaveGame“ (Blueprint: Details > Advanced > SaveGame,
 * C++: UPROPERTY(SaveGame)), optional die Position.
 *
 * World Partition: Wird der Actor entladen, merkt sich die Komponente den Zustand; lädt er wieder, stellt sie ihn her.
 * Actors, die erst zur Laufzeit gespawnt werden, brauchen eine feste SaveId.
 */
UCLASS(ClassGroup = (Ewigfels), meta = (BlueprintSpawnableComponent))
class EWIGFELS_API UEwigfelsSaveableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEwigfelsSaveableComponent();

	/** Eindeutiger Name im Spielstand. Leer = automatisch aus Level und Actor-Name (reicht für platzierte Actors). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Save")
	FName SaveId;

	/** Position/Drehung mitspeichern (für bewegliche Dinge, z. B. verschobene Kisten). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ewigfels|Save")
	bool bSaveTransform = false;

	/** Actor dauerhaft entfernen (eingesammelt, zerstört). Bleibt auch nach Laden weg. */
	UFUNCTION(BlueprintCallable, Category = "Ewigfels|Save")
	void DestroyPersistently();

	/** Wird nach dem Wiederherstellen aufgerufen (Blueprint: Optik an geladene Werte anpassen, z. B. Truhendeckel offen). */
	UPROPERTY(BlueprintAssignable, Category = "Ewigfels|Save")
	FEwigfelsSaveableRestored OnRestored;

	FName GetEffectiveSaveId() const;
	void WriteRecord(FEwigfelsActorRecord& OutRecord) const;
	void ReadRecord(const FEwigfelsActorRecord& Record);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
