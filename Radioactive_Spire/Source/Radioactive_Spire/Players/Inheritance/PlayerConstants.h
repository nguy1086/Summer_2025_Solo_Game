#pragma once

namespace PlayerConstants
{
    //DEFAULTS      ----------------------------------------------------------------------
    //stats
    constexpr float DefaultHealth = 100.0f;

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
    //END DEFAULT   ----------------------------------------------------------------------
}