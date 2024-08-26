// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloxGridTile.h"
#include <Components/BoxComponent.h>
#include "EndTile.generated.h"
/**
 * 
 */
UCLASS()
class BLOXORZ_API AEndTile : public ABloxGridTile
{
	GENERATED_BODY()
public:
	AEndTile();
protected:
	virtual void Trigger(ABlox* BloxActor) override;
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintImplementableEvent)
	void OnEndAnimation();
protected:
	float DownTime = 0.25f;
	float PreviosTimePressed = 0.0f;
};
