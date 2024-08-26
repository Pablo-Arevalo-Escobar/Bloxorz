// Fill out your copyright notice in the Description page of Project Settings.
#include "SplitTile.h"
#include "Blox.h"

void ASplitTile::Trigger(ABlox* BloxActor)
{
    if (BloxActor->GetBloxState() == BloxState::STANDING)
    {
        UE_LOG(LogTemp, Warning, TEXT("STANDING BROADCAST"));
        TileTrigger.Broadcast(this);
    }
}

void ASplitTile::SetSpawnTileIndex(int TileIndexToSpawn)
{
    UE_LOG(LogTemp, Warning, TEXT("Tile index to spawn %d"), TileIndexToSpawn);
    if (SpawnTile1 == -1)
    {
        SpawnTile1 = TileIndexToSpawn;
    }
    else
    {
        SpawnTile2 = TileIndexToSpawn;
    }
}
