// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "PlayableCharacterState.h"
#include "PlayableController.h"
#include "PlayerCamera.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../../Game/RadioactiveSpire_GameModeBase.h"

APlayableCharacter::APlayableCharacter() :
    PlayerState(nullptr),
    Camera(nullptr)
{
    PrimaryActorTick.bCanEverTick = true;

    //collision
    GetCapsuleComponent()->SetCollisionProfileName("Player");
    GetCapsuleComponent()->SetCapsuleRadius(14.0f);
    GetCapsuleComponent()->SetCapsuleHalfHeight(25.0f);

    //ground
    GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->MaxStepHeight = PlayerConstants::DefaultMaxStepHeight;
    GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 1.0f, 0.0f));

    //jump
    GetCharacterMovement()->GravityScale = PlayerConstants::DefaultGravityScale;
    GetCharacterMovement()->AirControl = PlayerConstants::DefaultAirControl;
    GetCharacterMovement()->JumpZVelocity = PlayerConstants::DefaultJumpZVelocity;
    JumpMaxHoldTime = 0.15f;

    //walking
    GetCharacterMovement()->GroundFriction = PlayerConstants::DefaultGroundFriction;
    GetCharacterMovement()->MaxWalkSpeed = PlayerConstants::DefaultMaxSpeed;
    GetCharacterMovement()->MaxAcceleration = PlayerConstants::DefaultMaxAcceleration;
    GetCharacterMovement()->MinAnalogWalkSpeed = PlayerConstants::DefaultMinSpeed;

    Tags.Add("Player");
}

void APlayableCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerState = GetPlayerState<APlayableCharacterState>();

    // restart the Character before spawning
    Restart();
}

void APlayableCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	UpdateFlipbook();

	if (PlayerState != nullptr)
	{
		// ensure that the player isn't dead
		if (PlayerState->State != EState::Dead)
		{
			// if player falls of the edge,no longer on ground, movement component 
			// catches this situation, update the IsOnGround value
			if (GetMovementComponent()->IsFalling() && PlayerState->IsOnGround)
				PlayerState->IsOnGround = false;

			// handle the situation where player is invincible, this 
			// occurs right after player takes damage
			if (IsInvincible())
			{
				PlayerState->InvincibilityTimer -= DeltaTime;

				if (PlayerState->InvincibilityTimer <= 0.0f)
				{
					PlayerState->InvincibilityTimer = 0.0f;
					GetCapsuleComponent()->SetCollisionProfileName("Player");

					GetSprite()->SetVisibility(true);
				}
				else
				{
					DamagedTimer -= DeltaTime;
					if (DamagedTimer <= 0.0f)
					{
						DamagedTimer = PlayerConstants::DefaultInvincibleVisibilityDuration;
						GetSprite()->ToggleVisibility();
					}
				}
			}

			if (GetActorLocation().Z < -250.0f)
				Death(false);
		}
	}
}

void APlayableCharacter::Duck()
{
	if (PlayerState->IsOnGround)
		ApplyStateChange(EState::Ducking);
}

void APlayableCharacter::StopDucking()
{
	if (PlayerState->IsOnGround)
	{
		APlayableController* controller = Cast<APlayableController>(Controller);
		if (controller)
		{
			if (controller->GetMoveValue() == 0.0f)
				ApplyStateChange(EState::Idle);
			else
				ApplyStateChange(EState::Walking);
		}
	}
}

void APlayableCharacter::Attack()
{

}

void APlayableCharacter::ApplyStateChange(EState newState)
{
	if (!PlayerState)
		return;

	if (PlayerState->State == newState)
		return;

	EState old = PlayerState->State;
	PlayerState->State = newState;

	if (newState == EState::Jumping)
		GetCharacterMovement()->bNotifyApex = true;

	if (newState == EState::Ducking)
	{

		GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::DefaultCapsuleRadius);
		GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, 63.0f));//half height - 12

		FVector location = GetActorLocation();
		location.Z -= 47.0f;//half height - radius
		SetActorLocation(location);
	}
	else
	{
		if (old == EState::Ducking)
		{
			GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::DefaultCapsuleHalfHeight);
			GetSprite()->SetRelativeLocation(FVector(0.0f, 0.0f, 17.0f));//17 is offset to center

			FVector location = GetActorLocation();
			location.Z += 47.0f;//half height - radius
			SetActorLocation(location);
		}
	}

	UpdateFlipbook();
}

