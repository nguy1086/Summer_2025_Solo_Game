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

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("HitboxFlipbook");
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetCollisionProfileName("NoCollision");
	FlipbookComponent->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

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
	//if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && (OtherActor != this->GetOwner()))
	//	ProjectileMovementComponent->StopMovementImmediately();

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
	BoxComponent->SetBoxExtent(FVector(0.0f, 0.0f, 0.0f));
	ProjectileMovementComponent->SetActive(false);
	if (FlipbookComponent->GetFlipbook() == BatterSpecialFlipbook)
	{
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComponent->SetCollisionProfileName("PlayerHitbox");
		BoxComponent->SetSimulatePhysics(true);
		ProjectileMovementComponent->SetActive(true,true);
		BoxComponent->SetBoxExtent(FVector(5.0f, 1.0f, 5.0f));


		RootComponent = BoxComponent;

		ProjectileMovementComponent->UpdatedComponent = BoxComponent;
		ProjectileMovementComponent->Velocity.Z = 10000.f;
		ProjectileMovementComponent->InitialSpeed = 250.f;
		ProjectileMovementComponent->MaxSpeed = 250.f;
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
		//if (Timer < 0.0f)
			//Destroy();
	}
}

