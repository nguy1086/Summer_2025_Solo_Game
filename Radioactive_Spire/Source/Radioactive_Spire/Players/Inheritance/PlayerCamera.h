// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCamera.generated.h"

/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API APlayerCamera : public AActor
{
	GENERATED_BODY()
	
public:
	APlayerCamera();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Components")
	class USphereComponent* SphereComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Background")
	class APaperTileMapActor* LevelBackground;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ApplyCameraShake();
	float LevelZIncrease;

private:
	void OnShake();

	class APlayableCharacter* Player;
	FVector InitialLocation;
	FVector ShakeOffset;
	FTimerHandle ShakeTimer;
	int32 ShakeCount;

	float OriginalZ;
};
