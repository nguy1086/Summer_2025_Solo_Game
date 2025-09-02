// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayableWidget.generated.h"

UENUM(BlueprintType)
enum class EPauseMenuState : uint8
{
	PauseMenu		UMETA(DisplayName = "PauseMenu"),
	OptionsMenu 	UMETA(DisplayName = "OptionsMenu")
};
/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API UPlayableWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void UpdateHealth();
	void UpdateSuper();
	void UpdateEnemies();
	void UpdatePause();

	void FadeEnter(float DeltaTime);
	void FadeQuit(float DeltaTime);
	void FadeRetry(float DeltaTime);

	void HideAllButGameOver();
	void DisplayGameOver();

	UFUNCTION()
	void OnResume();
	UFUNCTION()
	void OnPauseOptions();
	UFUNCTION()
	void OnRetry();
	UFUNCTION()
	void OnPauseQuit();
	UFUNCTION()
	void OnPauseFadeToQuit();
	UFUNCTION()
	void OnPauseFadeToRetry();

	void PauseMenuNavigation(float dir);
	void PauseMenuPressed();
	void PauseMenuBackPressed();
	void PauseMoveWidget(float posx, float posy, float DeltaTime, float speed = 48.0f);

	UFUNCTION()
	void OnPauseVolumeChange(float v);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float FadeTimer = 2.5f;

private:
	void UpdatePauseSoundChange();
	class URadioactive_Spire_GameInstance* GameInstance;
	EPauseMenuState State;
	TArray<class UButton*> PauseButtons;
	TArray<class UButton*> GameOverButtons;
	TArray<class USlider*> PauseVolumeSliders;
	int Increment;
	int GameOverIncrement;
	float GameOverDelay;
	class ARadioactiveSpire_GameModeBase* GameModeBase;
};
