// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Inheritance/Enemy.h"
#include "Slime.generated.h"

UENUM(BlueprintType)
enum class ESlimeState : uint8
{
	Attack	UMETA(DisplayName = "Attack"),
	Hurt	UMETA(DisplayName = "Hurt"),
	Walk	UMETA(DisplayName = "Walk")
};
/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API ASlime : public AEnemy
{
	GENERATED_BODY()

public:
	ASlime();
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime flipbooks")
	class UPaperFlipbook* IdleFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime flipbooks")
	class UPaperFlipbook* HurtFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slime flipbooks")
	class UPaperFlipbook* AttackFlipbook;

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void OnDamaged(float damage) override;
	void Landed(const FHitResult& Hit) override;

private:
	void ApplyStateChange(ESlimeState newState);
	float AttackTimer;

	FTimerHandle AttackTimerHandle;
	ESlimeState State;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void UpdateFlipbook() override;
	void Attack();
};
