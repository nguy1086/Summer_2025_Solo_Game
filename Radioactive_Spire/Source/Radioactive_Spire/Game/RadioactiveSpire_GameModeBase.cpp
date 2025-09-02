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
#include "../Enemies/Demo/Flying_Spitter_Projectile.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/BrushComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "../MainMenu/Radioactive_Spire_GameInstance.h"

ARadioactiveSpire_GameModeBase::ARadioactiveSpire_GameModeBase() :
	Camera(nullptr),
	SuperPauseTimer(0.0f),
	Game_IsPaused(false),
	Gameplay_Level(0),
	Gameplay_MaxEnemiesSpawn(0),
	Gameplay_CurrentEnemiesSpawned(0),
	Gameplay_SpawnDelay(0.0f),
	EnemiesKilled(0),
	Gameplay_LevelPosition(FVector(-1008.0f, 0.0f, 1500.0f)),
	Gameplay_TransitionPosition(),
	Gameplay_WaitTimer(5.0f),
	State(EGameState::FadeToEnter),
	LevelAudioComponent(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARadioactiveSpire_GameModeBase::BeginPlay()
{
    Super::BeginPlay();
	GameInstance = Cast<URadioactive_Spire_GameInstance>(GetGameInstance());

	UGameUserSettings* GameUserSettings = GEngine->GameUserSettings;
	if (GameUserSettings)
	{
		GameUserSettings->SetScreenResolution(FIntPoint(1280.0f, 720.0f));
		GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		GameUserSettings->ApplyResolutionSettings(true);
		GameUserSettings->ApplySettings(true);
	}

	if (GEngine && GEngine->GameViewport)
		GEngine->GameViewport->HandleToggleFullscreenCommand();

	Player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
	Player->SetActorLocation(FVector(360.0f, 0.0f, 740.0f));

	Gameplay_MaxEnemiesSpawn = FMath::RandRange(7, 10);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	Camera = GetWorld()->SpawnActor<APlayerCamera>(APlayerCamera::StaticClass(), FVector(), FRotator(), SpawnParams);
	ApplyViewMode(EViewModeIndex::VMI_Unlit, false, *GetWorld()->GetGameViewport()->GetEngineShowFlags());

	APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(GetRandomRedDesertLevel(), Gameplay_LevelPosition, FRotator::ZeroRotator);
	IncrementLevelPosition();
	tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(GetRandomRedDesertLevel(), Gameplay_LevelPosition, FRotator::ZeroRotator);

	if (RedDesertSky)
	{
		tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(RedDesertSky, FVector(-1008.0f, 0.0f, 2000.0f), FRotator::ZeroRotator);
		Camera->LevelBackground = tilemap;
	}


	//if (GameInstance && GameInstance->LevelMusic != nullptr)
	//	LevelAudioComponent = UGameplayStatics::SpawnSound2D(this, GameInstance->LevelMusic);
	if (GameInstance->LevelMusic != nullptr)
		UGameplayStatics::PlaySoundAtLocation(this, GameInstance->LevelMusic, FVector());

	//#include "Sound/SoundClass.h"
	//LevelSound->SoundClassObject->Properties.Volume = 0.0f;
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
			if (Gameplay_SpawnDelay < -1.0f)
			{
				TArray<AActor*> ActorsWithTag;
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Enemy"), ActorsWithTag);
				if (ActorsWithTag.Num() == 0)
					State = EGameState::Wait;
			}

			Gameplay_SpawnDelay -= DeltaTime;
			if (Gameplay_SpawnDelay <= 0.0f)
				SpawnEnemy();
		}
	}
	else if (State == EGameState::Wait)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Wait"));
		if (Gameplay_WaitTimer > 0.0f)
		{
			Gameplay_WaitTimer -= DeltaTime;
			if (Gameplay_WaitTimer <= 0.0f)
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
		FVector dir = Gameplay_TransitionPosition - pos;
		dir.Normalize();
		pos += dir * 300.0f * DeltaTime;
		pos.X = Player->GetActorLocation().X;


		Player->SetActorLocation(FVector(pos));
		Player->NoGravity();
		if (Player->GetActorLocation().Y <= Gameplay_TransitionPosition.Y + 1.0f)
		{
			State = EGameState::Gameplay;
			EnableControls();
			Player->SetGravity();
			Player->SetActorLocation(FVector(Player->GetActorLocation().X, Gameplay_TransitionPosition.Y, Player->GetActorLocation().Z));
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
		Game_IsPaused = false;
		if (LevelAudioComponent != nullptr)
			LevelAudioComponent->Stop();

		State = EGameState::EndGame;
		BlackenActors();
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
		if (super && super->ActorHasTag("ActiveSuper"))
		{
			super->CustomTimeDilation = 1.0f;
			SuperAttackPaused = super;
			SuperPausedVelocity = SuperAttackPaused->ProjectileMovementComponent->Velocity;
			SuperAttackPaused->ProjectileMovementComponent->Velocity = FVector::ZeroVector;
			SuperPauseTimer = timer;
			BlackenActors();
			SuperAttackPaused->Tags.Remove("ActiveSuper");
		}
	}
	Camera->CustomTimeDilation = 1.0f;
	CustomTimeDilation = 1.0f;
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
		AFlying_Spitter_Projectile* spitter_proj = Cast< AFlying_Spitter_Projectile>(actor);
		APaperTileMapActor* tilemap = Cast< APaperTileMapActor>(actor);
		APlayableAttackHitbox* super = Cast<APlayableAttackHitbox>(actor);
		if (enemy)
			enemy->FlipbookComponent->SetSpriteColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		else if (spitter_proj)
			spitter_proj->FlipbookComponent->SetSpriteColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
		else if (super && !super->ActorHasTag("ActiveSuper"))
			super->FlipbookComponent->SetSpriteColor(FLinearColor(0.2f, 0.2f, 0.2f, 0.5f));
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
		AFlying_Spitter_Projectile* spitter_proj = Cast< AFlying_Spitter_Projectile>(actor);
		APlayableAttackHitbox* super = Cast<APlayableAttackHitbox>(actor);
		if (enemy)
			enemy->FlipbookComponent->SetSpriteColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		else if (spitter_proj)
			spitter_proj->FlipbookComponent->SetSpriteColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		else if (super)
			super->FlipbookComponent->SetSpriteColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		else if (tilemap)
			tilemap->SetActorHiddenInGame(false);
	}
}

