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

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* EditorMappingContext;

	/** Mouse Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseMoveAction;
	/** Mouse Select Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseClickedAction;

	/** Change Tile Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchTileTypeAction;
	
	/** Delete Tile Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DeleteTileAction;

	/** Delete Tile Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeModeAction;

	// Editor Widget To Use
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UEditorWidget> EditorWidget;

public:
	// Sets default values for this pawn's properties
	AEditorPawn();

	void OnMouseMoved(const FInputActionValue& Value);
	void OnMouseClicked(const FInputActionValue& Value);
	void OnDeleteAction(const FInputActionValue& Value);
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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetCameraLocation(FVector Location);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	TMap<int, TSet<int>> LinkMap;
	int LinkSendTileIndex;
	EEditorState EditorState;
    UEditorWidget* Widget;
	APlayerController* PlayerController;
	ABloxGrid* Grid;
	FString SelectedTileName;
	int SelectedTileIndex;
	int TileTypeIndex = 0;
};
