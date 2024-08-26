// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloxGridTile.h"
#include <Components/BoxComponent.h>
#include "FallTile.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API AFallTile : public ABloxGridTile
{
	GENERATED_BODY()
public:
	AFallTile();
protected:
	virtual void Trigger(ABlox* BloxActor) override;
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UBoxComponent* BoxComponent;
	
};
