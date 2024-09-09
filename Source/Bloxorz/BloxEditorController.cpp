// Fill out your copyright notice in the Description page of Project Settings.


#include "BloxEditorController.h"
#include <EnhancedInputComponent.h>
#include <EnhancedInputSubsystems.h>
#include <Kismet/GameplayStatics.h>

ABloxEditorController::ABloxEditorController()
{
}

void ABloxEditorController::BeginPlay()
{
    Super::BeginPlay();
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(EditorControllerMappingContext, 0);
    }

    // RETRIEVE POSESSION PAWNS
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        if (Actor->Tags.Contains(TEXT("EditorPawn")))
        {
            EditorPawn = Cast<APawn>(Actor);
        }
        else if (Actor->Tags.Contains(TEXT("CameraPawn")))
        {
            CameraPawn = Cast<APawn>(Actor);
        }
    }

    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("LevelCamera"), FoundActors);
    if (FoundActors.Num() > 0)
    {
        EditorCamera = Cast<AActor>(FoundActors[0]);
        SetViewTarget(EditorCamera);
    }

    // Default values for the editor pawn (Possesion handled by default pawn)
    SetShowMouseCursor(true);
    bEnableMouseOverEvents = true;
}

void ABloxEditorController::SetupInputComponent()
{
    Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(PawnSwitchAction, ETriggerEvent::Triggered, this, &ABloxEditorController::SwitchPawn);
	    UE_LOG(LogTemp, Warning, TEXT("SetupInputComponent"))
    }
}

void ABloxEditorController::SwitchPawn()
{
    UE_LOG(LogTemp, Warning, TEXT("SwitchPawn"))
    // lOG BOOL VARIABLES AND NULLPTRS
    UE_LOG(LogTemp, Warning, TEXT("PosessingEditorPawn: %d"), PosessingEditorPawn)
    if (EditorPawn) UE_LOG(LogTemp, Warning, TEXT("EditorPawn: %s"), *EditorPawn->GetName());
    if (CameraPawn) UE_LOG(LogTemp, Warning, TEXT("CameraPawn: %s"), *CameraPawn->GetName());
    // END LOG BOOL VARIABLES AND NULLPTRS

	if (PosessingEditorPawn && CameraPawn)
    {
        Possess(CameraPawn);

        PosessingEditorPawn = false;
        SetShowMouseCursor(false);
        bEnableMouseOverEvents = false;
        UE_LOG(LogTemp, Warning, TEXT("Switching to CameraPawn"))

    }
    else if (!PosessingEditorPawn && EditorPawn)
    {
        Possess(EditorPawn);
        SetViewTarget(EditorCamera);

        PosessingEditorPawn = true;
        SetShowMouseCursor(true);
        bEnableMouseOverEvents = true;
        UE_LOG(LogTemp, Warning, TEXT("Switching to EditorPawn"))
    }
}
