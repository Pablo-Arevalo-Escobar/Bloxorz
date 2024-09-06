// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API ULevelSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
protected:
	UFUNCTION()
	void OnLevelLoaded();
	UFUNCTION()
	void OnButtonPressed();
protected:
	// UMG Widgets 
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UComboBoxString* mCBLevelSelect;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* mButtonLoadLevel;
};
