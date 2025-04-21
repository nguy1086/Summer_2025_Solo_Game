// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacterState.h"

APlayableCharacterState::APlayableCharacterState()
{
	State = EState::Idle;
	Direction = EDirection::Right;

	InvincibilityTimer = 0.0f;

	IsOnGround = true;
}

float APlayableCharacterState::DirectionScalar()
{
	return Direction == EDirection::Left ? -1.0f : 1.0f;
}
