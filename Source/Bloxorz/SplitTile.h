// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloxGridTile.h"
#include "SplitTile.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API ASplitTile : public ABloxGridTile
{
	GENERATED_BODY()
public:
	void SetSpawnTileIndex(int TileIndexToSpawn);
protected:
	virtual void Trigger(ABlox* BloxActor) override;
public:
	int32 SpawnTile1 = -1;
	int32 SpawnTile2 = -1;
};
