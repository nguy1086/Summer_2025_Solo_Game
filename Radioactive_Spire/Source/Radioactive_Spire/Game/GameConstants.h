#pragma once

namespace GameConstants
{
	// res and cam
	constexpr float ResolutionWidth = 640.0f;
	constexpr float ResolutionHeight = 480.0f;
	constexpr float ResolutionAspectRatio = ResolutionWidth / ResolutionHeight;
	constexpr float CameraDepthY = 200.0f;
	constexpr float CameraRotationYaw = 270.0f;
	constexpr int32 MaxCameraShakes = 9;

	// dead
	constexpr float DeadHoldTime = 1.5f;
	constexpr float DeadImpulseZ = 600.0f;
}