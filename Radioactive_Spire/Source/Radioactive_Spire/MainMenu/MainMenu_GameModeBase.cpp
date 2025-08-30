// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu_GameModeBase.h"

void AMainMenu_GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Tick"));
}
