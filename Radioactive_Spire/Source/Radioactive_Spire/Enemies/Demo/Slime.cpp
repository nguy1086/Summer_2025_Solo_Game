// Fill out your copyright notice in the Description page of Project Settings.


#include "Slime.h"
#include "Components/CapsuleComponent.h"
#include "../../AIModule/Classes/Perception/PawnSensingComponent.h"
#include "../../AIModule/Classes/AIController.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

#include "../../Players/Inheritance/PlayableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ASlime::ASlime() :
    State(ESlimeState::Idle),
    AttackTimer(AttackMax)
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleSize(32.0f, 32.0f);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASlime::OnOverlapBegin);
    FlipbookComponent->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
    //GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASlime::OnHit);

    //GetCharacterMovement()->GroundFriction = PlayerConstants::BatterGroundFriction;
    GetCharacterMovement()->MaxWalkSpeed = 20.0f;
    //GetCharacterMovement()->MaxAcceleration = PlayerConstants::BatterMaxAcceleration;
    //GetCharacterMovement()->MinAnalogWalkSpeed = PlayerConstants::BatterMinSpeed;
    Damage = 10.0f;
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

void ASlime::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    APlayableCharacter* player = Cast<APlayableCharacter>(OtherActor);
    if (player)
    {
        if (ActorHasTag("EnemyHurtbox"))
            player->Health -= Damage;
    }
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

    if (ActorHasTag("EnemyHurtbox"))
        Tags.Remove("EnemyHurtbox");

    AttackTimer = AttackMax;

    FlipbookComponent->SetPlayRate(1.0f);
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
        AttackTimer -= DeltaTime;
        if (AttackTimer < 0.0f)
        {
            float distance = GetHorizontalDistanceTo(player);
            if (distance < 100.0f)
                ApplyStateChange(ESlimeState::Attack);
        }
    }
    else if (State == ESlimeState::Hurt)
    {
        GetSprite()->ToggleVisibility();
    }
    else if (State == ESlimeState::Attack)
    {
        float FramesPerSecond = GetSprite()->GetFlipbook()->GetFramesPerSecond();
        float TotalDuration = GetSprite()->GetFlipbookLengthInFrames();

        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ASlime::Attack, ((TotalDuration - 1.0f) / 8.0f), false);
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

void ASlime::Attack()
{
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Attack"));

    LaunchCharacter(FVector(200.0f * (Direction == EEnemyDirection::Left ? -1.0f : 1.0f), 0.0f, 300.0f), false, true);
    Tags.Add("EnemyHurtbox");
    FlipbookComponent->SetPlayRate(0.0f);
}