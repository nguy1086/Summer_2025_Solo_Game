// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../AIModule/Classes/Perception/PawnSensingComponent.h"
#include "../../AIModule/Classes/AIController.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy() :
	BoxComponent(nullptr),
	FlipbookComponent(nullptr),
	Direction(EEnemyDirection::Right),
	InvincibleTimer(0.0f)
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

	//PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	//PawnSensingComponent->SetPeripheralVisionAngle(180.0f); //vision degrees
	//PawnSensingComponent->SightRadius = 5000.0f; //range
	//PawnSensingComponent->SetSensingInterval(0.5f);

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
