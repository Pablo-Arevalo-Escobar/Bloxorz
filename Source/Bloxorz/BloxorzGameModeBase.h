// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blox.h"
#include "BloxGrid.h"
#include "BloxorzGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API ABloxorzGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	/**
	* Initialize the GameState actor with default settings
	* called during PreInitializeComponents() of the GameMode after a GameState has been spawned
	* as well as during Reset()
	*/
	virtual void InitGameState() override;

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	// Blox Pawn Functionality
    void SwitchBloxPawn();
	void MergeSplitBlox(BloxMoveDirection MoveDirection);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLevelEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();

	// UFUNCTION macro required for functions subscribed to delegates
	UFUNCTION()
	void SplitBloxPawn(ABloxGridTile* GridTile);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ABlox> SingleBlox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ABlox> DoubleBlox;
	


private:
	AActor* LevelCamera;
	ABlox* StandardBlox;
	ABloxGrid* Grid;
    ABlox* BloxSplit0;
    ABlox* BloxSplit1;
	bool bPosses1;
};
