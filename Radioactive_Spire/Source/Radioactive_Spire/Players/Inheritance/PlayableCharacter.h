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
	void SuperAttack();

	void Roll();

	void ApplyStateChange(EState newState);

	UFUNCTION()
	void ApplyImpulse(FVector impulse);

	void HandleDamage(float damage, AActor* OtherActor);
	void EnemyKnockback(class AEnemy* OtherEnemy);
	float GetInvincibilityTimer();
	bool IsInvincible();
	void NoGravity();
	void SetGravity();
	void ZeroVelocity();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Stats_Health = PlayerConstants::DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Stats_MaxHealth = PlayerConstants::DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Stats_Super = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Stats_MaxSuper = PlayerConstants::DefaultMaxSuper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EPlayerType Type;

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
	bool GroundPound;
	float CanDash;

	FTimerHandle AttackTimerHandle;
	FTimerHandle InputTimerHandle;
	FTimerHandle StateTimerHandle;
	FTimerHandle GravityTimerHandle;
	FTimerHandle ImpulseTimerHandle;
	FTimerHandle PauseSpriteTimerHandle;

	class UPaperFlipbook* GetTestFlipbook();
	class UPaperFlipbook* GetBatterFlipbook();

	void InitializeType();
	void PrimeSFX();

	void EnableControls();
	void DisableControls();
	void ResetPlayerState();
	void PauseSprite();
	void UnPauseSprite();
	FVector HandleMeleeHitBoxLocation(float OffsetX, float Impulse);
	FRotator HandleMeleeHitBoxRotation();

	void BatterSpecialSpawn();
	void BatterComboAttackSpawn();
	void BatterGroundPoundSpawn();
	void BatterSuperSpawn();

	//batter
	bool DuckSpecial;

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
	class UPaperFlipbook* BatterAIRAttackOneFlipbook;//AIRATK1
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterAIRAttackTwoFlipbook;//AIRATK2
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterSpecialFlipbook;//SPECIAL
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterAIRSpecialFlipbook;//AIRSPECIAL
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterSpecialDuckFlipbook;//SPECIAL DUCK
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterGroundPoundFlipbook;//GROUND POUND
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterRollFlipbook;//ROLL
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterHurtFlipbook;//HURT

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterSuperFlipbook;//SUPER

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter flipbooks")
	class UPaperFlipbook* BatterDeadFlipbook;//DEAD

	//Attacks
	UPROPERTY(EditAnyWhere, Category = "Hitbox attack")
	TSubclassOf<class APlayableAttackHitbox> AttackHitboxTemplate;

	//SFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* SwingOneSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* SwingTwoSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* SwingThreeSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* SuperSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* SpecialSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* AirSpecialSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* GroundPoundSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Batter SFX")
	class USoundBase* HurtSound;
};
