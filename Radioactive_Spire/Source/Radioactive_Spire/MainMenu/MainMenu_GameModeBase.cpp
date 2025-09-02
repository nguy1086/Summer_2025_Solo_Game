// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu_GameModeBase.h"
#include "GameFramework/GameUserSettings.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "../MainMenu/Radioactive_Spire_GameInstance.h"

void AMainMenu_GameModeBase::BeginPlay()
{
    Super::BeginPlay();
    GameInstance = Cast<URadioactive_Spire_GameInstance>(GetGameInstance());

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

    //if (GameInstance && GameInstance->MainMenuMusic != nullptr)
    //    LevelAudioComponent = UGameplayStatics::SpawnSound2D(this, GameInstance->MainMenuMusic);
    if (GameInstance->MainMenuMusic != nullptr)
        UGameplayStatics::PlaySoundAtLocation(this, GameInstance->MainMenuMusic, FVector());
}
