// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inheritance/Enemy.h"
#include "Flying_Spiter.generated.h"

UENUM(BlueprintType)
enum class EFlyingSpitterState : uint8
{
	Attack	UMETA(DisplayName = "Attack"),
	Hurt	UMETA(DisplayName = "Hurt"),
	Flying	UMETA(DisplayName = "Flying")
};
/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API AFlying_Spiter : public AEnemy
{
	GENERATED_BODY()
	
public:
	AFlying_Spiter();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime flipbooks")
	class UPaperFlipbook* FlyingFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime flipbooks")
	class UPaperFlipbook* HurtFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime flipbooks")
	class UPaperFlipbook* AttackFlipbook;

	UPROPERTY(EditAnyWhere, Category = "Spitter Projectile")
	TSubclassOf<class AFlying_Spitter_Projectile> ProjectileTemplate;

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void OnDamaged(float damage) override;

private:
	void ApplyStateChange(EFlyingSpitterState newState);
	float AttackTimer;

	FTimerHandle AttackTimerHandle;
	EFlyingSpitterState State;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void UpdateFlipbook() override;
	void Attack();
};
