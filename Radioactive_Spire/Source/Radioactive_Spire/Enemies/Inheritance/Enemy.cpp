// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../Game/RadioactiveSpire_GameModeBase.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy() :
	BoxComponent(nullptr),
	FlipbookComponent(nullptr),
	Direction(EEnemyDirection::Right),
	InvincibleTimer(0.0f),
	Damage(0.0f)
{
	//GetCapsuleComponent() = CreateDefaultSubobject<UCapsuleComponent>("EnemyBoxComponent");
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionProfileName("Entity");
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->BodyInstance.bLockXRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockYRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockZRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockYTranslation = true;

	RootComponent = GetCapsuleComponent();

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("EnemyFlipbook");
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetCollisionProfileName("NoCollision");
	FlipbookComponent->SetupAttachment(RootComponent);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Tags.Add("Enemy");
}

void AEnemy::CheckDirection()
{
	if (GetVelocity().X < 0.0f)
	{
		Direction = EEnemyDirection::Left;
		SetActorRotation(FRotator(0.0, 180.0f, 0.0f));

	}
	else if (GetVelocity().X > 0.0f)
	{
		Direction = EEnemyDirection::Right;
		SetActorRotation(FRotator(0.0, 0.0f, 0.0f));
	}
}

void AEnemy::ChangeDirection(EEnemyDirection dir)
{
	if (dir == EEnemyDirection::Left)
	{
		Direction = dir;
		SetActorRotation(FRotator(0.0, 180.0f, 0.0f));
	}
	else if (dir == EEnemyDirection::Right)
	{
		Direction = dir;
		SetActorRotation(FRotator(0.0, 0.0f, 0.0f));
	}
}

//void AEnemy::Destroyed()
//{
//	ARadioactiveSpire_GameModeBase* gameMode = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
//	if (gameMode)
//	{
//		gameMode->EnemiesKilled++;
//	}
//}
