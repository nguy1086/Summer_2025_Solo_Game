// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableAttackHitbox.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "../../Enemies/Inheritance/Enemy.h"
#include "PlayableCharacter.h"
#include "PlayableCharacterState.h"

// Sets default values
APlayableAttackHitbox::APlayableAttackHitbox() :
	IsProjectile(false),
	Timer(0.0f),
	Ricochet(0),
	Knockback(0.0f, 0.0f, 75.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->BodyInstance.bLockYRotation = true;
	BoxComponent->BodyInstance.bLockZRotation = true;
	//BoxComponent->OnComponentHit.AddDynamic(this, &APlayableAttackHitbox::OnHit);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayableAttackHitbox::OnOverlapBegin);
	RootComponent = BoxComponent;

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("HitboxFlipbook");
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetCollisionProfileName("NoCollision");
	FlipbookComponent->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bSimulationEnabled = true;

	RootComponent->SetHiddenInGame(false);

	Tags.Add("PlayerHitbox");
}

// Called when the game starts or when spawned
void APlayableAttackHitbox::BeginPlay()
{
	Super::BeginPlay();
}

void APlayableAttackHitbox::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//OnHit(OverlapComponent, OtherActor, OtherComp, FVector::ZeroVector, SweepResult);
	AEnemy* enemy = Cast<AEnemy>(OtherActor);
	if (enemy)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Hit! Damage: " + FString::FromInt(Damage)) );

		enemy->OnDamaged(Damage);

		if (ActorHasTag("Baseball"))
			Destroy();
		else if (ActorHasTag("GroundPound"))
		{
			FVector loc = enemy->GetActorLocation();
			if (loc.X < GetActorLocation().X)
				Knockback.X *= -1.0f;
		}

		if (!enemy->ActorHasTag("Unknockable"))
			enemy->LaunchCharacter(Knockback, false, true);


	}
	else
	{
		if (IsProjectile)
		{
			//if (Name == TEXT("Batter_Special_Duck") || Name == TEXT("Batter_Special"))
			//{
			//	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && (OtherActor != this->GetOwner()))
			//	{
			//		ProjectileMovementComponent->StopMovementImmediately();
			//		if (OtherActor->ActorHasTag("Enemy"))
			//		{
			//			Damage *= (ProjectileMovementComponent->Velocity.Size() * 0.5f);//velocity.size() gets the current speed
			//			Destroy();
			//		}
			//		else
			//		{
			//			Ricochet++;
			//			Damage = Damage / 2.0f;

			//			if (Ricochet >= PlayerConstants::BatterSpecialMaxBounce)
			//			{
			//				Destroy();
			//			}
			//		}
			//	}
			//}


		}
		else
		{
			if (ActorHasTag("Batter_proj"))
			{
				if (OtherActor->ActorHasTag("Baseball"))
				{
					APlayableAttackHitbox* baseball = Cast<APlayableAttackHitbox>(OtherActor);
					float z = FMath::FRandRange(-2.0f, 2.0f);
					baseball->ProjectileMovementComponent->MaxSpeed = 3000.0f;
					baseball->ProjectileMovementComponent->Velocity = FVector(3000.0f * CheckDirectionOfHitbox(), 0.0f, z);
					baseball->Damage *= 5.5f;
					baseball->Knockback.Z *= 3.0f;
				}
			}
		}
	}

}

//void APlayableAttackHitbox::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//
//}

