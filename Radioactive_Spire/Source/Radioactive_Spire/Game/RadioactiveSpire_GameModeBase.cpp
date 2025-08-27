// Fill out your copyright notice in the Description page of Project Settings.


#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"
#include "DeadActor.h"

#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"
#include "../Players/Inheritance/PlayerCamera.h"
#include "../Players/Inheritance/PlayableController.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "PaperTileMapComponent.h"
#include "PaperTileMapActor.h"
#include "PaperTileMap.h"

#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/BrushComponent.h"


ARadioactiveSpire_GameModeBase::ARadioactiveSpire_GameModeBase() :
    Camera(nullptr)
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
	{
		APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(RedDesertLevel, FVector(-1008.0f, 0.0f, 1500.0f), FRotator::ZeroRotator);
	}
	if (RedDesertSky)
	{
		APaperTileMapActor* tilemap = GetWorld()->SpawnActor<APaperTileMapActor>(RedDesertSky, FVector(-1008.0f, 0.0f, 2000.0f), FRotator::ZeroRotator);
		Camera->LevelBackground = tilemap;
	}
}

void ARadioactiveSpire_GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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
