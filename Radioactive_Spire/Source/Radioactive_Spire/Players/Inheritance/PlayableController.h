// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayableController.generated.h"

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API APlayableController : public APlayerController
{
	GENERATED_BODY()
	
public:
	APlayableController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	virtual void SetupInputComponent() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DuckInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SpecialInputAction;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced input")
	//class UInputAction* UpInputAction;

	float GetMoveValue();
	bool IsJumpPressed();
	bool IsDuckPressed();
	bool IsAttackPressed();
	bool IsSpecialPressed();
	//bool IsUpPressed();

protected:
	void OnMove(const struct FInputActionValue& Value);
	void OnMoveReleased(const struct FInputActionValue& Value);

	void OnJumpPressed(const struct FInputActionValue& Value);
	void OnJumpReleased(const struct FInputActionValue& Value);

	void OnDuckPressed(const struct FInputActionValue& Value);
	void OnDuckReleased(const struct FInputActionValue& Value);

	void OnAttackPressed(const struct FInputActionValue& Value);

	void OnHeavyPressed(const struct FInputActionValue& Value);

	//void OnUpPressed(const struct FInputActionValue& Value);

private:
	class APlayableCharacter* PlayablePlayer;
	class APlayableCharacterState* PlayablePlayerState;
	class UEnhancedInputLocalPlayerSubsystem* InputSubsystem;
};
