// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Radioactive_Spire_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API URadioactive_Spire_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Volume")
	float MusicValue = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Volume")
	float SFXValue = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Volume")
	float AmbienceValue = 1.0f;
};
