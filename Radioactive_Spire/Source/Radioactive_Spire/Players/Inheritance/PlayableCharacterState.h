// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayableCharacterState.generated.h"

UENUM(BlueprintType)
enum class EState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Walking		UMETA(DisplayName = "Walking"),
	Jumping		UMETA(DisplayName = "Jumping"),
	Falling		UMETA(DisplayName = "Falling"),
	Ducking		UMETA(DisplayName = "Ducking"),
	Dead		UMETA(DisplayName = "Dead"),
};

UENUM(BlueprintType)
enum class EDirection : uint8
{
	Right	UMETA(DisplayName = "Right"),
	Left	UMETA(DisplayName = "Left")
};

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API APlayableCharacterState : public APlayerState
{
	GENERATED_BODY()
	
public:
	APlayableCharacterState();

	float DirectionScalar();

	EState State;
	EDirection Direction;

	float InvincibilityTimer;

	bool IsOnGround;
};
