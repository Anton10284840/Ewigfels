// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EwigfelsGameMode.generated.h"

/**
 * Spiel-Regeln. Phase 3: Spielfigur (AEwigfelsCharacter oder ein Blueprint davon), HUD, und in leeren Leveln
 * automatisch die Testarena. Läuft nur auf dem Server (Koop-fähig: jeder Spieler bekommt eine eigene Figur).
 */
UCLASS(Config = Game)
class EWIGFELS_API AEwigfelsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AEwigfelsGameMode();

	/** Text, der beim Spielstart im Log und auf dem Bildschirm erscheint. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ewigfels|Debug")
	FString StartupMessage;

	/**
	 * Optionaler Blueprint der Spielfigur (z. B. /Game/Ewigfels/BP_NocturnaPlayer.BP_NocturnaPlayer_C).
	 * Fehlt er, wird die C++-Figur genutzt. In DefaultGame.ini: [/Script/Ewigfels.EwigfelsGameMode] PlayerPawnClass=…
	 */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Player")
	FSoftClassPath PlayerPawnClass;

	/** Leeres Level ohne PlayerStart → Testarena + Startpunkt automatisch anlegen. */
	UPROPERTY(Config, EditDefaultsOnly, Category = "Ewigfels|Debug")
	bool bAutoSpawnTestArena = true;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UClass> ResolvedPawnClass;
};
