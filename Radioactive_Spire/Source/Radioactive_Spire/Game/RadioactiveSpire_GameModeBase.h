// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RadioactiveSpire_GameModeBase.generated.h"

enum class EPlayerType : uint8;

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

	void SuperAttackPause(float timer);

private:
	void SpawnDeathAnimation(FVector location);

	void PauseActors();
	void UnpauseActors();

	void BlackenActors();
	void UnblackenActors();

	UPROPERTY() class APlayerCamera* Camera;
	float SuperPauseTimer;
	FVector SuperPausedVelocity;
	class APlayableAttackHitbox* SuperAttackPaused;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death Templates")
	TSubclassOf<class ADeadActor> DeadTestTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death Templates")
	TSubclassOf<class ADeadActor> DeadBatterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertSky;
};
