// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BloxGridTile.generated.h"

class ABlox;
UENUM(BlueprintType)
enum class EBloxTileType : uint8
{
	STANDARD UMETA(DisplayName = "StandardTile"),
	START UMETA(DispalyName = "StartTile"),
	END UMETA(DisplayName = "EndTile"),
	FALL UMETA(DisplayName = "FallTile"),
	BUTTON_SWITCH UMETA(DisplayName = "ButtonSwitchTile"),
	CROSS_SWITCH UMETA(DisplayName = "CrossSwitchTile"),
	EMPTY UMETA(DisplayName = "Empty"),
    SPLIT UMETA(DisplayName = "Split"),

	// Bridge Tiles
	BRIDGE_LEFT UMETA(DisplayName = "BridgeLeftTile"),
	BRIDGE_RIGHT UMETA(DisplayName = "BridgeRightTile"),
	BRIDGE_UP UMETA(DisplayName = "BridgeUpTile"),
	BRIDGE_DOWN UMETA(DisplayName = "BridgeDownTile"),

	ENUM_END
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileEventSignature, ABloxGridTile*, GridTile);

UCLASS()
class BLOXORZ_API ABloxGridTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABloxGridTile();
	EBloxTileType GetTileType();
	UFUNCTION()
	virtual void LinkReceived(ABloxGridTile* GridTile);
	// Tile specific behaviour
	UFUNCTION()
	virtual void Trigger(ABlox* Blox);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FOnTileEventSignature TileTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBloxTileType TileType = EBloxTileType::STANDARD;

	int TileIndex = 0;

	UFUNCTION()
	void HighlightTile(bool bToHighlight);
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMeshComponent* TileMesh;
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    UMaterialInterface* OverlayMaterial;
};
