// Fill out your copyright notice in the Description page of Project Settings.


#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"
#include "DeadActor.h"

#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"
#include "../Players/Inheritance/PlayerCamera.h"
#include "../Players/Inheritance/PlayableController.h"
#include "../Players/Inheritance/PlayableAttackHitbox.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMapActor.h"
#include "PaperTileMap.h"
#include "../Enemies/Inheritance/Enemy.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/BrushComponent.h"
#include "Kismet/GameplayStatics.h"

ARadioactiveSpire_GameModeBase::ARadioactiveSpire_GameModeBase() :
	Camera(nullptr),
	SuperPauseTimer(0.0f),
	Game_IsPaused(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARadioactiveSpire_GameModeBase::BeginPlay()
{
    Super::BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	Camera = GetWorld()->SpawnActor<APlayerCamera>(APlayerCamera::StaticClass(), FVector(), FRotator(), SpawnParams);
	ApplyViewMode(EViewModeIndex::VMI_Unlit, false, *GetWorld()->GetGameViewport()->GetEngineShowFlags());

	if (RedDesertLevel)
		APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(RedDesertLevel, FVector(-1008.0f, 0.0f, 1500.0f), FRotator::ZeroRotator);
	if (RedDesertSky)
	{
		APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(RedDesertSky, FVector(-1008.0f, 0.0f, 2000.0f), FRotator::ZeroRotator);
		Camera->LevelBackground = tilemap;
	}
}

void ARadioactiveSpire_GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

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
}

void ARadioactiveSpire_GameModeBase::PlayerDied()
{
	APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
	if (player)
		SpawnDeathAnimation(player->GetActorLocation());
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
	if (Game_IsPaused)
	{
		Game_IsPaused = false;
		UnpauseActors();
		APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
		APlayableController* PlayableController = Cast<APlayableController>(player->GetController());
		PlayableController->SetShowMouseCursor(false);
	}
	else if (!Game_IsPaused)
	{
		Game_IsPaused = true;
		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AActor* actor = *ActorItr;
			actor->CustomTimeDilation = 0.0f;

			if (actor->ActorHasTag("Controller"))
				actor->CustomTimeDilation = 1.0f;
		}
		Camera->CustomTimeDilation = 1.0f;
		APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
		APlayableController* PlayableController = Cast<APlayableController>(player->GetController());
		PlayableController->SetShowMouseCursor(true);
	}
}

void ARadioactiveSpire_GameModeBase::SpawnDeathAnimation(FVector location)
{
	PauseActors();

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

		//if (actor->ActorHasTag("Controller"))
		//{
		//	actor->CustomTimeDilation = 1.0f;
		//}
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

//APlayableCharacter* player;
//for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
//{
//	AActor* actor = *ActorItr;
//	player = Cast<APlayableCharacter>(actor);
//	if (player)
//		break;
//}
//if (player)
//{
//	APlayableController* PlayableController = Cast<APlayableController>(player->GetController());
//	SetPause(PlayableController, FCanUnpause::)
//	Game_IsPaused = UGameplayStatics::IsGamePaused(GetWorld());
//}

//UGameplayStatics::SetGamePaused(GetWorld(), !Game_IsPaused);