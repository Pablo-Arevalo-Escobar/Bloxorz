// Copyright Epic Games, Inc. All Rights Reserved.


#include "BloxorzGameModeBase.h"
#include "Kismet/GameplayStatics.h" // Include GameplayStatics
#include "SplitTile.h"





void ABloxorzGameModeBase::InitGameState()
{
}

void ABloxorzGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    if (!(NewPlayer && NewPlayer->K2_GetPawn()))
    {
        return;
    }
    
    // Retrieve Player Blox Pawn
    UE_LOG(LogTemp, Warning, TEXT("Playercontroller login %s"), *NewPlayer->K2_GetPawn()->GetName());
    if (!Cast<ABlox>(NewPlayer->K2_GetPawn()))
    {
        UE_LOG(LogTemp,Warning,TEXT("Unable to cast default pawn to blox class"))
        return;
    }
    StandardBlox = Cast<ABlox>(NewPlayer->K2_GetPawn());

    // Retrieve Grid
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABloxGrid::StaticClass(), FoundActors);
    if (FoundActors.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Unable to find level grid"));
        return;
    }
    Grid = Cast<ABloxGrid>(FoundActors[0]);

    // Retrieve camera
    FoundActors.Empty();
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("LevelCamera"), FoundActors);
    if (FoundActors.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Unable to find level camera"));
        return;
    }
    LevelCamera = FoundActors[0];

    bPosses1 = true;
}

void ABloxorzGameModeBase::SwitchBloxPawn()
{
    if (!BloxSplit0 || !BloxSplit1)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid reference to split pawn"));
        return;
    }

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
        return;

    //PlayerController->GetPawn();
    bPosses1 = !bPosses1;
    if (bPosses1)
    {
        PlayerController->Possess(BloxSplit0);
        BloxSplit0->Highlight();
    }
    else
    {
        PlayerController->Possess(BloxSplit1);
        BloxSplit1->Highlight();
    }
    PlayerController->SetViewTargetWithBlend(LevelCamera);
}

void ABloxorzGameModeBase::MergeSplitBlox(BloxMoveDirection MoveDirection)
{
    UE_LOG(LogTemp, Warning, TEXT("MERGING SPLIT BLOX"));
    if (!DoubleBlox)
    {
        UE_LOG(LogTemp, Warning, TEXT("Double blox template is null"));
        return;
    }
    // Compute rotation to spawn the blox pawn in
    // 1. Spawn main blox
    StandardBlox = GetWorld()->SpawnActor<ABlox>(DoubleBlox);
    FVector SpawnLocation = bPosses1 ? BloxSplit0->GetActorLocation() : BloxSplit1->GetActorLocation();
    StandardBlox->SetActorLocation(SpawnLocation + FVector::UpVector*10000);
   
    // Vertical rotation
    // Horizontal rotation

    // 2. Delete split blox
    BloxSplit0->Destroy();
    BloxSplit1->Destroy();
    BloxSplit0 = nullptr;
    BloxSplit1 = nullptr;

    StandardBlox->SnapToGrid();
    StandardBlox->ToggleVisibility(true);
    StandardBlox->Rotate(MoveDirection);
    StandardBlox->SetBloxBehaviour(DEFAULT_STATE);
    Grid->RegisterBloxPawn(StandardBlox);

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("INVALID PLAYER CONTROLLER REFERENCE"));
        return;
    }
    PlayerController->Possess(StandardBlox);
    PlayerController->SetViewTargetWithBlend(LevelCamera);
}

// Splits the blox pawn into two seperate pawns
void ABloxorzGameModeBase::SplitBloxPawn(ABloxGridTile* GridTile)
{
  UE_LOG(LogTemp, Warning, TEXT("SPLITTING BLOX PAWN"));
  if(BloxSplit0 && BloxSplit1)
  {
      UE_LOG(LogTemp, Warning, TEXT("Blox split already exists"));
      return;
  }
  if (!SingleBlox)
  {
      UE_LOG(LogTemp, Warning, TEXT("SingleBlox Template is null"));
      return;
  }
  ASplitTile* SplitTile = Cast<ASplitTile>(GridTile);

  int TileIndexToSpawn1 = SplitTile->SpawnTile1;
  int TileIndexToSpawn2 = SplitTile->SpawnTile2;

  if (TileIndexToSpawn1 == -1 || TileIndexToSpawn2 == -1)
  {
      UE_LOG(LogTemp, Warning, TEXT("Invalid split tile configuration"));
      return;
  }

  BloxSplit0 = GetWorld()->SpawnActor<ABlox>(SingleBlox);
  BloxSplit0->SetActorLocation(Grid->GetTileLocation(TileIndexToSpawn1) + FVector::UpVector*1000);
  BloxSplit0->SnapToGrid();
  BloxSplit0->ToggleVisibility(true);

  BloxSplit1 = GetWorld()->SpawnActor<ABlox>(SingleBlox);
  BloxSplit1->SetActorLocation(Grid->GetTileLocation(TileIndexToSpawn2) + FVector::UpVector*1000);
  BloxSplit1->SnapToGrid();
  BloxSplit1->ToggleVisibility(true);
  StandardBlox->Destroy();
  SwitchBloxPawn();
}
 