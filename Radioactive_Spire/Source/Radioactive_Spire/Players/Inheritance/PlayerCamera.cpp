// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCamera.h"
#include "PlayableCharacter.h"
#include "../../Game/GameConstants.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

APlayerCamera::APlayerCamera() :
	Camera(nullptr),
	SphereComponent(nullptr),
	Player(nullptr),
	OriginalZ(0.0f)
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
	Camera->SetRelativeLocation(FVector(0.0f, GameConstants::CameraDepthY, -80.0f));
	Camera->SetRelativeRotation(FQuat(FRotator(0.0f, GameConstants::CameraRotationYaw, 0.0f)));
	Camera->SetupAttachment(RootComponent);

	Tags.Add("PlayerCamera");
}

void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	Player = playerController->GetPawn<APlayableCharacter>();

	playerController->SetViewTargetWithBlend(this);

	OriginalZ = Camera->GetComponentLocation().Z - 32.0f;
}

void APlayerCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player != nullptr)
	{
		// get player and the camera locations
		FVector marioLocation = Player->GetActorLocation();
		FVector cameraLocation = Camera->GetComponentLocation();

		// set the X value
		cameraLocation.X = FMath::FInterpTo(cameraLocation.X, marioLocation.X, DeltaTime, 8.0f) + ShakeOffset.X;

		// set the Z value
		cameraLocation.Z = InitialLocation.Z + ShakeOffset.Z;

		// set the camera location
		Camera->SetWorldLocation(FVector(cameraLocation.X, Camera->GetComponentLocation().Y, cameraLocation.Z));
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
	ShakeOffset.Z = FMath::RandRange(-5.0f, 5.0f);

	if (ShakeCount >= GameConstants::MaxCameraShakes)
	{
		GetWorldTimerManager().ClearTimer(ShakeTimer);
		ShakeTimer.Invalidate();
		ShakeOffset = FVector::Zero();
		ShakeCount = 0;
		Camera->SetWorldLocation(FVector(Camera->GetComponentLocation().X, Camera->GetComponentLocation().Y, OriginalZ));
	}
}
