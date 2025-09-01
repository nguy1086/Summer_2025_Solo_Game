// Fill out your copyright notice in the Description page of Project Settings.


#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"
#include "PlayableWidget.h"
#include "GameConstants.h"
#include "DeadActor.h"

#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"
#include "../Players/Inheritance/PlayerCamera.h"
#include "../Players/Inheritance/PlayableController.h"
#include "../Players/Inheritance/PlayableAttackHitbox.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/GameUserSettings.h" 

#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMapActor.h"
#include "PaperTileMap.h"

#include "../Enemies/Inheritance/Enemy.h"
#include "../Enemies/Demo/Slime.h"
#include "../Enemies/Demo/Flying_Spiter.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/BrushComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

ARadioactiveSpire_GameModeBase::ARadioactiveSpire_GameModeBase() :
	Camera(nullptr),
	SuperPauseTimer(0.0f),
	Game_IsPaused(false),
	Level(0),
	MaxEnemiesSpawn(0),
	CurrentEnemiesSpawned(0),
	SpawnDelay(0.0f),
	EnemiesKilled(0),
	LevelPosition(FVector(-1008.0f, 0.0f, 1500.0f)),
	TransitionPosition(),
	WaitTimer(5.0f),
	State(EGameState::FadeToEnter),
	LevelAudioComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARadioactiveSpire_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

	UGameUserSettings* GameUserSettings = GEngine->GameUserSettings;
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(FIntPoint(1280.0f, 960.0f));
		GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		GameUserSettings->ApplyResolutionSettings(true);
		GameUserSettings->ApplySettings(true);
	}

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->HandleToggleFullscreenCommand();
	}

	Player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
	Player->SetActorLocation(FVector(360.0f, 0.0f, 740.0f));

	MaxEnemiesSpawn = FMath::RandRange(7, 10);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	Camera = GetWorld()->SpawnActor<APlayerCamera>(APlayerCamera::StaticClass(), FVector(), FRotator(), SpawnParams);
	ApplyViewMode(EViewModeIndex::VMI_Unlit, false, *GetWorld()->GetGameViewport()->GetEngineShowFlags());

	APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(GetRandomRedDesertLevel(), LevelPosition, FRotator::ZeroRotator);
	IncrementLevelPosition();
	tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(GetRandomRedDesertLevel(), LevelPosition, FRotator::ZeroRotator);

	if (RedDesertSky)
	{
		tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(RedDesertSky, FVector(-1008.0f, 0.0f, 2000.0f), FRotator::ZeroRotator);
		Camera->LevelBackground = tilemap;
	}


	if (LevelSound != nullptr)
		LevelAudioComponent = UGameplayStatics::SpawnSound2D(this, LevelSound);
}

void ARadioactiveSpire_GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	if (State == EGameState::Gameplay)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Gameplay"));

		if (SuperPauseTimer >= 0.0f)
		{
			SuperPauseTimer -= DeltaTime;

			if (SuperPauseTimer <= 0.0f)
			{
				UnpauseActors();
				UnblackenActors();

				if (SuperAttackPaused)
					SuperAttackPaused->ProjectileMovementComponent->Velocity = SuperPausedVelocity;
			}
		}
		else
		{
			if (SpawnDelay < -1.0f)
			{
				TArray<AActor*> ActorsWithTag;
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Enemy"), ActorsWithTag);
				if (ActorsWithTag.Num() == 0)
				{
					State = EGameState::Wait;

				}
			}

			SpawnDelay -= DeltaTime;
			if (SpawnDelay <= 0.0f)
				SpawnEnemy();
		}
	}
	else if (State == EGameState::Wait)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Wait"));
		if (WaitTimer > 0.0f)
		{
			WaitTimer -= DeltaTime;
			if (WaitTimer <= 0.0f)
				TransitionToNextLevel();
		}
	}
	else if (State == EGameState::Transition)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Transition"));
		DisableControls();
		//FVector p = FMath::VInterpTo(TransitionPosition, Player->GetActorLocation(), DeltaTime, 0.00000000001f);
		Player->CustomTimeDilation = 0.0f;
		FVector pos = Player->GetActorLocation();
		FVector dir = TransitionPosition - pos;
		dir.Normalize();
		pos += dir * 300.0f * DeltaTime;
		pos.X = Player->GetActorLocation().X;


		Player->SetActorLocation(FVector(pos));
		Player->NoGravity();
		if (Player->GetActorLocation().Y <= TransitionPosition.Y + 1.0f)
		{
			State = EGameState::Gameplay;
			EnableControls();
			Player->SetGravity();
			Player->SetActorLocation(FVector(Player->GetActorLocation().X, TransitionPosition.Y, Player->GetActorLocation().Z));
			Player->CustomTimeDilation = 1.0f;
		}
	}
	else if (State == EGameState::FadeToEnter)
	{
		Player->CustomTimeDilation = 0.0f;
		APlayableController* PlayableController = Cast<APlayableController>(Player->GetController());
		if ((PlayableController->GameInfoWidget->FadeTimer / 2.0f )<= 0.0f)
		{
			Player->CustomTimeDilation = 1.0f;
			Player->ApplyStateChange(EState::Falling);
			State = EGameState::Gameplay;
			PlayableController->GameInfoWidget->FadeTimer = 0.0f;
		}
	}
	else if (State == EGameState::FadeToQuit)
	{
		CustomTimeDilation = 1.0f;
		DisableControls();
		PauseActors();
		APlayableController* PlayableController = Cast<APlayableController>(Player->GetController());
		if ((PlayableController->GameInfoWidget->FadeTimer / 1.5f) >= 1.5f)
			PlayableController->GameInfoWidget->OnPauseQuit();
	}
	else if (State == EGameState::FadeToRetry)
	{
		DisableControls();
		APlayableController* PlayableController = Cast<APlayableController>(Player->GetController());

		if ((PlayableController->GameInfoWidget->FadeTimer / 1.5f) >= 1.5f)
			PlayableController->GameInfoWidget->OnRetry();
	}
	else if (State == EGameState::EndGame)
	{
		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AActor* actor = *ActorItr;
			APlayableCharacter* player = Cast<APlayableCharacter>(actor);
			AEnemy* enemy = Cast<AEnemy>(actor);
			APaperTileMapActor* tilemap = Cast< APaperTileMapActor>(actor);
			if (!player && actor->CustomTimeDilation > 0.0f && !actor->ActorHasTag("PlayerCamera"))
				actor->CustomTimeDilation -= DeltaTime;
			if (enemy)
				enemy->FlipbookComponent->SetSpriteColor(enemy->FlipbookComponent->GetSpriteColor() - FLinearColor(DeltaTime, DeltaTime, DeltaTime, DeltaTime));
			else if (tilemap)
				tilemap->SetActorHiddenInGame(true);
		}
	}
}

