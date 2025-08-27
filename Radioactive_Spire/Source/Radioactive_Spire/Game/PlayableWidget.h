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
	void UpdatePause();
	void DisplayGameOver();
};
