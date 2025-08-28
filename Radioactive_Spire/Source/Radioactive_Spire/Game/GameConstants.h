#pragma once

namespace GameConstants
{
	// res and cam
	constexpr float ResolutionWidth = 1280.0f;
	constexpr float ResolutionHeight = 720.0f;
	constexpr float ResolutionAspectRatio = ResolutionWidth / ResolutionHeight;
	constexpr float CameraDepthY = 200.0f;
	constexpr float CameraRotationYaw = 270.0f;
	constexpr int32 MaxCameraShakes = 9;
	constexpr float CameraZOffset = 256.0f;
	constexpr float BlackOverlayFadeInOutDuration = 0.7f;
	constexpr float BlackOverlayFadeHoldDuration = 0.1f;

	// dead
	constexpr float DeadHoldTime = 1.5f;
	constexpr float DeadImpulseZ = 600.0f;

	// states
	const float WaitMax = 5.0f;

	// levels
	const float LevelPosYIncrement = 128.0f;
	const float LevelPosZIncrement = 32.0f;
}