void ARadioactiveSpire_GameModeBase::PlayerDied()
{
	if (Player)
	{
		if (LevelAudioComponent != nullptr)
			LevelAudioComponent->Stop();

		State = EGameState::EndGame;
		BlackenActors();
		SpawnDeathAnimation(Player->GetActorLocation());
	}
}

void ARadioactiveSpire_GameModeBase::EnableControls()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		if (ActorItr->ActorHasTag("Controller"))
			ActorItr->CustomTimeDilation = 1.0f;
}//disable/enableinput(controller) doesnt work apparently, even using every getcontroller function
	//so this is just an elaborate way to work through this problem
void ARadioactiveSpire_GameModeBase::DisableControls()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		if (ActorItr->ActorHasTag("Controller"))
			ActorItr->CustomTimeDilation = 0.0f;
}

void ARadioactiveSpire_GameModeBase::SuperAttackPause(float timer)
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		actor->CustomTimeDilation = 0.0f;

		APlayableCharacter* player = Cast<APlayableCharacter>(actor);
		if (player)
			player->CustomTimeDilation = 1.0f;
		APlayableAttackHitbox* super = Cast<APlayableAttackHitbox>(actor);
		if (super && super->ActorHasTag("Super"))
		{
			super->CustomTimeDilation = 1.0f;
			SuperAttackPaused = super;
			SuperPausedVelocity = SuperAttackPaused->ProjectileMovementComponent->Velocity;
			SuperAttackPaused->ProjectileMovementComponent->Velocity = FVector::ZeroVector;
		}
	}
	Camera->CustomTimeDilation = 1.0f;
	CustomTimeDilation = 1.0f;
	BlackenActors();
	SuperPauseTimer = timer;
}

void ARadioactiveSpire_GameModeBase::GamePause()
{
	if (State != EGameState::EndGame)
	{
		if (Game_IsPaused)
		{
			Game_IsPaused = false;
			UnpauseActors();
			APlayableController* PlayableController = Cast<APlayableController>(Player->GetController());
		}
		else if (!Game_IsPaused)
		{
			Game_IsPaused = true;
			PauseActors();
			Camera->CustomTimeDilation = 1.0f;
			APlayableController* PlayableController = Cast<APlayableController>(Player->GetController());
		}
	}
}

void ARadioactiveSpire_GameModeBase::SpawnDeathAnimation(FVector location)
{
	//PauseActors();

	APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
	if (player)
	{
		// HANDLE TYPE OF PLAYER
		if (player->Type == EPlayerType::Test)
		{
			if (DeadTestTemplate != nullptr)
			{
				UWorld* const world = GetWorld();
				if (world != nullptr)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
					FTransform SpawnTransform(location);

					ADeadActor* deadActor = world->SpawnActor<ADeadActor>(DeadTestTemplate, SpawnTransform, SpawnParams);
				}
			}
		}
		else if (player->Type == EPlayerType::Batter)
		{
			UWorld* const world = GetWorld();
			if (world != nullptr)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				FTransform SpawnTransform(location);

				ADeadActor* deadActor = world->SpawnActor<ADeadActor>(DeadBatterTemplate, SpawnTransform, SpawnParams);
			}
		}
	}
}

