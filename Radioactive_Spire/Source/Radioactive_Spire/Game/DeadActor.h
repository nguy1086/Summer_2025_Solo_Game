// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadActor.generated.h"

UCLASS()
class RADIOACTIVE_SPIRE_API ADeadActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ADeadActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Components")
	class USphereComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Components")
	class UPaperSpriteComponent* SpriteComponent;

	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	float HoldInPlaceTimer;
};
