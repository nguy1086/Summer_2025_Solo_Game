// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "MainMenu_GameModeBase.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/CanvasPanelSlot.h"

#include "Kismet/GameplayStatics.h"
#include "MainMenuController.h"
#include "Math/UnrealMathUtility.h"

bool UMainMenuWidget::Initialize()
{
    GameModeBase = GetWorld()->GetAuthGameMode<AMainMenu_GameModeBase>();
    State = EMainMenuState::MainMenu;
    Increment = 0;
    bIsFocusable = true;
    SetKeyboardFocus();

    bool bResult = Super::Initialize();
    if (!bResult)
        return false;

    UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GetWidgetFromName("MainCanvas"));
    if (CanvasSlot)
        CanvasSlot->SetPosition(FVector2D(0.0f, 0.0f));

    UImage* Image = Cast<UImage>(GetWidgetFromName("Intro"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    Image = Cast<UImage>(GetWidgetFromName("MainMenu"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    Image = Cast<UImage>(GetWidgetFromName("Options"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    Image = Cast<UImage>(GetWidgetFromName("CharacterSelect"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    Image = Cast<UImage>(GetWidgetFromName("GameName"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);

    UButton* Button = Cast<UButton>(GetWidgetFromName("PlayButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCharacterSelect);
        MainMenuButtons.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("OptionsButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMainMenuOptions);
        MainMenuButtons.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("QuitButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMainMenuQuit);
        MainMenuButtons.Add(Button);
    }

    return true;
}

void UMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (State == EMainMenuState::MainMenu)
    {
        if (GEngine)
	        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Main Menu!"));

        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GetWidgetFromName("MainCanvas"));
        if (CanvasSlot)
        {
            float x = FMath::FInterpTo(0.0f, CanvasSlot->GetPosition().X, InDeltaTime, 8.0f);
            float y = FMath::FInterpTo(0.0f, CanvasSlot->GetPosition().Y, InDeltaTime, 8.0f);

            CanvasSlot->SetPosition(FVector2D(x, y));
        }

        FSlateBrush brush;
        UTexture2D* NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/UI/Game_ButtonHover.Game_ButtonHover")));

        brush.SetResourceObject(NormalTexture);
        brush.TintColor = FSlateColor(brush.TintColor = FSlateColor(FLinearColor(0.724268f, 0.724268f, 0.724268f, 1.0f)));
        FButtonStyle style;
        style.SetNormal(brush);
        style.SetHovered(brush);

        NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/UI/Game_ButtonPressed.Game_ButtonPressed")));
        brush.SetResourceObject(NormalTexture);
        style.SetPressed(brush);
        MainMenuButtons[Increment]->SetStyle(style);

        for (int i = 0; i < MainMenuButtons.Num(); i++)
        {
            if (MainMenuButtons[i]->IsHovered())
                Increment = i;
            if (i != Increment)
            {
                NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/UI/Game_Button.Game_Button")));
                brush.SetResourceObject(NormalTexture);
                brush.TintColor = FSlateColor(brush.TintColor = FSlateColor(FLinearColor(0.495466f, 0.495466f, 0.495466f, 1.0f)));
                style.SetNormal(brush);
                MainMenuButtons[i]->SetStyle(style);
            }
        }
    }
    else if (State == EMainMenuState::Options)
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Options!"));

        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GetWidgetFromName("MainCanvas"));
        if (CanvasSlot)
        {
            float x = FMath::FInterpTo(0.0f, CanvasSlot->GetPosition().X, InDeltaTime, 8.0f);
            float y = FMath::FInterpTo(-1920.0f, CanvasSlot->GetPosition().Y, InDeltaTime, 8.0f);

            CanvasSlot->SetPosition(FVector2D(x, y));
        }
    }
    else if (State == EMainMenuState::Character)
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Character Select!"));

        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(GetWidgetFromName("MainCanvas"));
        if (CanvasSlot)
        {
            float x = FMath::FInterpTo(1080.0f, CanvasSlot->GetPosition().X, InDeltaTime, 8.0f);
            float y = FMath::FInterpTo(0.0f, CanvasSlot->GetPosition().Y, InDeltaTime, 8.0f);

            CanvasSlot->SetPosition(FVector2D(x, y));
        }
    }
}

void UMainMenuWidget::UpdateIntro()
{
}

void UMainMenuWidget::UpdateMainMenu()
{
}

void UMainMenuWidget::UpdateOptions()
{
}

void UMainMenuWidget::UpdateCharacterSelect()
{
}

void UMainMenuWidget::OnCharacterSelect()
{
    if (State == EMainMenuState::MainMenu || State == EMainMenuState::Options)
        State = EMainMenuState::Character;

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Character Select!"));
}

void UMainMenuWidget::OnMainMenuOptions()
{
    if (State == EMainMenuState::MainMenu || State == EMainMenuState::Character)
        State = EMainMenuState::Options;

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Options!"));
}

void UMainMenuWidget::OnMainMenuQuit()
{

    AMainMenuController* PlayableController = Cast<AMainMenuController>(GameModeBase->GetInstigatorController());
    UKismetSystemLibrary::QuitGame(GetWorld(), PlayableController, EQuitPreference::Quit, true);
}

void UMainMenuWidget::MainMenuNavigation(float dir)
{
    Increment += dir;
    if (Increment < 0)
        Increment = MainMenuButtons.Num() - 1;
    else if (Increment >= MainMenuButtons.Num())
        Increment = 0;
}

void UMainMenuWidget::MainMenuPressed()
{
    MainMenuButtons[Increment]->OnClicked.Broadcast();
}

void UMainMenuWidget::MainMenuBackPressed()
{
    if (State == EMainMenuState::Character || State == EMainMenuState::Options)
        State = EMainMenuState::MainMenu;

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Main Menu!"));
}
