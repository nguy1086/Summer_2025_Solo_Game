// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "PlayableCharacterState.h"
#include "PlayableController.h"
#include "PlayerCamera.h"
#include "PlayableAttackHitbox.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../../Game/RadioactiveSpire_GameModeBase.h"

APlayableCharacter::APlayableCharacter() :
    PlayerState(nullptr),
    Camera(nullptr),
	PlayableController(nullptr),
	DamagedTimer(0.0f)
{
    PrimaryActorTick.bCanEverTick = true;

	//InitializeType();

	//DEBUG
	GetCapsuleComponent()->SetHiddenInGame(false);

    Tags.Add("Player");
}

void APlayableCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerState = GetPlayerState<APlayableCharacterState>();
	PlayableController = Cast<APlayableController>(Controller);

	InitializeType();

	if (Type == EPlayerType::Test)
		GetSprite()->SetRelativeLocation(PlayerConstants::DefaultSpriteOffset);
	else if (Type == EPlayerType::Batter)
		GetSprite()->SetRelativeLocation(PlayerConstants::BatterSpriteOffset);

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
				Death();
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
		if (PlayableController)
		{
			if (PlayableController->IsSpecialPressed() || PlayerState->State == EState::Special)//make sures crouch doesnt interfere with special attack animation
				ApplyStateChange(EState::Special);
			else if (PlayableController->GetMoveValue() == 0.0f)
				ApplyStateChange(EState::Idle);
			else
				ApplyStateChange(EState::Walking);
		}
	}
}

void APlayableCharacter::Attack()
{
	if (Type == EPlayerType::Test)
	{
		FVector location = GetActorLocation();
		FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);
		if (PlayerState->Direction == EDirection::Left)
		{
			location.X -= 32.0f;
			rotation = FRotator(0.0f, 180.0f, 0.0f);
		}
		else if (PlayerState->Direction == EDirection::Right)
			location.X += 32.0f;

		APlayableAttackHitbox* hitbox = GetWorld()->SpawnActor<APlayableAttackHitbox>(AttackHitboxTemplate, location, rotation);
		hitbox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		hitbox->Spawn(TEXT("Test_Basic"), PlayerConstants::DefaultDamage);
	}
	else if (Type == EPlayerType::Batter)
	{
		if (AttackHitboxTemplate)
		{
			DisableControls();
			StopDucking();
			ApplyStateChange(EState::Attacking);


			float FramesPerSecond = GetSprite()->GetFlipbook()->GetFramesPerSecond();
			float TotalDuration = GetSprite()->GetFlipbookLengthInFrames();
			//float DesiredFrame = (TotalDuration - 3.0f) / FramesPerSecond;//get total frame subtract to the frame you want to spawn in,
			//i want to spawn at frame 4, so 7 (total) - 3 = 4 divide by the fps, or literally just setting it to the exact frame instead of subtracting lol
			float Delay = ComboNumber == 1 || ComboNumber == 3 ? 4.0f : 5.0f;
			Delay = ComboNumber >= PlayerConstants::BatterMaxCombo ? 5.0f : Delay;

			GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APlayableCharacter::BatterComboAttackSpawn, ((TotalDuration - Delay) / FramesPerSecond), false);
			GetWorldTimerManager().SetTimer(InputTimerHandle, this, &APlayableCharacter::EnableControls, ((TotalDuration - Delay + (ComboNumber >= PlayerConstants::BatterMaxCombo ? 4.0f : 0.0f)) / FramesPerSecond), false);
			GetWorldTimerManager().SetTimer(StateTimerHandle, this, &APlayableCharacter::ResetPlayerState, (TotalDuration / FramesPerSecond), false);
		}																						//total frames / fps to get the total frame duration
	}
}

