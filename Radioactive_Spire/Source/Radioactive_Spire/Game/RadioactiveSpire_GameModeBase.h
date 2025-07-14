// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RadioactiveSpire_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API ARadioactiveSpire_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARadioactiveSpire_GameModeBase();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void PlayerDied();

	void EnableControls();
	void DisableControls();

private:
	void SpawnDeathAnimation(FVector location);

	void PauseActors();
	void UnpauseActors();

	UPROPERTY() class APlayerCamera* Camera;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode Templates")
	TSubclassOf<class ADeadActor> DeadTestTemplate;
};
