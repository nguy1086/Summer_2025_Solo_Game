// Fill out your copyright notice in the Description page of Project Settings.


#include "Slime.h"
#include "Components/CapsuleComponent.h"
#include "../../AIModule/Classes/Perception/PawnSensingComponent.h"
#include "../../AIModule/Classes/AIController.h"
#include "PaperFlipbookComponent.h"

#include "../../Players/Inheritance/PlayableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
ASlime::ASlime() :
    State(ESlimeState::Idle)
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleSize(32.0f, 32.0f);
    FlipbookComponent->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
    //GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASlime::OnHit);

    //GetCharacterMovement()->GroundFriction = PlayerConstants::BatterGroundFriction;
    GetCharacterMovement()->MaxWalkSpeed = 20.0f;
    //GetCharacterMovement()->MaxAcceleration = PlayerConstants::BatterMaxAcceleration;
    //GetCharacterMovement()->MinAnalogWalkSpeed = PlayerConstants::BatterMinSpeed;
}

void ASlime::BeginPlay()
{
    Super::BeginPlay();
}

void ASlime::ApplyStateChange(ESlimeState newState)
{
    if (State == newState)
        return;

    ESlimeState old = State;
    State = newState;
    GetSprite()->SetVisibility(true);
    UpdateFlipbook();
}

void ASlime::UpdateFlipbook()
{
    UPaperFlipbook* flipbook = nullptr;
    if (State == ESlimeState::Idle)
        flipbook = IdleFlipbook;
    else if (State == ESlimeState::Hurt)
        flipbook = HurtFlipbook;
    else if (State == ESlimeState::Attack)
        flipbook = AttackFlipbook;

    FlipbookComponent->SetFlipbook(flipbook);
    FlipbookComponent->PlayFromStart();
}

void ASlime::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (State == ESlimeState::Idle)
    {
        APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
        if (player && player->IsPlayerControlled())
        {
            float dir = player->GetActorLocation().X - GetActorLocation().X;
            AddMovementInput(FVector(1.0f, 0.0f, 0.0f), dir);

            if (GEngine)
                GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Magenta, FString::SanitizeFloat(dir));

            CheckDirection();
        }
    }
    else if (State == ESlimeState::Hurt)
    {
        GetSprite()->ToggleVisibility();
    }
    InvincibleTimer -= DeltaTime;
}

void ASlime::OnDamaged(float damage)
{
    if (InvincibleTimer < 0.0f)
    {
        Health -= damage;
        if (Health <= 0.0f)
            Destroy();
        else
        {
            ApplyStateChange(ESlimeState::Hurt);
            InvincibleTimer = 0.1f;
        }
    }
}

void ASlime::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    ApplyStateChange(ESlimeState::Idle);
}
