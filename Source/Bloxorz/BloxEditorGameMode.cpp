// Fill out your copyright notice in the Description page of Project Settings.


#include "BloxEditorGameMode.h"

ABloxEditorGameMode::ABloxEditorGameMode()
{
    DefaultPawnClass = AEditorPawn::StaticClass();
}


void ABloxEditorGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
    UE_LOG(LogTemp, Warning, TEXT("InitGame Editor"));
}

void ABloxEditorGameMode::InitGameState()
{
    Super::InitGameState();
    UE_LOG(LogTemp, Warning, TEXT("InitGameState Editor"));

    // Spawn grid actor 
    if (!GridTemplate)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Grid Template"));
        return;
    }
    Grid = GetWorld()->SpawnActor<ABloxGrid>(GridTemplate);
    Grid->LoadOnStart = false;
    Grid->InEditorMode = true;
    if (!Grid)
    {
        UE_LOG(LogTemp, Warning, TEXT("Unable to spawn grid actor"));
    }
}

