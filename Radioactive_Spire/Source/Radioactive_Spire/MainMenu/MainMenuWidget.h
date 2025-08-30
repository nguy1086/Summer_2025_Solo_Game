// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UENUM(BlueprintType)
enum class EMainMenuState : uint8
{
	Unknown 	UMETA(DisplayName = "Unknown"),
	Intro		UMETA(DisplayName = "Intro"),
	MainMenu 	UMETA(DisplayName = "MainMenu"),
	Options		UMETA(DisplayName = "Options"),
	Character   UMETA(DisplayName = "Character"),
	Loading		UMETA(DisplayName = "Loading")
};
/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
};
