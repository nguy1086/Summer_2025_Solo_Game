// Fill out your copyright notice in the Description page of Project Settings.


#include "Flying_Spitter_Projectile.h"
#include "../../Players/Inheritance/PlayableCharacter.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"

AFlying_Spitter_Projectile::AFlying_Spitter_Projectile() :
	Velocity(FVector(100.0f,100.0f,100.0f)),
	LifeTime(8.0f)
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionProfileName("Entity");
	CapsuleComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent->BodyInstance.bLockXRotation = true;
	CapsuleComponent->BodyInstance.bLockYRotation = true;
	CapsuleComponent->BodyInstance.bLockZRotation = true;
	CapsuleComponent->BodyInstance.bLockYTranslation = true;
	CapsuleComponent->SetCapsuleSize(16.0f, 16.0f);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AFlying_Spitter_Projectile::OnOverlapBegin);
	RootComponent = CapsuleComponent;

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("EnemyFlipbook");
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetCollisionProfileName("NoCollision");
	FlipbookComponent->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	Tags.Add("EnemyHurtbox");
}

void AFlying_Spitter_Projectile::BeginPlay()
{
	Super::BeginPlay();
}

void AFlying_Spitter_Projectile::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayableCharacter* player = Cast<APlayableCharacter>(OtherActor);
	if (player)
	{
		player->HandleDamage(15.0f, this);
		Destroy();
	}
}

void AFlying_Spitter_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTime -= DeltaTime;
	if (LifeTime <= 0.0f)
		Destroy();
	else
	{
		FVector start = GetActorLocation();

		start += GetActorForwardVector() * Velocity * DeltaTime;
		SetActorLocation(start);
	}
}