void APlayableCharacter::Special()
{
	if (Type == EPlayerType::Batter)
	{
		if (PlayerState->State != EState::Special)
		{
			if (AttackHitboxTemplate)
			{
				DisableControls();
				StopDucking();
				ApplyStateChange(EState::Special);

				float FramesPerSecond = GetSprite()->GetFlipbook()->GetFramesPerSecond();
				float TotalDuration = GetSprite()->GetFlipbookLengthInFrames();

				GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &APlayableCharacter::BatterSpecialSpawn, (1.0f / FramesPerSecond), false);
				GetWorldTimerManager().SetTimer(InputTimerHandle, this, &APlayableCharacter::EnableControls, (1.0f / FramesPerSecond), false);
				GetWorldTimerManager().SetTimer(StateTimerHandle, this, &APlayableCharacter::ResetPlayerState, (TotalDuration / FramesPerSecond), false);
			}
		}
	}
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

	if (Type == EPlayerType::Test)
	{
		if (newState == EState::Ducking)
		{

			GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::DefaultCapsuleRadius);
			GetSprite()->SetRelativeLocation(PlayerConstants::DefaultSpriteCrouchOffset);//half height - 12

			FVector location = GetActorLocation();
			location.Z -= 47.0f;//half height - radius
			SetActorLocation(location);
		}
		else
		{
			if (old == EState::Ducking)
			{
				GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::DefaultCapsuleHalfHeight);
				GetSprite()->SetRelativeLocation(PlayerConstants::DefaultSpriteOffset);//17 is offset to center

				FVector location = GetActorLocation();
				location.Z += 47.0f;//half height - radius
				SetActorLocation(location);
			}
		}
	}
	else if (Type == EPlayerType::Batter)
	{
		if (newState == EState::Ducking)
		{

			GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::BatterCapsuleRadius);
			GetSprite()->SetRelativeLocation(PlayerConstants::BatterSpriteCrouchOffset);//half height - 12

			FVector location = GetActorLocation();
			location.Z -= 47.0f;//half height - radius
			SetActorLocation(location);
		}
		else
		{
			if (old == EState::Ducking)
			{
				GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::BatterCapsuleHalfHeight);
				GetSprite()->SetRelativeLocation(PlayerConstants::BatterSpriteOffset);//17 is offset to center

				FVector location = GetActorLocation();
				location.Z += 47.0f;//half height - radius
				SetActorLocation(location);
			}
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
			Death();
		}
		else
		{
			DamagedTimer = PlayerConstants::DefaultInvincibleVisibilityDuration;
			PlayerState->InvincibilityTimer = PlayerConstants::DefaultInvincibleTime;
			GetCapsuleComponent()->SetCollisionProfileName("Invincible");
			ResetPlayerState();
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

		if (PlayableController && 
			PlayerState->State != EState::Special &&
			PlayerState->State != EState::Attacking)//make sures the launchcharacter() doesnt reset the frames when attack/special
		{
			if (PlayableController->IsDuckPressed())
			{
				ApplyStateChange(EState::Ducking);
			}
			else
			{
				if (PlayableController->GetMoveValue() == 0.0f)
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
	else if (Type == EPlayerType::Batter)
	{
		newFlipbook = GetBatterFlipbook();
	}

	// safety check that the new flipbook is not null and different than the current flipbook
	if (currentFlipbook != newFlipbook && newFlipbook)
	{
		GetSprite()->SetFlipbook(newFlipbook);
		GetSprite()->PlayFromStart();

		GetSprite()->SetPlayRate(1.0f);
	}
}

void APlayableCharacter::Death()
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
	else if (PlayerState->State == EState::Attacking)
		flipbook = TestAttackFlipbook;

	return flipbook;
}

UPaperFlipbook* APlayableCharacter::GetBatterFlipbook()
{
	if (!PlayerState)
		return nullptr;

	UPaperFlipbook* flipbook = nullptr;

	if (PlayerState->State == EState::Idle)
		flipbook = BatterIdleFlipbook;
	else if (PlayerState->State == EState::Walking)
		flipbook = BatterWalkFlipbook;
	else if (PlayerState->State == EState::Jumping)
		flipbook = BatterJumpFlipbook;
	else if (PlayerState->State == EState::Falling)
		flipbook = BatterFallingFlipbook;
	else if (PlayerState->State == EState::Ducking)
		flipbook = BatterDuckFlipbook;
	else if (PlayerState->State == EState::Attacking)
		if ((ComboNumber == 0 || ComboNumber == 2) && PlayableController->IsAttackPressed())
			flipbook = BatterAttackOneFlipbook;
		else if ((ComboNumber == 1 || ComboNumber == 3) && PlayableController->IsAttackPressed())
			flipbook = BatterAttackTwoFlipbook;
		else if(ComboNumber >= PlayerConstants::BatterMaxCombo && PlayableController->IsAttackPressed())
			flipbook = BatterFinisherFlipbook;
	else if (PlayerState->State == EState::Special)
		flipbook = BatterSpecialFlipbook;

	return flipbook;
}

void APlayableCharacter::InitializeType()
{
	if (Type == EPlayerType::Test)
	{
		Health = PlayerConstants::DefaultHealth;
		//collision
		GetCapsuleComponent()->SetCollisionProfileName("Entity");
		GetCapsuleComponent()->SetCapsuleRadius(PlayerConstants::DefaultCapsuleRadius);
		GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::DefaultCapsuleHalfHeight);

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

		Tags.Add("Test");
	}
	else if (Type == EPlayerType::Batter)
	{
		Health = PlayerConstants::BatterHealth;
		//collision
		GetCapsuleComponent()->SetCollisionProfileName("Entity");
		GetCapsuleComponent()->SetCapsuleRadius(PlayerConstants::BatterCapsuleRadius);
		GetCapsuleComponent()->SetCapsuleHalfHeight(PlayerConstants::BatterCapsuleHalfHeight);

		//ground
		GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
		GetCharacterMovement()->bConstrainToPlane = true;
		GetCharacterMovement()->MaxStepHeight = PlayerConstants::BatterMaxStepHeight;
		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, 1.0f, 0.0f));

		//jump
		GetCharacterMovement()->GravityScale = PlayerConstants::BatterGravityScale;
		GetCharacterMovement()->AirControl = PlayerConstants::BatterAirControl;
		GetCharacterMovement()->JumpZVelocity = PlayerConstants::BatterJumpZVelocity;
		JumpMaxHoldTime = 0.15f;

		//walking
		GetCharacterMovement()->GroundFriction = PlayerConstants::BatterGroundFriction;
		GetCharacterMovement()->MaxWalkSpeed = PlayerConstants::BatterMaxSpeed;
		GetCharacterMovement()->MaxAcceleration = PlayerConstants::BatterMaxAcceleration;
		GetCharacterMovement()->MinAnalogWalkSpeed = PlayerConstants::BatterMinSpeed;

		Tags.Add("Batter");
	}
}

