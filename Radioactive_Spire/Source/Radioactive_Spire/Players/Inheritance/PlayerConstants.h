#pragma once

namespace PlayerConstants
{
    //DEFAULTS      ----------------------------------------------------------------------
    //sprite offset
    const FVector DefaultSpriteOffset = FVector(0, 0, 17.0f);
    const FVector DefaultSpriteCrouchOffset = FVector(0, 0, 63.0f);

    //stats
    constexpr float DefaultHealth = 100.0f;
    constexpr float DefaultDamage = 50.0f;

    //walk
    constexpr float DefaultGroundFriction = 3.5f;
    constexpr float DefaultMinSpeed = 200.0f;
    constexpr float DefaultMaxAcceleration = 900.0f;
    constexpr float DefaultMaxSpeed = 300.0f;
    constexpr float DefaultMaxStepHeight = 10.0f;

    //jump
    constexpr float DefaultGravityScale = 3.75f;
    constexpr float DefaultAirControl = 1.0f;
    constexpr float DefaultJumpZVelocity = 800.0f;
    constexpr float DefaultJumpMaxHoldTimeWalking = 0.15f;
    constexpr float DefaultJumpMaxHoldTimeRunning = 0.19f;
    constexpr float DefaultJumpIncreasePerSegment = 0.0005f;

    constexpr float DefaultInvincibleTime = 1.5f;
    constexpr float DefaultInvincibleVisibilityDuration = 0.05f;

    //collision
    constexpr float DefaultCapsuleRadius = 28.0f;
    constexpr float DefaultCapsuleHalfHeight = 75.0f;

    //attack
    constexpr float DefaultAttackCooldown = 0.3f;
    constexpr float DefaultSpecialHeldThreshold = 0.75f;
    //END DEFAULT   ----------------------------------------------------------------------

    //BATTER        ----------------------------------------------------------------------
    //sprite offset
    const FVector BatterSpriteOffset = FVector(0, 0, 26.0f);
    const FVector BatterSpriteCrouchOffset = FVector(0, 0, 63.0f);

    //stats
    constexpr float BatterHealth = 250.0f;

    constexpr float BatterSpecialAttackDamage = 10.0f;
    constexpr float BatterSpecialMaxAttackDamage = 125.0f;

    constexpr float BatterComboOneDamage = 16.0f;
    constexpr float BatterComboTwoDamage = 20.0f;
    constexpr float BatterComboThreeDamage = 24.0f;
    constexpr float BatterComboFinisherDamage = 50.0f;

    constexpr float BatterAirComboOneDamage = 26.0f;
    constexpr float BatterAirComboTwoDamage = 36.0f;

    constexpr int BatterMaxCombo = 4;
    constexpr int BatterMaxAirCombo = 1;
    constexpr float BatterSpecialLifetime = 5.5f;
    constexpr int BatterSpecialMaxBounce = 2;
    //walk
    constexpr float BatterGroundFriction = 3.5f;
    constexpr float BatterMinSpeed = 200.0f;
    constexpr float BatterMaxAcceleration = 1200.0f;
    constexpr float BatterMaxSpeed = 300.0f;
    constexpr float BatterMaxStepHeight = 10.0f;

    //jump
    constexpr float BatterGravityScale = 1.75f;
    constexpr float BatterAirControl = 0.75f;
    constexpr float BatterJumpZVelocity = 450.0f;
    constexpr float BatterJumpMaxHoldTimeWalking = 0.15f;
    constexpr float BatterJumpMaxHoldTimeRunning = 0.19f;
    constexpr float BatterJumpIncreasePerSegment = 0.0005f;
                    
    constexpr float BatterInvincibleTime = 1.5f;
    constexpr float BatterInvincibleVisibilityDuration = 0.05f;

    //collision
    constexpr float BatterCapsuleRadius = 28.0f;
    constexpr float BatterCapsuleHalfHeight = 65.0f;

    //END BATTER    ----------------------------------------------------------------------
}