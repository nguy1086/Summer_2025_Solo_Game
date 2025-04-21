// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PlayerConstants.h"
#include "PlayableCharacter.generated.h"

enum class EState : uint8;
enum class EPlayerType : uint8;
/**
 * 
 */
UCLASS()
class RADIOACTIVE_SPIRE_API APlayableCharacter : public APaperCharacter
{
	GENERATED_BODY()
	
public:
	APlayableCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void Duck();
	void StopDucking();

	void ApplyStateChange(EState newState);
	void ApplyBounce();

	void HandleDamage(float damage);
	bool IsInvincible();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = PlayerConstants::DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EPlayerType Type;

protected:
	void OnJumped_Implementation() override;
	void NotifyJumpApex() override;
	void Landed(const FHitResult& Hit) override;

	void UpdateFlipbook();

private:
	void Death(bool spawnDeathAnimation);

	class APlayableCharacterState* PlayerState;
	class APlayerCamera* Camera;
	float DamagedTimer;

	class UPaperFlipbook* GetTestFlipbook();

public:
	//Test
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test flipbooks")
	class UPaperFlipbook* TestIdleFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test flipbooks")
	class UPaperFlipbook* TestWalkFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test flipbooks")
	class UPaperFlipbook* TestJumpFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test flipbooks")
	class UPaperFlipbook* TestFallingFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test flipbooks")
	class UPaperFlipbook* TestDuckFlipbook;
};
