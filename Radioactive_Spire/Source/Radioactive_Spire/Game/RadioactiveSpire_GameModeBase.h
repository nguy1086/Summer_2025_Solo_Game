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

	void GamePause();
	bool Game_IsPaused;

	int EnemiesKilled;
private:
	void SpawnDeathAnimation(FVector location);

	void PauseActors();
	void UnpauseActors();

	void BlackenActors();
	void UnblackenActors();

	void TransitionToNextLevel();
	void SpawnEnemy();

	UPROPERTY() class APlayerCamera* Camera;
	class APlayableCharacter* Player;
	float SuperPauseTimer;
	FVector SuperPausedVelocity;
	class APlayableAttackHitbox* SuperAttackPaused;

	int Level;
	int MaxEnemiesSpawn;
	int CurrentEnemiesSpawned;
	float SpawnDelay;
	FVector LevelPosition;
	float WaitTimer;
	const float WaitMax = 5.0f;
	void IncrementLevelPosition();
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death Templates")
	TSubclassOf<class ADeadActor> DeadTestTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death Templates")
	TSubclassOf<class ADeadActor> DeadBatterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertSky;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Templates")
	TSubclassOf<class ASlime> Slime;
};
