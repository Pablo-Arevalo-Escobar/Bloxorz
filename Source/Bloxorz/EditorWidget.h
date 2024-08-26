// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/CanvasPanel.h"
#include "Components/SpinBox.h"
#include "Components/Button.h"
#include "EditorWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLOXORZ_API UEditorWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    // UMG Widgets 
    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UCanvasPanel* CanvasPanel;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UComboBoxString* TileTypeUsing;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class USpinBox* ResolutionSpinBox;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UButton* GridResolutionButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UButton* FillButton;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UButton* EmptyButton;

    UPROPERTY(EditAnywhere, meta = (BindWidget))
    class UButton* SerializeButton;
};
