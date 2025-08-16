// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableController.h"
#include "PlayableCharacter.h"
#include "PlayableCharacterState.h"
//#include "PlatformerGameModeBase.h"
//#include "PlatformerGameStateBase.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"



APlayableController::APlayableController() :
	InputMappingContext(nullptr),
	MoveInputAction(nullptr),
	JumpInputAction(nullptr),
	DuckInputAction(nullptr),
	//UpInputAction(nullptr),
	PlayablePlayer(nullptr),
	PlayablePlayerState(nullptr),
	InputSubsystem(nullptr)
{
	Tags.Add("Controller");
}

void APlayableController::BeginPlay()
{
	Super::BeginPlay();

	PlayablePlayerState = Cast<APlayableCharacterState>(PlayerState);

	InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (InputSubsystem != nullptr)
		InputSubsystem->AddMappingContext(InputMappingContext, 0);
}

void APlayableController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	PlayablePlayer = Cast<APlayableCharacter>(aPawn);
}

void APlayableController::OnUnPossess()
{
	Super::OnUnPossess();

	PlayablePlayer = nullptr;
}

void APlayableController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent != nullptr)
	{
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &APlayableController::OnMove);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Completed, this, &APlayableController::OnMoveReleased);

		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &APlayableController::OnJumpPressed);
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &APlayableController::OnJumpReleased);

		EnhancedInputComponent->BindAction(DuckInputAction, ETriggerEvent::Started, this, &APlayableController::OnDuckPressed);
		EnhancedInputComponent->BindAction(DuckInputAction, ETriggerEvent::Completed, this, &APlayableController::OnDuckReleased);

		EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &APlayableController::OnAttackPressed);

		EnhancedInputComponent->BindAction(SpecialInputAction, ETriggerEvent::Started, this, &APlayableController::OnSpecialPressed);

		EnhancedInputComponent->BindAction(RollInputAction, ETriggerEvent::Started, this, &APlayableController::OnRollPressed);

		//EnhancedInputComponent->BindAction(UpInputAction, ETriggerEvent::Started, this, &APlayableController::OnUpPressed);
	}
}

float APlayableController::GetMoveValue()
{
	if (InputSubsystem != nullptr)
	{
		FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(MoveInputAction);
		return InputActionValue.Get<float>();
	}
	return 0.0f;
}

bool APlayableController::IsJumpPressed()
{
	if (InputSubsystem != nullptr)
	{
		FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(JumpInputAction);
		return InputActionValue.Get<bool>();
	}
	return false;
}

bool APlayableController::IsDuckPressed()
{
	if (InputSubsystem != nullptr)
	{
		FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(DuckInputAction);
		return InputActionValue.Get<bool>();
	}
	return false;
}

bool APlayableController::IsAttackPressed()
{
	if (InputSubsystem != nullptr)
	{
		FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(AttackInputAction);
		return InputActionValue.Get<bool>();
	}
	return false;
}

bool APlayableController::IsSpecialPressed()
{
	if (InputSubsystem != nullptr)
	{
		FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(SpecialInputAction);
		return InputActionValue.Get<bool>();
	}
	return false;
}

//bool APlayableController::IsUpPressed()
//{
//	if (InputSubsystem != nullptr)
//	{
//		FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(UpInputAction);
//		return InputActionValue.Get<bool>();
//	}
//	return false;
//}

void APlayableController::OnMove(const FInputActionValue& Value)
{
	float Direction = Value.Get<float>();

	if (PlayablePlayer != nullptr && PlayablePlayerState != nullptr)
	{
		if (PlayablePlayerState->State != EState::Ducking && 
			PlayablePlayerState->State != EState::Special && 
			PlayablePlayerState->State != EState::Attacking &&
			PlayablePlayerState->State != EState::Roll)
		{
			if (PlayablePlayerState->IsOnGround)
				PlayablePlayer->ApplyStateChange(EState::Walking);

			PlayablePlayer->AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Direction);

			if (Direction < 0.0f)
			{
				SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
				PlayablePlayerState->Direction = EDirection::Left;
			}
			else if (Direction > 0.0f)
			{
				SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
				PlayablePlayerState->Direction = EDirection::Right;
			}
		}
		else if (PlayablePlayerState->State == EState::Attacking)//able to turn when attacking
		{
			if (Direction < 0.0f)
			{
				SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
				PlayablePlayerState->Direction = EDirection::Left;
			}
			else if (Direction > 0.0f)
			{
				SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
				PlayablePlayerState->Direction = EDirection::Right;
			}
		}
	}
}

void APlayableController::OnMoveReleased(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && PlayablePlayerState != nullptr)
		if (PlayablePlayerState->IsOnGround && 
			PlayablePlayerState->State != EState::Ducking && 
			PlayablePlayerState->State != EState::Special &&
			PlayablePlayerState->State != EState::Attacking &&
			PlayablePlayerState->State != EState::Roll)
			PlayablePlayer->ApplyStateChange(EState::Idle);
}

void APlayableController::OnJumpPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr)
		PlayablePlayer->Jump();
}

void APlayableController::OnJumpReleased(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr)
		PlayablePlayer->StopJumping();
}

void APlayableController::OnDuckPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr)
		PlayablePlayer->Duck();
}

void APlayableController::OnDuckReleased(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr)
		PlayablePlayer->StopDucking();
}

void APlayableController::OnAttackPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr)
		PlayablePlayer->Attack();
}

void APlayableController::OnSpecialPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr)
		PlayablePlayer->Special();
}

void APlayableController::OnRollPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr)
		PlayablePlayer->Roll();
}

//void APlayableController::OnUpPressed(const FInputActionValue& Value)
//{
//}
