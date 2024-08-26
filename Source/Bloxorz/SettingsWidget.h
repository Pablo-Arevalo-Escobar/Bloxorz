// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "SettingsWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable)
    void ApplySettings();

    void OnInitialized();

public:
    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UComboBoxString* Resolution;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UCheckBox* FullscreenToggle;
};
