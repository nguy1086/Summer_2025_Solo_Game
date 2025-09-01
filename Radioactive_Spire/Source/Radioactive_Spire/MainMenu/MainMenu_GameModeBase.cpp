// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu_GameModeBase.h"
#include "GameFramework/GameUserSettings.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

void AMainMenu_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

    UGameUserSettings* GameUserSettings = GEngine->GameUserSettings;
    if (GameUserSettings)
    {
        GameUserSettings->SetScreenResolution(FIntPoint(1280.0f, 720.0f));
        GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
        GameUserSettings->ApplyResolutionSettings(true);
        GameUserSettings->ApplySettings(true);
    }

    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->HandleToggleFullscreenCommand();
    }

    if (LevelSound != nullptr)
        LevelAudioComponent = UGameplayStatics::SpawnSound2D(this, LevelSound);
}
