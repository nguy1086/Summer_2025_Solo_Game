// Fill out your copyright notice in the Description page of Project Settings.


#include "Slime.h"
#include "Components/CapsuleComponent.h"
#include "../../AIModule/Classes/Perception/PawnSensingComponent.h"
#include "../../AIModule/Classes/AIController.h"
#include "PaperFlipbookComponent.h"
ASlime::ASlime() :
    State(ESlimeState::Idle)
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleSize(32.0f, 32.0f);
    FlipbookComponent->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
    //GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASlime::OnHit);
}

void ASlime::BeginPlay()
{
    Super::BeginPlay();

    if (PawnSensingComponent)
        PawnSensingComponent->OnSeePawn.AddDynamic(this, &ASlime::OnSeePawn);
}

void ASlime::OnSeePawn(APawn* OtherPawn)
{
}

void ASlime::ApplyStateChange(ESlimeState newState)
{
    if (State == newState)
        return;

    ESlimeState old = State;
    State = newState;
}

void ASlime::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASlime::OnDamaged(float damage)
{
    Health -= damage;
    if (Health <= 0.0f)
        Destroy();
}