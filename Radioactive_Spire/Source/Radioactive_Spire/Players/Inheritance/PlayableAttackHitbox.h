// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerConstants.h"
#include "PlayableAttackHitbox.generated.h"

UCLASS()
class RADIOACTIVE_SPIRE_API APlayableAttackHitbox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayableAttackHitbox();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPaperFlipbookComponent* FlipbookComponent;
	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test flipbooks")
	class UPaperFlipbook* TestBasicFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterSpecialFlipbook;

	virtual void Tick(float DeltaTime) override;

	void Spawn(FString name, float damage);
	void Projectile(FString name, float damage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION()
	void OnFinishPlaying();

	void InitializeHitbox();

private:
	void DestroyBox();
	void DestroySphere();
	bool IsProjectile;
	float Timer;
	float Damage;
	FTimerHandle HitboxTimerHandle;
};