void APlayableCharacter::ApplyBounce()
{
	LaunchCharacter(FVector(0.0f, 0.0f, 600.0f), false, true);
	GetPlayerState<APlayableCharacterState>()->IsOnGround = false;
}

void APlayableCharacter::HandleDamage(float damage)
{
	if (PlayerState && !IsInvincible())
	{
		Health -= damage;
		if (Health <= 0)
		{
			DamagedTimer = PlayerConstants::DefaultInvincibleVisibilityDuration;
			PlayerState->InvincibilityTimer = PlayerConstants::DefaultInvincibleTime;
			GetCapsuleComponent()->SetCollisionProfileName("Invincible");
			Death(true);
		}
		else
		{
			DamagedTimer = PlayerConstants::DefaultInvincibleVisibilityDuration;
			PlayerState->InvincibilityTimer = PlayerConstants::DefaultInvincibleTime;
			GetCapsuleComponent()->SetCollisionProfileName("Invincible");
		}
	}
}

bool APlayableCharacter::IsInvincible()
{
	return GetPlayerState<APlayableCharacterState>()->InvincibilityTimer > 0.0f;
}

void APlayableCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();

	if (PlayerState)
		PlayerState->IsOnGround = false;

	ApplyStateChange(EState::Jumping);
}

void APlayableCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();

	if (PlayerState && !PlayerState->IsOnGround)
		ApplyStateChange(EState::Falling);
}

void APlayableCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (PlayerState)
	{
		PlayerState->IsOnGround = true;

		APlayableController* controller = Cast<APlayableController>(Controller);
		if (controller != nullptr)
		{
			if (controller->IsDuckPressed())
			{
				ApplyStateChange(EState::Ducking);
			}
			else
			{
				if (controller->GetMoveValue() == 0.0f)
					ApplyStateChange(EState::Idle);
				else
					ApplyStateChange(EState::Walking);
			}
		}
	}
}

void APlayableCharacter::UpdateFlipbook()
{
	if (!PlayerState)
		return;

	UPaperFlipbook* currentFlipbook = GetSprite()->GetFlipbook();
	UPaperFlipbook* newFlipbook = nullptr;

	if (Type == EPlayerType::Test)
	{
		newFlipbook = GetTestFlipbook();
	}

	// safety check that the new flipbook is not null and different than the current flipbook
	if (currentFlipbook != newFlipbook && newFlipbook)
	{
		GetSprite()->SetFlipbook(newFlipbook);
		GetSprite()->PlayFromStart();

		GetSprite()->SetPlayRate(1.0f);
	}
}

void APlayableCharacter::Death(bool spawnDeathAnimation)
{
	if (PlayerState->State != EState::Dead)
	{
		DamagedTimer = PlayerConstants::DefaultInvincibleVisibilityDuration;
		PlayerState->InvincibilityTimer = PlayerConstants::DefaultInvincibleTime;

		ApplyStateChange(EState::Dead);

		SetActorHiddenInGame(true);

		ARadioactiveSpire_GameModeBase* gameMode = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
		if (gameMode)
			gameMode->PlayerDied();
	}
}

UPaperFlipbook* APlayableCharacter::GetTestFlipbook()
{
	if (!PlayerState)
		return nullptr;

	UPaperFlipbook* flipbook = nullptr;

	if (PlayerState->State == EState::Idle)
		flipbook = TestIdleFlipbook;
	else if (PlayerState->State == EState::Walking)
		flipbook = TestWalkFlipbook;
	else if (PlayerState->State == EState::Jumping)
		flipbook = TestJumpFlipbook;
	else if (PlayerState->State == EState::Falling)
		flipbook = TestFallingFlipbook;
	else if (PlayerState->State == EState::Ducking)
		flipbook = TestDuckFlipbook;

	return flipbook;
}
