// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableController.h"
#include "PlayableCharacter.h"
#include "PlayableCharacterState.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../../Game/PlayableWidget.h"
#include "../../Game/RadioactiveSpire_GameModeBase.h"


APlayableController::APlayableController() :
	InputMappingContext(nullptr),
	MoveInputAction(nullptr),
	JumpInputAction(nullptr),
	DuckInputAction(nullptr),
	//UpInputAction(nullptr),
	AttackInputAction(nullptr),
	SpecialInputAction(nullptr),
	RollInputAction(nullptr),
	SuperInputAction(nullptr),
	PauseInputAction(nullptr),
	PauseButtonNavigation(nullptr),
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

	if (GameInfoBP != nullptr && IsLocalController())
	{
		GameInfoWidget = CreateWidget<UPlayableWidget>(this, GameInfoBP);
		GameInfoWidget->AddToViewport();
	}
	GameModeBase = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
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

		EnhancedInputComponent->BindAction(DuckInputAction, ETriggerEvent::Triggered, this, &APlayableController::OnDuckPressed);
		EnhancedInputComponent->BindAction(DuckInputAction, ETriggerEvent::Completed, this, &APlayableController::OnDuckReleased);

		EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &APlayableController::OnAttackPressed);

		EnhancedInputComponent->BindAction(SpecialInputAction, ETriggerEvent::Started, this, &APlayableController::OnSpecialPressed);

		EnhancedInputComponent->BindAction(RollInputAction, ETriggerEvent::Started, this, &APlayableController::OnRollPressed);

		EnhancedInputComponent->BindAction(SuperInputAction, ETriggerEvent::Started, this, &APlayableController::OnSuperPressed);

		EnhancedInputComponent->BindAction(PauseInputAction, ETriggerEvent::Started, this, &APlayableController::OnPausePressed);

		EnhancedInputComponent->BindAction(PauseButtonNavigation, ETriggerEvent::Started, this, &APlayableController::OnPauseButtonNavigation);

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

bool APlayableController::IsRollPressed()
{
	if (InputSubsystem != nullptr)
	{
		FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(RollInputAction);
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

	if (PlayablePlayer != nullptr && PlayablePlayerState != nullptr && GameModeBase != nullptr)
	{
		if (!GameModeBase->Game_IsPaused && 
			GameModeBase->State != EGameState::EndGame && 
			GameModeBase->State != EGameState::FadeToQuit && 
			GameModeBase->State != EGameState::FadeToRetry)
		{
			if (PlayablePlayerState->State != EState::Ducking &&
				PlayablePlayerState->State != EState::Special &&
				PlayablePlayerState->State != EState::Attacking &&
				PlayablePlayerState->State != EState::Roll &&
				PlayablePlayerState->State != EState::Hurt &&
				PlayablePlayerState->State != EState::Super)
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
}

void APlayableController::OnMoveReleased(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && PlayablePlayerState != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && 
			GameModeBase->State != EGameState::EndGame && 
			GameModeBase->State != EGameState::FadeToQuit && 
			GameModeBase->State != EGameState::FadeToRetry)
			if (PlayablePlayerState->IsOnGround && 
				PlayablePlayerState->State != EState::Ducking && 
				PlayablePlayerState->State != EState::Special &&
				PlayablePlayerState->State != EState::Attacking &&
				PlayablePlayerState->State != EState::Roll &&
				PlayablePlayerState->State != EState::Hurt &&
				PlayablePlayerState->State != EState::Super)
				PlayablePlayer->ApplyStateChange(EState::Idle);
}

void APlayableController::OnJumpPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && 
			GameModeBase->State != EGameState::EndGame && 
			GameModeBase->State != EGameState::FadeToQuit && 
			GameModeBase->State != EGameState::FadeToRetry)
				PlayablePlayer->Jump();
}

void APlayableController::OnJumpReleased(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && 
			GameModeBase->State != EGameState::EndGame && 
			GameModeBase->State != EGameState::FadeToQuit && 
			GameModeBase->State != EGameState::FadeToRetry)
				PlayablePlayer->StopJumping();
}

void APlayableController::OnDuckPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && 
			GameModeBase->State != EGameState::EndGame && 
			GameModeBase->State != EGameState::FadeToQuit && 
			GameModeBase->State != EGameState::FadeToRetry)
				PlayablePlayer->Action_Duck();

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Start Ducking!"));
}

void APlayableController::OnDuckReleased(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && 
			GameModeBase->State != EGameState::EndGame && 
			GameModeBase->State != EGameState::FadeToQuit && 
			GameModeBase->State != EGameState::FadeToRetry)
				PlayablePlayer->Action_StopDucking();

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Stop Ducking!"));
}

void APlayableController::OnAttackPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && GameModeBase->State != EGameState::EndGame && GameModeBase->State != EGameState::FadeToQuit && GameModeBase->State != EGameState::FadeToRetry)
			PlayablePlayer->Action_Attack();
		else
			if (GameInfoWidget)
				GameInfoWidget->PauseMenuPressed();

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Attack!"));
}

void APlayableController::OnSpecialPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && GameModeBase->State != EGameState::EndGame && GameModeBase->State != EGameState::FadeToQuit && GameModeBase->State != EGameState::FadeToRetry)
			PlayablePlayer->Action_Special();
		else
			if (GameInfoWidget)
				GameInfoWidget->PauseMenuBackPressed();

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Special!"));
}

void APlayableController::OnRollPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && GameModeBase->State != EGameState::EndGame && GameModeBase->State != EGameState::FadeToQuit && GameModeBase->State != EGameState::FadeToRetry)
			PlayablePlayer->Action_Roll();

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Roll!"));
}

void APlayableController::OnSuperPressed(const FInputActionValue& Value)
{
	if (PlayablePlayer != nullptr && GameModeBase != nullptr)
		if (!GameModeBase->Game_IsPaused && GameModeBase->State != EGameState::EndGame)
			PlayablePlayer->Action_Super();
}

void APlayableController::OnPausePressed(const FInputActionValue& Value)
{
	if (GameModeBase != nullptr)
		GameModeBase->GamePause();
}

void APlayableController::OnPauseButtonNavigation(const FInputActionValue& Value)
{
	float Direction = Value.Get<float>();
	if (GameModeBase->Game_IsPaused || GameModeBase->State == EGameState::EndGame)
		if (GameInfoWidget)
			GameInfoWidget->PauseMenuNavigation(Direction);
}

//void APlayableController::OnUpPressed(const FInputActionValue& Value)
//{
//}
