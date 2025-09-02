// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RadioactiveSpire_GameModeBase.generated.h"

enum class EPlayerType : uint8;

UENUM(BlueprintType)
enum class EGameState : uint8
{
	Unknown 	UMETA(DisplayName = "Unknown"),
	Gameplay 	UMETA(DisplayName = "Gameplay"),
	Wait 		UMETA(DisplayName = "Wait"),
	Transition	UMETA(DisplayName = "Transition"),
	FadeToQuit	UMETA(DisplayName = "FadeToQuit"),
	FadeToEnter	UMETA(DisplayName = "FadeToEnter"),
	FadeToRetry	UMETA(DisplayName = "FadeToRetry"),
	EndGame     UMETA(DisplayName = "EndGame")
};
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
	EGameState State;
private:

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

	int Gameplay_Level;
	int Gameplay_MaxEnemiesSpawn;
	int Gameplay_CurrentEnemiesSpawned;
	float Gameplay_SpawnDelay;
	FVector Gameplay_LevelPosition;
	FVector Gameplay_TransitionPosition;
	float Gameplay_WaitTimer;

	void IncrementLevelPosition();
	TSubclassOf<class APaperTileMapActor> GetRandomRedDesertLevel();

	class UAudioComponent* LevelAudioComponent;
	class URadioactive_Spire_GameInstance* GameInstance;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertLevelOne;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertLevelTwo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertLevelThree;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertLevelFour;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertLevelFive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level Templates")
	TSubclassOf<class APaperTileMapActor> RedDesertSky;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Templates")
	TSubclassOf<class ASlime> Slime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Templates")
	TSubclassOf<class AFlying_Spiter> FlyingSpitter;
};