void APlayableCharacter::EnableControls()
{
	ARadioactiveSpire_GameModeBase* gameMode = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
	if (gameMode)
		gameMode->EnableControls();
}

void APlayableCharacter::DisableControls()
{
	ARadioactiveSpire_GameModeBase* gameMode = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
	if (gameMode)
		gameMode->DisableControls();
}

void APlayableCharacter::ResetPlayerState()
{
	if (PlayableController)
	{
		if (PlayableController->GetMoveValue() == 0.0f)
			ApplyStateChange(EState::Idle);
		else if (!PlayerState->IsOnGround)
			ApplyStateChange(EState::Falling);
		else
			ApplyStateChange(EState::Walking);

		ComboNumber = 0;
		GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		GetWorldTimerManager().ClearTimer(InputTimerHandle);
		GetWorldTimerManager().ClearTimer(StateTimerHandle);
	}
}

void APlayableCharacter::BatterSpecialSpawn()
{
	FVector location = GetActorLocation();
	FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);
	if (PlayerState->Direction == EDirection::Left)
	{
		location.X -= 128.0f;
		rotation = FRotator(0.0f, 180.0f, 0.0f);
		LaunchCharacter(FVector(-300.0f, 0.0f, 64.0f), false, false);
	}
	else if (PlayerState->Direction == EDirection::Right)
	{
		location.X += 128.0f;
		LaunchCharacter(FVector(300.0f, 0.0f, 64.0f), false, false);
	}

	APlayableAttackHitbox* hitbox = GetWorld()->SpawnActor<APlayableAttackHitbox>(AttackHitboxTemplate, location, rotation);
	hitbox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	hitbox->Spawn(TEXT("Test_Basic"), PlayerConstants::BatterSpecialAttackDamage);
}

void APlayableCharacter::BatterComboAttackSpawn()
{
	if (ComboNumber == 0 || ComboNumber == 2)
	{
		FVector location = GetActorLocation();
		FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);
		if (PlayerState->Direction == EDirection::Left)
		{
			location.X -= 96.0f;
			rotation = FRotator(0.0f, 180.0f, 0.0f);
			LaunchCharacter(FVector(-300.0f, 0.0f, 64.0f), false, false);
		}
		else if (PlayerState->Direction == EDirection::Right)
		{
			location.X += 96.0f;
			LaunchCharacter(FVector(300.0f, 0.0f, 64.0f), false, false);
		}

		APlayableAttackHitbox* hitbox = GetWorld()->SpawnActor<APlayableAttackHitbox>(AttackHitboxTemplate, location, rotation);
		hitbox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		hitbox->Spawn(TEXT("Test_Basic"), PlayerConstants::BatterComboOneDamage);
		ComboNumber++;
	}
	else if (ComboNumber == 1 || ComboNumber == 3)
	{
		FVector location = GetActorLocation();
		FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);
		if (PlayerState->Direction == EDirection::Left)
		{
			location.X -= 96.0f;
			rotation = FRotator(0.0f, 180.0f, 0.0f);
			LaunchCharacter(FVector(-300.0f, 0.0f, 64.0f), false, false);
		}
		else if (PlayerState->Direction == EDirection::Right)
		{
			location.X += 96.0f;
			LaunchCharacter(FVector(300.0f, 0.0f, 64.0f), false, false);
		}

		APlayableAttackHitbox* hitbox = GetWorld()->SpawnActor<APlayableAttackHitbox>(AttackHitboxTemplate, location, rotation);
		hitbox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		hitbox->Spawn(TEXT("Test_Basic"), (ComboNumber == 1 ? PlayerConstants::BatterComboTwoDamage : PlayerConstants::BatterComboThreeDamage));
		ComboNumber++;
	}
	else
	{
		FVector location = GetActorLocation();
		FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);
		if (PlayerState->Direction == EDirection::Left)
		{
			location.X -= 96.0f;
			rotation = FRotator(0.0f, 180.0f, 0.0f);
			LaunchCharacter(FVector(-300.0f, 0.0f, 64.0f), false, false);
		}
		else if (PlayerState->Direction == EDirection::Right)
		{
			location.X += 96.0f;
			LaunchCharacter(FVector(300.0f, 0.0f, 64.0f), false, false);
		}

		APlayableAttackHitbox* hitbox = GetWorld()->SpawnActor<APlayableAttackHitbox>(AttackHitboxTemplate, location, rotation);
		hitbox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		hitbox->Spawn(TEXT("Test_Basic"), PlayerConstants::BatterComboFinisherDamage);
		ComboNumber = 0;
	}
}
