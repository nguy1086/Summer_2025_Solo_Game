// Fill out your copyright notice in the Description page of Project Settings.


#include "Slime.h"
#include "Components/CapsuleComponent.h"
#include "../../AIModule/Classes/Perception/PawnSensingComponent.h"
#include "../../AIModule/Classes/AIController.h"

ASlime::ASlime()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleSize(16.0f, 16.0f);
    GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASlime::OnHit);
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

void ASlime::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ASlime::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}