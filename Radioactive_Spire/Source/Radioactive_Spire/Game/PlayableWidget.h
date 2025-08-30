// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayableWidget.generated.h"

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

	void PauseMenuNavigation(float dir);
	void PauseMenuPressed();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float FadeTimer = 2.5f;

private:
	TArray<class UButton*> PauseButtons;
	int Increment;
	class ARadioactiveSpire_GameModeBase* GameModeBase;
};
