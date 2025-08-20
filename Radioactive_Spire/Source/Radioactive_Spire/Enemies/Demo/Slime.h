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
	Idle	UMETA(DisplayName = "Idle")
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
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnSeePawn(APawn* OtherPawn);

private:
	void ApplyStateChange(ESlimeState newState);
	float AttackTimer;
	ESlimeState State;
};
