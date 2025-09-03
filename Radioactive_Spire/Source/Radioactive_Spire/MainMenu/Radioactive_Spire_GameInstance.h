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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu SFX")
	class USoundBase* SelectSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu SFX")
	class USoundBase* NavigationSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu SFX")
	class USoundBase* CancelSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Menu Music")
	class USoundBase* MainMenuMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Music")
	class USoundBase* LevelMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	class USoundClass* Music;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
	class USoundClass* SFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ambience")
	class USoundClass* Ambience;

};
