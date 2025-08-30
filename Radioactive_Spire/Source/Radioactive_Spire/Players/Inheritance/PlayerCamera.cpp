// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"
#include "PlayableCharacter.h"
#include "../../Game/GameConstants.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "PaperTileMapActor.h"
#include "../../Game/RadioactiveSpire_GameModeBase.h"

APlayerCamera::APlayerCamera() :
	Camera(nullptr),
	SphereComponent(nullptr),
	Player(nullptr),
	OriginalZ(0.0f),
	LevelZIncrease(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>("CameraSphereComponent");
	SphereComponent->SetVisibility(false);
	RootComponent = SphereComponent;

	Camera = CreateDefaultSubobject<UCameraComponent>("Player Camera Component");
	Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	Camera->SetOrthoWidth(GameConstants::ResolutionWidth);
	Camera->SetAspectRatio(GameConstants::ResolutionAspectRatio);
	Camera->SetConstraintAspectRatio(true);
	Camera->SetRelativeLocation(FVector(0.0f, GameConstants::CameraDepthY, 180.0f));
	Camera->SetRelativeRotation(FQuat(FRotator(0.0f, GameConstants::CameraRotationYaw, 0.0f)));
	Camera->SetupAttachment(RootComponent);

	Tags.Add("PlayerCamera");
}

void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	Player = playerController->GetPawn<APlayableCharacter>();
	GameModeBase = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();

	playerController->SetViewTargetWithBlend(this);

	OriginalZ = Camera->GetComponentLocation().Z + GameConstants::CameraZOffset;
}

void APlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player != nullptr && GameModeBase != nullptr)
	{
		FVector playerLocation = Player->GetActorLocation();
		FVector cameraLocation = Camera->GetComponentLocation();
		if (GameModeBase->State != EGameState::EndGame)
		{
			cameraLocation.X = FMath::FInterpTo(cameraLocation.X, 500.0f/*playerLocation.X*/, DeltaTime, 8.0f) + ShakeOffset.X;

			cameraLocation.Y = FMath::FInterpTo(cameraLocation.Y, playerLocation.Y, DeltaTime, 8.0f) + GameConstants::CameraDepthY;

			cameraLocation.Z = FMath::FInterpTo(InitialLocation.Z + LevelZIncrease, playerLocation.Z, DeltaTime, 1.0f) + GameConstants::CameraZOffset + ShakeOffset.Z;
		}
		else
		{
			cameraLocation.X = FMath::FInterpTo(cameraLocation.X, playerLocation.X, DeltaTime, 8.0f) + ShakeOffset.X;

			cameraLocation.Y = FMath::FInterpTo(cameraLocation.Y, playerLocation.Y, DeltaTime, 8.0f) + GameConstants::CameraDepthY / 2.0f;

			cameraLocation.Z = FMath::FInterpTo(cameraLocation.Z, playerLocation.Z, DeltaTime, 8.0f) + ShakeOffset.Z;
		}
		Camera->SetWorldLocation(cameraLocation);

		if (LevelBackground)
			LevelBackground->SetActorLocation(FVector(cameraLocation.X - 820.0f, cameraLocation.Y - 2640.0f, cameraLocation.Z + 1840.0f));
	}
}

void APlayerCamera::ApplyCameraShake()
{
	GetWorldTimerManager().SetTimer(ShakeTimer, this, &APlayerCamera::OnShake, 0.025f, true);
	InitialLocation = Camera->GetComponentLocation();
}

void APlayerCamera::OnShake()
{
	ShakeCount++;

	ShakeOffset.X = FMath::RandRange(-2.0f, 2.0f);
	ShakeOffset.Z = FMath::RandRange(-3.0f, 3.0f);

	if (ShakeCount >= GameConstants::MaxCameraShakes)
	{
		GetWorldTimerManager().ClearTimer(ShakeTimer);
		ShakeTimer.Invalidate();
		ShakeOffset = FVector::Zero();
		ShakeCount = 0;
		Camera->SetWorldLocation(FVector(Camera->GetComponentLocation().X, Camera->GetComponentLocation().Y, OriginalZ));
	}
}