void ARadioactiveSpire_GameModeBase::PauseActors()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		actor->CustomTimeDilation = 0.0f;

		APlayableCharacter* player = Cast<APlayableCharacter>(actor);
		if (player)
			player->GetSprite()->SetVisibility(false);

		if (actor->ActorHasTag("Controller"))
			actor->CustomTimeDilation = 1.0f;
	}
}

void ARadioactiveSpire_GameModeBase::UnpauseActors()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		actor->CustomTimeDilation = 1.0f;

		APlayableCharacter* player = Cast<APlayableCharacter>(actor);
		if (player)
			player->GetSprite()->SetVisibility(true);
	}
}

void ARadioactiveSpire_GameModeBase::BlackenActors()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		AEnemy* enemy = Cast<AEnemy>(actor);
		APaperTileMapActor* tilemap = Cast< APaperTileMapActor>(actor);
		if (enemy)
			enemy->FlipbookComponent->SetSpriteColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		else if (tilemap)
			tilemap->SetActorHiddenInGame(true);

	}
}

void ARadioactiveSpire_GameModeBase::UnblackenActors()
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		AEnemy* enemy = Cast<AEnemy>(actor);
		APaperTileMapActor* tilemap = Cast< APaperTileMapActor>(actor);
		if (enemy)
			enemy->FlipbookComponent->SetSpriteColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		else if (tilemap)
			tilemap->SetActorHiddenInGame(false);
	}
}

void ARadioactiveSpire_GameModeBase::TransitionToNextLevel()
{
	IncrementLevelPosition();
	APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(GetRandomRedDesertLevel(), LevelPosition, FRotator::ZeroRotator);
	TransitionPosition = FVector(Player->GetActorLocation().X, tilemap->GetActorLocation().Y + GameConstants::LevelPosYIncrement, Player->GetActorLocation().Z + 64.0f);
	//Player->SetActorLocation(FVector(Player->GetActorLocation().X, tilemap->GetActorLocation().Y, Player->GetActorLocation().Z + 64.0f));
	MaxEnemiesSpawn += FMath::RandRange(7, 10);
	Level++;
	CurrentEnemiesSpawned = 0;
	EnemiesKilled = 0;
	WaitTimer = GameConstants::WaitMax;
	SpawnDelay = 0.2f;

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		APaperTileMapActor* earlytilemap = Cast<APaperTileMapActor>(actor);
		if (earlytilemap)
			if (earlytilemap->GetActorLocation().Y > tilemap->GetActorLocation().Y + GameConstants::LevelPosYIncrement)
				earlytilemap->Destroy();
	}

	State = EGameState::Transition;
}

void ARadioactiveSpire_GameModeBase::SpawnEnemy()
{
	if (CurrentEnemiesSpawned < MaxEnemiesSpawn)
	{
		int32 index = FMath::RandRange(0, 10) % 2;
		float x[] = { 50.0f, 940.0f };

		int32 spawnchance = FMath::RandRange(0, 2);
		if (spawnchance == 0)
			ASlime* slime = GetWorld()->SpawnActor<ASlime>(Slime, FVector(x[index], Player->GetActorLocation().Y, 940.0f + Camera->LevelZIncrease), FRotator::ZeroRotator);
		else
			AFlying_Spiter* slime = GetWorld()->SpawnActor<AFlying_Spiter>(FlyingSpitter, FVector(x[index], Player->GetActorLocation().Y, 940.0f + Camera->LevelZIncrease), FRotator::ZeroRotator);

		CurrentEnemiesSpawned++;
		SpawnDelay = FMath::RandRange(0.5f, 2.5f) - (0.1f * Level);
		if (SpawnDelay < 0.0f)
			SpawnDelay = 0.0f;
	}
}

void ARadioactiveSpire_GameModeBase::IncrementLevelPosition()
{
	LevelPosition.Y -= GameConstants::LevelPosYIncrement;
	LevelPosition.Z += GameConstants::LevelPosZIncrement;
	Camera->LevelZIncrease += GameConstants::LevelPosZIncrement;
}

TSubclassOf<APaperTileMapActor> ARadioactiveSpire_GameModeBase::GetRandomRedDesertLevel()
{
	TArray<TSubclassOf<APaperTileMapActor>> x = { RedDesertLevelOne, RedDesertLevelTwo, RedDesertLevelThree, RedDesertLevelFour, RedDesertLevelFive };
	return x[FMath::RandRange(0, 4)];
}
