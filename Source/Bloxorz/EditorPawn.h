// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BloxGrid.h"
#include "EditorWidget.h"
#include "EditorPawn.generated.h"

UENUM()
enum class EEditorState : uint8
{
	EDITING UMETA(DisplayName = "Editing"),
	LINKING UMETA(DisplayName = "Linking")
};

UCLASS()
class BLOXORZ_API AEditorPawn : public APawn
{
	GENERATED_BODY()

	//----------------------------------------------------------------------------------------------
	/** INPUT ACTIONS */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* EditorMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseClickedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchTileTypeAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DeleteTileAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EmptyGridAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FillGridAction;

	/** END INPUT ACTIONS */
	//----------------------------------------------------------------------------------------------

	// Editor Widget To Use
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UEditorWidget> EditorWidget;

public:
	// STANDARD FUNCTIONS
	AEditorPawn();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// EVENTS
	void OnMouseClicked(const FInputActionValue& Value);
	void OnDeleteAction(const FInputActionValue& Value);

	// EDITOR ACTIONS
	void SwitchTileType(const FInputActionValue& Value);
	void ChangeEditorMode(const FInputActionValue& Value);
	UFUNCTION()
	void SerializeGrid();
	UFUNCTION(BlueprintCallable)
	FString GetSelectedTile();
	UFUNCTION()
	void ResizeGrid();
	UFUNCTION()
    void FillGrid();
	UFUNCTION()
	void EmptyGrid();

private:
	void VerifyLinkIntegrity();
	void StopLinkVisualization();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void LinkTile(ABloxGridTile& TileHit);

	UFUNCTION(BlueprintImplementableEvent)
	void SetCameraLocation(FVector Location);

private:
	// LINK STATE VARIABLES
	// Manage the links between buttons and bridge tiles
	TMap<int, TSet<int>> LinkMap;
	int LinkSendTileIndex;

	// UNREAL STATE VARIABLES
	EEditorState EditorState;
    UEditorWidget* Widget;
	APlayerController* PlayerController;
	ABloxGrid* Grid;

	// SELECTED TILE INFO VARIABLES
	FString SelectedTileName;
	int SelectedTileIndex;
	int PrevSelectedTileIndex = -1;
	float PrevSelectedTileHitTime = 0.0f;
	int TileTypeIndex = 0;
	bool StartTilePlaced = false;
	bool EndTilePlaced = false;
};