void ARadioactiveSpire_GameModeBase::TransitionToNextLevel()
{
	IncrementLevelPosition();
	APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(GetRandomRedDesertLevel(), Gameplay_LevelPosition, FRotator::ZeroRotator);
	Gameplay_TransitionPosition = FVector(Player->GetActorLocation().X, tilemap->GetActorLocation().Y + GameConstants::LevelPosYIncrement, Player->GetActorLocation().Z + 64.0f);
	//Player->SetActorLocation(FVector(Player->GetActorLocation().X, tilemap->GetActorLocation().Y, Player->GetActorLocation().Z + 64.0f));
	Gameplay_MaxEnemiesSpawn += FMath::RandRange(7, 10);
	Gameplay_Level++;
	Gameplay_CurrentEnemiesSpawned = 0;
	EnemiesKilled = 0;
	Gameplay_WaitTimer = GameConstants::WaitMax;
	Gameplay_SpawnDelay = 0.2f;

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
	if (Gameplay_CurrentEnemiesSpawned < Gameplay_MaxEnemiesSpawn)
	{
		int32 index = FMath::RandRange(0, 10) % 2;
		float x[] = { 50.0f, 940.0f };

		int32 spawnchance = FMath::RandRange(0, 1);
		if (spawnchance == 0)
			ASlime* slime = GetWorld()->SpawnActor<ASlime>(Slime, FVector(x[index], Player->GetActorLocation().Y, 940.0f + Camera->LevelZIncrease), FRotator::ZeroRotator);
		else
			AFlying_Spiter* spitter = GetWorld()->SpawnActor<AFlying_Spiter>(FlyingSpitter, FVector(x[index], Player->GetActorLocation().Y, 940.0f + Camera->LevelZIncrease), FRotator::ZeroRotator);

		Gameplay_CurrentEnemiesSpawned++;
		Gameplay_SpawnDelay = FMath::RandRange(0.5f, 2.5f) - (0.1f * Gameplay_Level);
		if (Gameplay_SpawnDelay < 0.0f)
			Gameplay_SpawnDelay = 0.0f;
	}
}

void ARadioactiveSpire_GameModeBase::IncrementLevelPosition()
{
	Gameplay_LevelPosition.Y -= GameConstants::LevelPosYIncrement;
	Gameplay_LevelPosition.Z += GameConstants::LevelPosZIncrement;
	Camera->LevelZIncrease += GameConstants::LevelPosZIncrement;
}

TSubclassOf<APaperTileMapActor> ARadioactiveSpire_GameModeBase::GetRandomRedDesertLevel()
{
	TArray<TSubclassOf<APaperTileMapActor>> x = { RedDesertLevelOne, RedDesertLevelTwo, RedDesertLevelThree, RedDesertLevelFour, RedDesertLevelFive };
	return x[FMath::RandRange(0, 4)];
}
