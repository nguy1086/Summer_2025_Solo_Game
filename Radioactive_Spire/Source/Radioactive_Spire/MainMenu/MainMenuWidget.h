// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UENUM(BlueprintType)
enum class EMainMenuState : uint8
{
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
	void UpdateIntro(float DeltaTime);
	void UpdateMainMenu();
	void UpdateOptions();
	void UpdateCharacterSelect(float DeltaTime, float speed = 48.0f);

	UFUNCTION()
	void OnCharacterSelect();
	UFUNCTION()
	void OnMainMenuOptions();
	UFUNCTION()
	void OnMainMenuQuit();

	void MainMenuNavigation(float dir);
	void MainMenuPressed();
	void MainMenuBackPressed();
	void MainMenuMoveWidget(float posx, float posy, float DeltaTime, float speed = 48.0f);

	UFUNCTION()
	void OnMusicChange(float v);
	UFUNCTION()
	void OnMusicPressed();
	UFUNCTION()
	void OnSFXChange(float v);
	UFUNCTION()
	void OnSFXPressed();
	UFUNCTION()
	void OnAmbienceChange(float v);
	UFUNCTION()
	void OnAmbiencePressed();

	UFUNCTION()
	void OnBatterSelect();

private:
	class URadioactive_Spire_GameInstance* GameInstance;
	void ResetIncrement();
	void UpdateMainSoundChange();
	class AMainMenu_GameModeBase* GameModeBase;
	EMainMenuState State;
	TArray<class UButton*> MainMenuButtons;
	TArray<class USlider*> MainVolumeSliders;
	TArray<class UButton*> MainVolumeButtons;
	TArray<class UButton*> CharacterSelections;
	TArray<class UImage*> CharacterScreenAnimate;
	int Increment;
	int OptionIncrement;
	int CharacterIncrement;
	float FadeTimer;
	float CharacterScreenAnimateTimer;
	int CharacterScreenAnimateIncrement;
};
