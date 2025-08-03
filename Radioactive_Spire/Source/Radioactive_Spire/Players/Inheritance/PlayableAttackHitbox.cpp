// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableAttackHitbox.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
APlayableAttackHitbox::APlayableAttackHitbox() :
	Timer(0.0f),
	IsProjectile(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("HitboxFlipbook");
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetCollisionProfileName("NoCollision");
	FlipbookComponent->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
}

// Called when the game starts or when spawned
void APlayableAttackHitbox::BeginPlay()
{
	Super::BeginPlay();
}

void APlayableAttackHitbox::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void APlayableAttackHitbox::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && (OtherActor != this->GetOwner()))
		ProjectileMovementComponent->StopMovementImmediately();

}

void APlayableAttackHitbox::Spawn(FString name, float damage)
{
	if (!name.IsEmpty() && FlipbookComponent)
	{
		UPaperFlipbook* flipbook = nullptr;

		Damage = damage;

		if (name == TEXT("Test_Basic"))
			flipbook = TestBasicFlipbook;


		FlipbookComponent->SetFlipbook(flipbook);
		FlipbookComponent->SetLooping(false);

		//FlipbookComponent->OnFinishedPlaying.AddDynamic(this, &APlayableAttackHitbox::OnFinishPlaying());
		GetWorldTimerManager().SetTimer(
			HitboxTimerHandle, 
			this, &APlayableAttackHitbox::OnFinishPlaying,
			FlipbookComponent->GetFlipbookLength(), 
			true
		);

		InitializeHitbox();

		FlipbookComponent->PlayFromStart();
	}
}

void APlayableAttackHitbox::Projectile(FString name, float damage)
{
	if (!name.IsEmpty() && FlipbookComponent)
	{
		UPaperFlipbook* flipbook = nullptr;

		Damage = damage;

		if (name == TEXT("Batter_Special"))
		{
			flipbook = BatterSpecialFlipbook;
			Timer = PlayerConstants::BatterSpecialLifetime;
		}


		FlipbookComponent->SetFlipbook(flipbook);

		InitializeHitbox();

		FlipbookComponent->PlayFromStart();
	}

}

void APlayableAttackHitbox::OnFinishPlaying()
{
	Destroy();
}

void APlayableAttackHitbox::InitializeHitbox()
{
	SphereComponent->SetSphereRadius(0.0f);
	BoxComponent->SetBoxExtent(FVector(0.0f, 0.0f, 0.0f));
	if (FlipbookComponent->GetFlipbook() == BatterSpecialFlipbook)
	{
		SphereComponent->SetSphereRadius(10.0f);

		ProjectileMovementComponent->UpdatedComponent = SphereComponent;
		ProjectileMovementComponent->InitialSpeed = 2000.f;
		ProjectileMovementComponent->MaxSpeed = 2000.f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
	}

}

// Called every frame
void APlayableAttackHitbox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FlipbookComponent->GetFlipbook() == BatterSpecialFlipbook)
	{
		Timer -= DeltaTime;
		if (Timer < 0.0f)
			Destroy();
	}
}

