// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadActor.h"
#include "GameConstants.h"
#include "PaperSpriteComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ADeadActor::ADeadActor() :
	Root(nullptr),
	SpriteComponent(nullptr),
	HoldInPlaceTimer(GameConstants::DeadHoldTime)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USphereComponent>("Root");
	Root->SetVisibility(false);
	Root->SetCollisionProfileName("OverlapAll");
	Root->SetSimulatePhysics(true);
	Root->SetEnableGravity(false);
	RootComponent = Root;

	SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>("SpriteComponent");
	SpriteComponent->SetupAttachment(Root);

	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ADeadActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeadActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HoldInPlaceTimer > 0.0f)
	{
		HoldInPlaceTimer -= DeltaTime;
		if (HoldInPlaceTimer <= 0.0f)
		{
			HoldInPlaceTimer = 0.0f;

			Root->SetEnableGravity(true);
			//Root->AddImpulse(FVector(0.0f, 0.0f, GameConstants::DeadImpulseZ));
		}
	}
}

