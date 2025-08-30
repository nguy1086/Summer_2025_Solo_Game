// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuController.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainMenu_GameModeBase.h"
#include "MainMenuWidget.h"

AMainMenuController::AMainMenuController() :
    InputMappingContext(nullptr),
    NavigationInputAction(nullptr),
    SelectInputAction(nullptr),
    BackInputAction(nullptr),
    InputSubsystem(nullptr)
{
    Tags.Add("Controller");
}

void AMainMenuController::BeginPlay()
{
    Super::BeginPlay();

    InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (InputSubsystem != nullptr)
        InputSubsystem->AddMappingContext(InputMappingContext, 0);

    if (GameInfoBP != nullptr && IsLocalController())
    {
        GameInfoWidget = CreateWidget<UMainMenuWidget>(this, GameInfoBP);
        GameInfoWidget->AddToViewport();
    }
    GameModeBase = GetWorld()->GetAuthGameMode<AMainMenu_GameModeBase>();
}

void AMainMenuController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    if (EnhancedInputComponent != nullptr)
    {
        EnhancedInputComponent->BindAction(NavigationInputAction, ETriggerEvent::Started, this, &AMainMenuController::OnNavigation);

        EnhancedInputComponent->BindAction(SelectInputAction, ETriggerEvent::Started, this, &AMainMenuController::OnSelectPressed);

        EnhancedInputComponent->BindAction(BackInputAction, ETriggerEvent::Started, this, &AMainMenuController::OnBackPressed);
    }
}

float AMainMenuController::GetNavigationValue()
{
    if (InputSubsystem != nullptr)
    {
        FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(NavigationInputAction);
        return InputActionValue.Get<float>();
    }
    return 0.0f;
}

bool AMainMenuController::IsSelectPressed()
{
    if (InputSubsystem != nullptr)
    {
        FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(SelectInputAction);
        return InputActionValue.Get<bool>();
    }
    return false;
}

bool AMainMenuController::IsBackPressed()
{
    if (InputSubsystem != nullptr)
    {
        FInputActionValue InputActionValue = InputSubsystem->GetPlayerInput()->GetActionValue(BackInputAction);
        return InputActionValue.Get<bool>();
    }
    return false;
}

void AMainMenuController::OnNavigation(const FInputActionValue& Value)
{
    float Direction = Value.Get<float>();

    if (GameInfoWidget)
        GameInfoWidget->MainMenuNavigation(Direction);
}

void AMainMenuController::OnSelectPressed(const FInputActionValue& Value)
{
    if (GameInfoWidget)
        GameInfoWidget->MainMenuPressed();
}

void AMainMenuController::OnBackPressed(const FInputActionValue& Value)
{
    if (GameInfoWidget)
        GameInfoWidget->MainMenuBackPressed();
}
