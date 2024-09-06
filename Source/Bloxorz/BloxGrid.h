// Fill out your copyright notice in the Description page of Project Settings.
// Add functionality to make a custom grid
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BloxGridTile.h"
#include "BloxGrid.generated.h"
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartAnimationEndSignature);

/*
* A grid class that stores specific instantiated values.
*/
UCLASS()
class BLOXORZ_API ABloxGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABloxGrid();
	~ABloxGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(CallInEditor, Category = "BloxGrid")
	void Reconstruct();
	void Initalize();
	UFUNCTION(BlueprintCallable)
	void LoadLevel(FString Level, bool IsPreview);
	void LoadEmpty();
	void PlayStartAnimation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool SwitchTileType(int TileToSwitch, EBloxTileType TileType);
	
	EBloxTileType GetTileType(int TileIndex);
	FVector GetPlayerStart();
	FVector GetTileLocation(int TileIndexIn);
	ABloxGridTile* GetTileAtIndex(int TileIndex);
	void HighlightTileAtIndex(int TileIndex, bool DoHighlight);

	UFUNCTION(BlueprintCallable)
	void SerializeGrid(TArray<FString>& iLinkStrings, const FString& GridName = TEXT("CustomLevel"));
	void RegisterBloxPawn(ABlox* BloxPawn);
    UFUNCTION()
    void Resize(int GridResolution);

    void Fill(EBloxTileType TypeToFill);
private:
	ABloxGridTile* MakeTile(FVector TileRelativeLocation, int TileIndex, EBloxTileType TileType, bool bUseRelative);
private:
	void ReadIntoLines(FString FileContent, TArray<FString>& FileLines, TArray<FString>& Links);

public:
	UPROPERTY(BlueprintCallable)
	FOnStartAnimationEndSignature OnAnimationEndTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	bool InEditorMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	bool LoadOnStart = false;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	float GridCellSize = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	float GridResolution = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	bool CreateNew = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	FString Level = "Lvl0";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	UStaticMeshComponent* FloorMesh;

	// Tile types (made in blue print and used here)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> Tile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> FallTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> EmptyTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> StartTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> EndTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> ButtonSwitchTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> CrossSwitchTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	TSubclassOf<ABloxGridTile> BridgeTile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
    TSubclassOf<ABloxGridTile> SplitTile;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid")
	FVector TileScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid Animation")
	float StartAnimationRandOffset = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid Animation")
	float StartAnimationBottomOffset = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BloxGrid Animation")
	float AnimationLength = 1;


private:
	int PlayerStartTileIndex = -1;
	int EndTileIndex = -1;
	TArray<ABloxGridTile*> FloorArray;
	TArray<int> ValidFloors;


	// Animation Variables
	TMap<int, float> FloorZOffset;
	bool InAnimation = false;
	bool GridAnimation = false;
	float AnimationStartTime = 0;
	
};
