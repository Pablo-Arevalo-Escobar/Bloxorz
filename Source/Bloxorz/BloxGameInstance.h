// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BloxGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API UBloxGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentMovesValue();
	UFUNCTION(BlueprintCallable)
	void IncrementMoveDelta();
	UFUNCTION(BlueprintCallable)
	void ResetMoveDelta();
	UFUNCTION(BlueprintCallable)
	void ApplyMoveDelta();
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere);
	int32 MoveDelta;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumberOfMoves;
};
