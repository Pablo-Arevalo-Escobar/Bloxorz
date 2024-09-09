// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BloxEditorController.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API ABloxEditorController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* EditorControllerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PawnSwitchAction;

public:
	ABloxEditorController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void SwitchPawn();
private:
	FRotator EditorPawnRotation;
	APawn* EditorPawn;
	APawn* CameraPawn;
	AActor* EditorCamera;
	bool PosessingEditorPawn = true;
};
