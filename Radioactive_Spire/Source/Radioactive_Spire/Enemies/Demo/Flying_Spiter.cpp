// Fill out your copyright notice in the Description page of Project Settings.


#include "Flying_Spiter.h"
#include "Components/CapsuleComponent.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "../../Players/Inheritance/PlayableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Flying_Spitter_Projectile.h"

AFlying_Spiter::AFlying_Spiter() :
    AttackTimer(FMath::RandRange(2.0f, 5.0f)),
    State(EFlyingSpitterState::Flying)
{
    PrimaryActorTick.bCanEverTick = true;

    GetCapsuleComponent()->SetCapsuleSize(32.0f, 32.0f);
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFlying_Spiter::OnOverlapBegin);
    GetCapsuleComponent()->SetSimulatePhysics(false);
    GetCapsuleComponent()->SetEnableGravity(false);
    GetCharacterMovement()->MaxFlySpeed = 100.0f;
    FlipbookComponent->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));

    GetCharacterMovement()->MaxWalkSpeed = 30.0f;
    Damage = 10.0f;

}

void AFlying_Spiter::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AFlying_Spiter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (State == EFlyingSpitterState::Flying)
    {
        APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
        if (player && player->IsPlayerControlled())
        {
            FVector dir = player->GetActorLocation() - GetActorLocation();
            dir.Y = 0.0f;

            float distanceX = GetHorizontalDistanceTo(player);
            float distanceZ = GetVerticalDistanceTo(player);

            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("distanceX: " + FString::SanitizeFloat(dir.X)));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("distanceX: " + FString::SanitizeFloat(dir.Z)));
            if (distanceX < 100.0f && distanceZ < 150.0f ||
                distanceX > -100.0f && distanceZ > -150.0f)
            {
                AddMovementInput(dir, -dir.X);
                CheckDirection();
            }
            else if (distanceX < 125.0f && distanceZ < 175.0f ||
                distanceX > -125.0f && distanceZ > -175.0f)
            {
                if (dir.X > 0.0f)
                    Direction = EEnemyDirection::Right;
                else
                    Direction = EEnemyDirection::Left;

                AttackTimer -= DeltaTime;
            }
            else
            {
                AddMovementInput(dir, dir.X);
                CheckDirection();
            }

            ChangeDirection(Direction);
        }

        if (AttackTimer < 0.0f)
        {
            float distance = GetHorizontalDistanceTo(player);
            if (distance < 105.0f)
                ApplyStateChange(EFlyingSpitterState::Attack);
        }
    }
    else if (State == EFlyingSpitterState::Hurt)
    {
        GetSprite()->ToggleVisibility();
        if (InvincibleTimer >= 0.0f)
            InvincibleTimer -= DeltaTime;
        else
            ApplyStateChange(EFlyingSpitterState::Flying);
    }
    else if (State == EFlyingSpitterState::Attack)
    {

    }
}

void AFlying_Spiter::OnDamaged(float damage)
{
    if (InvincibleTimer < 0.0f)
    {
        Health -= damage;
        if (Health <= 0.0f)
            Destroy();
        else
        {
            ApplyStateChange(EFlyingSpitterState::Hurt);
            InvincibleTimer = 0.5f;
        }
    }
}

void AFlying_Spiter::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AFlying_Spiter::ApplyStateChange(EFlyingSpitterState newState)
{
    if (State == newState)
        return;

    EFlyingSpitterState old = State;
    State = newState;

    GetSprite()->SetVisibility(true);
    UpdateFlipbook();

    if (State == EFlyingSpitterState::Attack)
    {
        float FramesPerSecond = FlipbookComponent->GetFlipbook()->GetFramesPerSecond();
        float TotalDuration = FlipbookComponent->GetFlipbookLengthInFrames();

        GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AFlying_Spiter::Attack, TotalDuration / FramesPerSecond, false);
    }
}

void AFlying_Spiter::UpdateFlipbook()
{
    UPaperFlipbook* flipbook = nullptr;
    if (State == EFlyingSpitterState::Flying)
        flipbook = FlyingFlipbook;
    else if (State == EFlyingSpitterState::Hurt)
        flipbook = HurtFlipbook;
    else if (State == EFlyingSpitterState::Attack)
        flipbook = AttackFlipbook;

    AttackTimer = FMath::RandRange(2.0f, 5.0f);

    FlipbookComponent->SetPlayRate(1.0f);
    FlipbookComponent->SetFlipbook(flipbook);
    FlipbookComponent->PlayFromStart();
}

void AFlying_Spiter::Attack()
{
    APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), player->GetActorLocation());

    AFlying_Spitter_Projectile* hitbox = GetWorld()->SpawnActor<AFlying_Spitter_Projectile>(ProjectileTemplate, GetActorLocation(), LookAtRotation);
    ApplyStateChange(EFlyingSpitterState::Flying);
}