void APlayableAttackHitbox::Spawn(FString name, float damage)
{
	if (!name.IsEmpty() && FlipbookComponent)
	{
		Damage = damage;
		Name = name;

		InitializeMeleeFlipbook();

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
		Name = name;
		IsProjectile = true;

		if (name == TEXT("Batter_Special_Duck") || name == TEXT("Batter_Special"))
		{
			flipbook = BatterSpecialFlipbook;
			Timer = PlayerConstants::BatterSpecialLifetime;//initiallifespan no work
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
	BoxComponent->SetCollisionProfileName("PlayerHitbox");
	BoxComponent->SetBoxExtent(FVector(0.0f, 0.0f, 0.0f));
	ProjectileMovementComponent->SetActive(false);
	if (Name == TEXT("Batter_Special_Duck"))
	{
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComponent->SetCollisionProfileName("PlayerProjectileWorldHitbox");
		//BoxComponent->SetSimulatePhysics(false);
		ProjectileMovementComponent->SetActive(true,true);
		BoxComponent->SetBoxExtent(FVector(5.0f, 1.0f, 5.0f));

		ProjectileMovementComponent->UpdatedComponent = BoxComponent;
		ProjectileMovementComponent->Velocity.Z = 1000.0f;
		ProjectileMovementComponent->InitialSpeed = 750.0f;
		ProjectileMovementComponent->MaxSpeed = 750.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.5f;
		ProjectileMovementComponent->ProjectileGravityScale = 1.0f;

		Tags.Add("Baseball");
	}
	else if (Name == TEXT("Batter_Special"))
	{
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComponent->SetCollisionProfileName("PlayerProjectileWorldHitbox");
		//BoxComponent->SetSimulatePhysics(false);
		ProjectileMovementComponent->SetActive(true, true);
		BoxComponent->SetBoxExtent(FVector(5.0f, 1.0f, 5.0f));

		ProjectileMovementComponent->UpdatedComponent = BoxComponent;
		ProjectileMovementComponent->Velocity.X = 1000.0f * (GetActorRotation() == FRotator(0.0f,180.0f,0.0f) ? -1.0f : 1.0f);//NOT THE BEST AT CHECKING WILL PROBABLY CHANGED THIS LATER
		ProjectileMovementComponent->InitialSpeed = 1250.0f * (GetActorRotation() == FRotator(0.0f, 180.0f, 0.0f) ? -1.0f : 1.0f);
		ProjectileMovementComponent->MaxSpeed = 1250.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.5f;
		ProjectileMovementComponent->ProjectileGravityScale = 1.0f;

		Tags.Add("Baseball");
	}
	else if (Name == TEXT("Batter_ComboOne"))
	{
		BoxComponent->SetBoxExtent(FVector(48.0f, 1.0f, 32.0f));
		Knockback = FVector(100.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
	}
	else if (Name == TEXT("Batter_ComboTwo"))
	{
		BoxComponent->SetBoxExtent(FVector(64.0f, 1.0f, 48.0f));
		Knockback = FVector(100.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
	}
	else if (Name == TEXT("Batter_Finisher"))
	{
		BoxComponent->SetBoxExtent(FVector(48.0f, 1.0f, 64.0f));
		FVector loc = FlipbookComponent->GetRelativeLocation();
		loc.X += 24.0f;
		FlipbookComponent->SetRelativeLocation(loc);
		FlipbookComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		Knockback = FVector(200.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
	}
	else if (Name == TEXT("Batter_AirComboOne"))
	{
		BoxComponent->SetBoxExtent(FVector(48.0f, 1.0f, 32.0f));
		Knockback = FVector(0.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
		Tags.Add("Air");
	}
	else if (Name == TEXT("Batter_AirComboTwo"))
	{
		BoxComponent->SetBoxExtent(FVector(48.0f, 1.0f, 64.0f));
		Knockback = FVector(50.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
		Tags.Add("Air");
	}
	else if (Name == TEXT("Batter_AirSpecial"))
	{
		BoxComponent->SetBoxExtent(FVector(54.0f, 1.0f, 32.0f));
		Knockback = FVector(100.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
		Tags.Add("Air");
	}
	else if (Name == TEXT("Batter_GroundPound"))
	{
		FVector loc = FlipbookComponent->GetRelativeLocation();
		loc.Z += 52.0f;
		FlipbookComponent->SetRelativeLocation(loc);
		BoxComponent->SetBoxExtent(FVector(96.0f, 1.0f, 48.0f));
		Knockback = FVector(300.0f, 0.0f, 750.0f);

		Tags.Add("GroundPound");
	}
}

void APlayableAttackHitbox::InitializeMeleeFlipbook()
{
	UPaperFlipbook* flipbook = nullptr;


	if (Name == TEXT("Test_Basic"))
		flipbook = TestBasicFlipbook;
	//BATTER
	else if (Name == TEXT("Batter_ComboOne") || Name == TEXT("Batter_ComboTwo") || Name == TEXT("Batter_AirComboOne") || Name == TEXT("Batter_AirComboTwo"))
		flipbook = FrameTwoFlipbook;
	else if (Name == TEXT("Batter_Finisher") || Name == TEXT("Batter_AirSpecial"))
		flipbook = BatterFinisherFlipbook;
	else if (Name == TEXT("Batter_GroundPound"))
		flipbook = BatterGroundPoundFlipbook;

	FlipbookComponent->SetFlipbook(flipbook);
	FlipbookComponent->SetLooping(false);
}

float APlayableAttackHitbox::CheckDirectionOfHitbox()
{
	APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
	APlayableCharacterState* PlayerState = player->GetPlayerState<APlayableCharacterState>();
	if (PlayerState->Direction == EDirection::Left)
		return -1.0f;
	else if (PlayerState->Direction == EDirection::Right)
		return 1.0f;
	
	return 0.0f;
}

// Called every frame
void APlayableAttackHitbox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsProjectile)
	{
		Timer -= DeltaTime;
		if (Timer < 0.0f)
			Destroy();
	}
}

