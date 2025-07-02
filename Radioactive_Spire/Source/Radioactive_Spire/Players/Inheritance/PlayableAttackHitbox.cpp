// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableAttackHitbox.h"
#include "Components/BoxComponent.h"

#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayableAttackHitbox::APlayableAttackHitbox() :
	Timer(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FlipbookComponent = CreateDefaultSubobject<UPaperFlipbookComponent>("HitboxFlipbook");
	FlipbookComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FlipbookComponent->SetCollisionProfileName("NoCollision");
	FlipbookComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayableAttackHitbox::BeginPlay()
{
	Super::BeginPlay();
}

void APlayableAttackHitbox::OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void APlayableAttackHitbox::Spawn(FString name)
{
	if (!name.IsEmpty() && FlipbookComponent)
	{
		UPaperFlipbook* flipbook = nullptr;

		if (name == TEXT("Test_Basic"))
		{
			flipbook = TestBasicFlipbook;
		}

		FlipbookComponent->SetFlipbook(flipbook);
		FlipbookComponent->SetLooping(false);

		//FlipbookComponent->OnFinishedPlaying.AddDynamic(this, &APlayableAttackHitbox::OnFinishPlaying());
		GetWorldTimerManager().SetTimer(MyTimerHandle, this, &APlayableAttackHitbox::OnFinishPlaying,FlipbookComponent->GetFlipbookLength(), true);

		FlipbookComponent->PlayFromStart();
	}
}

void APlayableAttackHitbox::OnFinishPlaying()
{
	Destroy();
}

// Called every frame
void APlayableAttackHitbox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

