// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API AEnemy : public APaperCharacter
{
	GENERATED_BODY()
	
public:
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPaperFlipbookComponent* FlipbookComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pawn Sensing", meta = (AllowPrivateAccess = "true"))
	class UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.0f;

protected:
	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnSeePawn(APawn* OtherPawn);
};
