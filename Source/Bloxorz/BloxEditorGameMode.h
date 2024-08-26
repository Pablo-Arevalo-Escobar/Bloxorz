// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BloxGrid.h"
#include "EditorPawn.h"
#include "BloxEditorGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API ABloxEditorGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

	//~=============================================================================
	// Initializing the game
	/**
		* Initialize the game.
		* The GameMode's InitGame() event is called before any other functions (including PreInitializeComponents() )
		* and is used by the GameMode to initialize parameters and spawn its helper classes.
		* @warning: this is called before actors' PreInitializeComponents.
		*/
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/**
	 * Initialize the GameState actor with default settings
	 * called during PreInitializeComponents() of the GameMode after a GameState has been spawned
	 * as well as during Reset()
	 */
	virtual void InitGameState() override;
public:
	ABloxEditorGameMode();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ABloxGrid> GridTemplate;
private:
	ABloxGrid* Grid;
};
