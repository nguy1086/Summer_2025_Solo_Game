// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

/**
 * 
 */
class UMainMenuWidget;
UCLASS()
class RADIOACTIVE_SPIRE_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainMenuController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* NavigationInputAction;//same as move

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SelectInputAction;//same as attack

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BackInputAction;// same as special

	float GetNavigationValue();
	bool IsSelectPressed();
	bool IsBackPressed();

	class UEnhancedInputLocalPlayerSubsystem* GetInputSubSystem() { return InputSubsystem; }

	UPROPERTY()
	UMainMenuWidget* GameInfoWidget;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMainMenuWidget> GameInfoBP;

protected:
	void OnNavigation(const struct FInputActionValue& Value);

	void OnSelectPressed(const struct FInputActionValue& Value);

	void OnBackPressed(const struct FInputActionValue& Value);

private:
	class AMainMenu_GameModeBase* GameModeBase;
	class UEnhancedInputLocalPlayerSubsystem* InputSubsystem;
};
