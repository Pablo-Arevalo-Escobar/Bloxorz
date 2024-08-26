// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloxGridTile.h"
#include <Components/BoxComponent.h>
#include "Blox.h"
#include "ButtonTile.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API AButtonTile : public ABloxGridTile
{
	GENERATED_BODY()


public:
	AButtonTile();
	virtual void BeginPlay() override;

protected:
	virtual void Trigger(ABlox* BloxActor) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DownTime = 0.25f;

private:
	float PreviosTimePressed = 0.0f;
	ABlox* Player;
};
