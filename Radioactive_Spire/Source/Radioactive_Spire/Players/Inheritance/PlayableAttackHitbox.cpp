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

	//RootComponent->SetHiddenInGame(false);

	Tags.Add("PlayerHitbox");
}

// Called when the game starts or when spawned
void APlayableAttackHitbox::BeginPlay()
{
	Super::BeginPlay();

	PrimeHitSFX();
}

void APlayableAttackHitbox::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* enemy = Cast<AEnemy>(OtherActor);
	if (enemy)
	{
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Hit! Damage: " + FString::FromInt(Damage)) );

		enemy->OnDamaged(Damage);
		ChargeSuper();
		PlaySound();

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
			else if (OtherActor->ActorHasTag("Batter_Super"))
			{
				APlayableAttackHitbox* super = Cast<APlayableAttackHitbox>(OtherActor);
				float z = FMath::FRandRange(-5.0f, 5.0f);
				super->ProjectileMovementComponent->Velocity = FVector(400.0f * CheckDirectionOfHitbox(), 0.0f, z);
				super->Timer += 1.5f;
			}
		}
	}
}

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
		Damage = damage;
		Name = name;
		IsProjectile = true;

		InitializeProjectileFlipbook();

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
		Knockback = FVector(200.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
	}
	else if (Name == TEXT("Batter_ComboTwo"))
	{
		BoxComponent->SetBoxExtent(FVector(64.0f, 1.0f, 48.0f));
		Knockback = FVector(200.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
	}
	else if (Name == TEXT("Batter_Finisher"))
	{
		BoxComponent->SetBoxExtent(FVector(48.0f, 1.0f, 64.0f));
		FVector loc = FlipbookComponent->GetRelativeLocation();
		loc.X += 24.0f;
		FlipbookComponent->SetRelativeLocation(loc);
		FlipbookComponent->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
		Knockback = FVector(400.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

		Tags.Add("Batter_proj");
	}
	else if (Name == TEXT("Batter_AirComboOne"))
	{
		BoxComponent->SetBoxExtent(FVector(48.0f, 1.0f, 32.0f));
		Knockback = FVector(100.0f * CheckDirectionOfHitbox(), 0.0f, 150.0f);

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
	else if (Name == TEXT("Batter_Super"))
	{
		FlipbookComponent->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

		BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComponent->SetCollisionProfileName("PlayerProjectileWorldHitbox");
		ProjectileMovementComponent->SetActive(true, true);
		BoxComponent->SetBoxExtent(FVector(64.0f, 1.0f, 64.0f));

		ProjectileMovementComponent->UpdatedComponent = BoxComponent;
		ProjectileMovementComponent->Velocity.X = 400.0f * (GetActorRotation() == FRotator(0.0f, 180.0f, 0.0f) ? -1.0f : 1.0f);
		ProjectileMovementComponent->InitialSpeed = 400.0f * (GetActorRotation() == FRotator(0.0f, 180.0f, 0.0f) ? -1.0f : 1.0f);
		ProjectileMovementComponent->MaxSpeed = 400.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 2.0f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

		Tags.Add("Batter_Super");
		Tags.Add("Super");
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

void APlayableAttackHitbox::InitializeProjectileFlipbook()
{
	UPaperFlipbook* flipbook = nullptr;

	if (Name == TEXT("Batter_Special_Duck") || Name == TEXT("Batter_Special"))
	{
		flipbook = BatterSpecialFlipbook;
		Timer = PlayerConstants::BatterSpecialLifetime;//initiallifespan no work
	}
	else if (Name == TEXT("Batter_Super"))
	{
		flipbook = BatterSuperProjectileFlipbook;
		Timer = PlayerConstants::BatterSuperLifeTime;
		FlipbookComponent->SetLooping(false);
	}
	FlipbookComponent->SetFlipbook(flipbook);
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

void APlayableAttackHitbox::ChargeSuper()
{
	APlayableCharacter* player = GetWorld()->GetFirstPlayerController()->GetPawn<APlayableCharacter>();
	if (player->Stats_Super < player->Stats_MaxSuper)
	{
		if (player->Type == EPlayerType::Test)
			player->Stats_Super += Damage * PlayerConstants::DefaultSuperChargeRate;
		else if (player->Type == EPlayerType::Batter)
			player->Stats_Super += (Damage * PlayerConstants::BatterSuperChargeRate) / (ActorHasTag("Super") ? 2.0f : 1.0f);
	}
}

void APlayableAttackHitbox::PlaySound()
{
	if ((Name == TEXT("Batter_Finisher") || Name == TEXT("Batter_AirSpecial")))
	{
		if (HitSpecialSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, HitSpecialSound, GetActorLocation());
	}
	else if (Name == TEXT("Batter_ComboOne") || Name == TEXT("Batter_AirComboOne"))
	{
		if (HitOneSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, HitOneSound, GetActorLocation());
	}
	else if (Name == TEXT("Batter_ComboTwo") || Name == TEXT("Batter_AirComboTwo"))
	{
		if (HitTwoSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, HitTwoSound, GetActorLocation());
	}
	else if (Name == TEXT("Batter_Finisher") || Name == TEXT("Batter_AirSpecial"))
	{
		if (HitThreeSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, HitThreeSound, GetActorLocation());
	}
	else if (Name == TEXT("Batter_Super"))
	{
		if (HitSuperSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(this, HitSuperSound, GetActorLocation());
	}
}

void APlayableAttackHitbox::PrimeHitSFX()
{
	if (HitSpecialSound != nullptr)
		UGameplayStatics::PrimeSound(HitSpecialSound);
	if (HitOneSound != nullptr)
		UGameplayStatics::PrimeSound(HitOneSound);
	if (HitTwoSound != nullptr)
		UGameplayStatics::PrimeSound(HitTwoSound);
	if (HitThreeSound != nullptr)
		UGameplayStatics::PrimeSound(HitThreeSound);
	if (HitSuperSound != nullptr)
		UGameplayStatics::PrimeSound(HitSuperSound);
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

