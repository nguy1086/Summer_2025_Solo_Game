// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
//#include "Perception/PawnSensingComponent.h"
//#include "AIController.h"

AEnemy::AEnemy() :
	BoxComponent(nullptr),
	FlipbookComponent(nullptr)
{
	//GetCapsuleComponent() = CreateDefaultSubobject<UCapsuleComponent>("EnemyBoxComponent");
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->BodyInstance.bLockXRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockYRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockZRotation = true;
	GetCapsuleComponent()->BodyInstance.bLockYTranslation = true;
	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnemy::OnHit);
	RootComponent = GetCapsuleComponent();

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("EnemyFlipbook");
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetCollisionProfileName("NoCollision");
	FlipbookComponent->SetupAttachment(RootComponent);

	//if (PawnSensingComponent)
	//	PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::OnSeePawn);

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	Tags.Add("Enemy");
}

void AEnemy::OnSeePawn(APawn* OtherPawn)
{
}
