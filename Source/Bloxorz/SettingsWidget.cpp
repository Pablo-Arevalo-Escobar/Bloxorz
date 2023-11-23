// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/GameUserSettings.h"


void USettingsWidget::ApplySettings()
{
  UE_LOG(LogTemp,Warning,TEXT("APPLYING SETTINGS"));
  UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
  if(!ViewportClient)
  {
    UE_LOG(LogTemp,Warning, TEXT("SettingsWidget::ApplySettings Unable to access ViewportClient"));
    return;
  }

  if (FullscreenToggle->IsChecked())
  {
      GetWorld()->GetGameViewport()->HandleToggleFullscreenCommand();
      FullscreenToggle->SetIsChecked(!FullscreenToggle->IsChecked());
  }

  FString SelectedResolution = Resolution->GetSelectedOption();
  TArray<FString> NewResolution;
  SelectedResolution.ParseIntoArray(NewResolution, TEXT("x"),true);
  FIntPoint Res(FCString::Atoi(*NewResolution[0]), FCString::Atoi(*NewResolution[1]));
  GEngine->GameUserSettings->SetScreenResolution(Res);

}

void USettingsWidget::OnInitialized()
{
  Super::OnInitialized();
  if (!Resolution)
  {
      UE_LOG(LogTemp, Warning, TEXT("UNABLE TO LOAD RESOLUTION"));
      return;
  }
  UE_LOG(LogTemp,Warning,TEXT("Post load"));
  Resolution->AddOption("1920x1080");
  FullscreenToggle->SetIsChecked(GetWorld()->GetGameViewport()->IsFullScreenViewport());

}
