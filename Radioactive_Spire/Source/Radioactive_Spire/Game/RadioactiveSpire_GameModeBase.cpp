// Fill out your copyright notice in the Description page of Project Settings.


#include "RadioactiveSpire_GameModeBase.h"
#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"
#include "../Players/Inheritance/PlayerCamera.h"
//#include "EngineUtils.h"
ARadioactiveSpire_GameModeBase::ARadioactiveSpire_GameModeBase() :
    Camera(nullptr)
{
}

void ARadioactiveSpire_GameModeBase::BeginPlay()
{
    Super::BeginPlay();
}

void ARadioactiveSpire_GameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ARadioactiveSpire_GameModeBase::PlayerDied(bool spawnDeadMario)
{
}

void ARadioactiveSpire_GameModeBase::SpawnDeathAnimation(FVector location)
{
}

void ARadioactiveSpire_GameModeBase::PauseActors()
{
	//for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	//{
	//	AActor* actor = *ActorItr;
	//	actor->CustomTimeDilation = 0.0f;

	//	if (actor->ActorHasTag("Player"))
	//	{
	//		APlayableCharacter* player = Cast<APlayableCharacter>(actor);
	//		player->GetSprite()->SetVisibility(false);
	//	}
	//}
}

void ARadioactiveSpire_GameModeBase::UnpauseActors()
{
	//for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	//{
	//	AActor* actor = *ActorItr;
	//	actor->CustomTimeDilation = 1.0f;

	//	if (actor->ActorHasTag("Player"))
	//	{
	//		APlayableCharacter* player = Cast<APlayableCharacter>(actor);
	//		player->GetSprite()->SetVisibility(true);
	//	}
	//}
}
