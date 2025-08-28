// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RadioactiveSpire_GameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API ARadioactiveSpire_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ARadioactiveSpire_GameStateBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	float BlackOverlayAlpha;
};
