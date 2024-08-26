// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloxGridTile.h"
#include "BridgeTile.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API ABridgeTile : public ABloxGridTile
{
	GENERATED_BODY()
public:
	ABridgeTile();
	void SetDirection(EBloxTileType Type);
	void ToggleBridge();
	virtual void LinkReceived(ABloxGridTile* GridTile) override;
protected:
	void RotateAroundPoint(UPrimitiveComponent* Component, FVector RotationPoint, FRotator Rotation);
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* BridgeTile;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* PivotLeft;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* PivotRight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* PivotUp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* PivotDown;
};
