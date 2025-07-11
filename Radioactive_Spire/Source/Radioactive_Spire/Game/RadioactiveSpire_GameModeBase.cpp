// Fill out your copyright notice in the Description page of Project Settings.


#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"
#include "DeadActor.h"

#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"
#include "../Players/Inheritance/PlayerCamera.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
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
