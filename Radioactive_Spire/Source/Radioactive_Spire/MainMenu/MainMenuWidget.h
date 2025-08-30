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

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void UpdateIntro();
	void UpdateMainMenu();
	void UpdateOptions();
	void UpdateCharacterSelect();

	UFUNCTION()
	void OnCharacterSelect();
	UFUNCTION()
	void OnMainMenuOptions();
	UFUNCTION()
	void OnMainMenuQuit();

	void MainMenuNavigation(float dir);
	void MainMenuPressed();
	void MainMenuBackPressed();
	void MoveWidget(float posx, float posy, float DeltaTime, float speed = 48.0f);

private:
	class AMainMenu_GameModeBase* GameModeBase;
	EMainMenuState State;
	TArray<class UButton*> MainMenuButtons;
	int Increment;
};
