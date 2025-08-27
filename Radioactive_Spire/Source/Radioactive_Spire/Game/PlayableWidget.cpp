// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"
#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"

bool UPlayableWidget::Initialize()
{
    bool bResult = Super::Initialize();
    if (!bResult)
        return false;
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("HealthText"));
    if (Widget)
        Widget->SetText(FText::FromString("Health"));
    Widget = Cast<UTextBlock>(GetWidgetFromName("SuperText"));
    if (Widget)
        Widget->SetText(FText::FromString("Super"));
    Widget = Cast<UTextBlock>(GetWidgetFromName("EnemiesLeftText"));
    if (Widget)
        Widget->SetText(FText::FromString("Enemies Left"));
    Widget = Cast<UTextBlock>(GetWidgetFromName("EnemiesLeftNumber"));
    if (Widget)
        Widget->SetText(FText::FromString("0"));
    Widget = Cast<UTextBlock>(GetWidgetFromName("LocationText"));
    if (Widget)
        Widget->SetText(FText::FromString("Location"));
    Widget = Cast<UTextBlock>(GetWidgetFromName("LocationWorld"));
    if (Widget)
        Widget->SetText(FText::FromString("DEMO"));

    UProgressBar* Bar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));
    if (Bar)
        Bar->SetPercent(1.0f);
    Bar = Cast<UProgressBar>(GetWidgetFromName("SuperBar"));
    if (Bar)
    {
        Bar->SetPercent(1.0f);
        Bar->SetFillColorAndOpacity(FLinearColor(0.99f, 0.815f, 0.09f, 1.0f));
    }

    UImage* Image = Cast<UImage>(GetWidgetFromName("InGameUI"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);

    Image = Cast<UImage>(GetWidgetFromName("PauseScreen"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Hidden);


    return true;
}

void UPlayableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateHealth();
    UpdateSuper();
    UpdatePause();
    DisplayGameOver();
}

void UPlayableWidget::UpdateHealth()
{
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* playerPawn = pController->GetPawn();
    APlayableCharacter* Player = Cast<APlayableCharacter>(playerPawn);
    if (Player)
    {
        FString Value = FString::Printf(TEXT("%d / %d"), (int)Player->Stats_Health, (int)Player->Stats_MaxHealth);
        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("HealthText"));
        if (Widget != nullptr)
            Widget->SetText(FText::FromString(Value));

        UProgressBar* Bar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));
        if (Bar != nullptr)
        {
            Bar->SetPercent((Player->Stats_Health / Player->Stats_MaxHealth));
            if (Player->Type == EPlayerType::Batter)
                Bar->SetFillColorAndOpacity(FLinearColor(0.37f, 0.0f, 0.63f, 1.0f));
        }
    }
}

void UPlayableWidget::UpdateSuper()
{
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* playerPawn = pController->GetPawn();
    APlayableCharacter* Player = Cast<APlayableCharacter>(playerPawn);
    if (Player)
    {
        FString Value = FString::Printf(TEXT("%d / %d"), (int)Player->Stats_Super, (int)Player->Stats_MaxSuper);
        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("SuperText"));
        if (Widget != nullptr)
        {
            if (Value == TEXT("1"))
                Widget->SetText(FText::FromString("SUPER"));
            else
                Widget->SetText(FText::FromString(Value));
        }

        UProgressBar* Bar = Cast<UProgressBar>(GetWidgetFromName("SuperBar"));
        if (Bar != nullptr)
        {
            Bar->SetPercent((Player->Stats_Super / Player->Stats_MaxSuper));
        }
    }
}

void UPlayableWidget::UpdatePause()
{
    ARadioactiveSpire_GameModeBase* GameModeBase = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
    if (GameModeBase && GameModeBase->Game_IsPaused)
    {
        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("HealthText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("SuperText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("EnemiesLeftText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("EnemiesLeftNumber"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("LocationText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("LocationWorld"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        UProgressBar* Bar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));
        if (Bar)
            Bar->SetVisibility(ESlateVisibility::Hidden);
        Bar = Cast<UProgressBar>(GetWidgetFromName("SuperBar"));
        if (Bar)
            Bar->SetVisibility(ESlateVisibility::Hidden);
        UImage* Image = Cast<UImage>(GetWidgetFromName("InGameUI"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Hidden);
        //-----------------------------------------------------------------------
        Image = Cast<UImage>(GetWidgetFromName("PauseScreen"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Visible);
    }
    else if (GameModeBase && !GameModeBase->Game_IsPaused)
    {
        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("HealthText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("SuperText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("EnemiesLeftText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("EnemiesLeftNumber"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("LocationText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("LocationWorld"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        UProgressBar* Bar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));
        if (Bar)
            Bar->SetVisibility(ESlateVisibility::Visible);
        Bar = Cast<UProgressBar>(GetWidgetFromName("SuperBar"));
        if (Bar)
            Bar->SetVisibility(ESlateVisibility::Visible);
        UImage* Image = Cast<UImage>(GetWidgetFromName("InGameUI"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Visible);
        //-----------------------------------------------------------------------
        Image = Cast<UImage>(GetWidgetFromName("PauseScreen"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UPlayableWidget::DisplayGameOver()
{
}
