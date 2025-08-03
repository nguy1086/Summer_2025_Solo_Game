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

	void Attack();
	void Special();

	void ApplyStateChange(EState newState);
	void ApplyBounce();

	void HandleDamage(float damage);
	bool IsInvincible();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = PlayerConstants::DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EPlayerType Type;

	bool SpecialHeld;

protected:
	void OnJumped_Implementation() override;
	void NotifyJumpApex() override;
	void Landed(const FHitResult& Hit) override;

	void UpdateFlipbook();

private:
	void Death();

	class APlayableCharacterState* PlayerState;
	class APlayerCamera* Camera;
	class APlayableController* PlayableController;
	float DamagedTimer;
	int ComboNumber;
	FTimerHandle AttackTimerHandle;
	FTimerHandle InputTimerHandle;
	FTimerHandle StateTimerHandle;

	class UPaperFlipbook* GetTestFlipbook();
	class UPaperFlipbook* GetBatterFlipbook();

	void InitializeType();

	void EnableControls();
	void DisableControls();
	void ResetPlayerState();

	void BatterSpecialSpawn();
	void BatterComboAttackSpawn();
	float SpecialTimer;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test flipbooks")
	class UPaperFlipbook* TestAttackFlipbook;

	//Batter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterIdleFlipbook;//IDLE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterWalkFlipbook;//WALK
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterJumpFlipbook;//JUMP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterFallingFlipbook;//FALL
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterDuckFlipbook;//DUCK
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterAttackOneFlipbook;//ATK1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterAttackTwoFlipbook;//ATK2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterFinisherFlipbook;//FINISHER
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterSpecialFlipbook;//SPECIAL STARTUP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterSpecialPressFlipbook;//SPECIAL PRESS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterSpecialHoldFlipbook;//SPECIAL HOLD

	//Test Attacks
	UPROPERTY(EditAnyWhere, Category = "Test attack")
	TSubclassOf<class APlayableAttackHitbox> AttackHitboxTemplate;
};
