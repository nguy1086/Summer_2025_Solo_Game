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
	void DisplayGameOver();

	UFUNCTION()
	void OnResume();
	UFUNCTION()
	void OnOptions();
	UFUNCTION()
	void OnRetry();
	UFUNCTION()
	void OnQuit();

	UFUNCTION()
	void SetIncrement(int i);

	void PauseMenuNavigation(float dir);
	void PauseMenuPressed();

private:
	TArray<class UButton*> PauseButtons;
	int Increment;
};
