// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"
#include "../Players/Inheritance/PlayableCharacter.h"

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
        Bar->SetFillColorAndOpacity(FLinearColor::Yellow);
    }


    return true;
}

void UPlayableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdateHealth();
    UpdateSuper();
    DisplayGameOver();
}

void UPlayableWidget::UpdateHealth()
{
    AController* pController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    APawn* playerPawn = pController->GetPawn();
    APlayableCharacter* Player = Cast<APlayableCharacter>(playerPawn);
    if (Player)
    {
        FString Value = FString::Printf(TEXT("%d / %d"), (int)Player->Health, (int)Player->MaxHealth);
        UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("HealthText"));
        if (Widget != nullptr)
            Widget->SetText(FText::FromString(Value));

        UProgressBar* Bar = Cast<UProgressBar>(GetWidgetFromName("HealthBar"));
        if (Bar != nullptr)
            Bar->SetPercent((Player->Health / Player->MaxHealth));
    }
}

void UPlayableWidget::UpdateSuper()
{
}

void UPlayableWidget::DisplayGameOver()
{
}
