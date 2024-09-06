// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSelectWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Components/ComboBoxString.h"
#include <Components/Button.h>


void ULevelSelectWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (mCBLevelSelect)
    {
        FString RelativePath = FPaths::ProjectContentDir().Append("/Maps/MapFiles/");
        TArray<FString> Files;
        IFileManager::Get().FindFiles(Files, *RelativePath, TEXT("*.bloxlevel"));
        for (FString File : Files)
        {
            FString LevelName = FPaths::GetBaseFilename(File);
            mCBLevelSelect->AddOption(LevelName);
        }

        mCBLevelSelect->SetSelectedIndex(0);
    }

    if (mButtonLoadLevel)
    {
        mButtonLoadLevel->OnClicked.AddDynamic(this, &ULevelSelectWidget::OnButtonPressed);
        UE_LOG(LogTemp, Warning, TEXT("Button binding successful"));
    }
}

void ULevelSelectWidget::OnLevelLoaded()
{
    UE_LOG(LogTemp, Warning, TEXT("Level Loaded: %s"), *mCBLevelSelect->GetSelectedOption());
    FString Options = FString::Printf(TEXT("?LevelName=%s?"), *mCBLevelSelect->GetSelectedOption());
    UGameplayStatics::OpenLevel(this, TEXT("lvlCustom"), true, Options);
}

void ULevelSelectWidget::OnButtonPressed()
{
    // LOG selected level
    UE_LOG(LogTemp, Warning, TEXT("ATTEMPTING TO OPEN Selected Level: %s"), *mCBLevelSelect->GetSelectedOption());
    FLatentActionInfo LatentInfo;
    LatentInfo.CallbackTarget = this;
    LatentInfo.ExecutionFunction = "OnLevelLoaded";
    LatentInfo.Linkage = 0;
    LatentInfo.UUID = __LINE__;
    UGameplayStatics::LoadStreamLevel(this, *mCBLevelSelect->GetSelectedOption(), true, true, LatentInfo);
}
