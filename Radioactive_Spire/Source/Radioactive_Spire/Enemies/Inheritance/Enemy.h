// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"


UENUM(BlueprintType)
enum class EEnemyDirection : uint8
{
	Right	UMETA(DisplayName = "Right"),
	Left	UMETA(DisplayName = "Left")
};
/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()
	
public:
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPaperFlipbookComponent* FlipbookComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn Sensing", meta = (AllowPrivateAccess = "true"))
	//class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.0f;

	virtual void OnDamaged(float damage) {};
	virtual void UpdateFlipbook() {};

	void CheckDirection();
	EEnemyDirection Direction;
	float InvincibleTimer;
};
