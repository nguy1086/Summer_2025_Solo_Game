// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenu_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API AMainMenu_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainMenu_GameModeBase();
public:
	virtual void Tick(float DeltaTime) override;
};
