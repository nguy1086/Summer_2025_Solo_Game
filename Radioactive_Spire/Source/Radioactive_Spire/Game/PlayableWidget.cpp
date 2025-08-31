// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Border.h"

#include "Kismet/GameplayStatics.h"

#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"

#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"
#include "../Players/Inheritance/PlayableController.h"

#include "Input/ReplyBase.h"

#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"

bool UPlayableWidget::Initialize()
{
    GameModeBase = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
    Increment = 0;
    FadeTimer = 2.5f;
    GameOverDelay = 4.0f;
    SetIsFocusable(true);
    SetKeyboardFocus();

    if (SelectSound != nullptr)
        UGameplayStatics::PrimeSound(SelectSound);
    if (NavigationSound != nullptr)
        UGameplayStatics::PrimeSound(NavigationSound);
    if (CancelSound != nullptr)
        UGameplayStatics::PrimeSound(CancelSound);

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
    //--------------------------------------------------------------
    Image = Cast<UImage>(GetWidgetFromName("PauseScreen"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Hidden);

    UButton* Button = Cast<UButton>(GetWidgetFromName("Resume"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnResume);
        PauseButtons.Add(Button);
    }

    Button = Cast<UButton>(GetWidgetFromName("Options"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseOptions);
        PauseButtons.Add(Button);
    }

    Button = Cast<UButton>(GetWidgetFromName("Retry"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseFadeToRetry);
        PauseButtons.Add(Button);
    }

    Button = Cast<UButton>(GetWidgetFromName("Quit"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseFadeToQuit);
        PauseButtons.Add(Button);
    }

    Widget = Cast<UTextBlock>(GetWidgetFromName("PausedText"));
    if (Widget)
    {
        Widget->SetText(FText::FromString("Paused"));
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }
    //------------------------------------------------------------------------
    UBorder* Border = Cast<UBorder>(GetWidgetFromName("Fade"));
    if (Border)
        Border->SetVisibility(ESlateVisibility::Visible);
    //------------------------------------------------------------------------
    //gameover
    Widget = Cast<UTextBlock>(GetWidgetFromName("GameOverText"));
    if (Widget)
    {
        Widget->SetText(FText::FromString("Game Over"));
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }
    Image = Cast<UImage>(GetWidgetFromName("GameOver"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Hidden);
    Button = Cast<UButton>(GetWidgetFromName("GameOverRetry"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseFadeToRetry);
        GameOverButtons.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("GameOverQuit"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseFadeToRetry);
        GameOverButtons.Add(Button);
    }
    //------------------------------------------------------------------------
    //options
    Image = Cast<UImage>(GetWidgetFromName("OptionsScreen"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Hidden);

    return true;
}

void UPlayableWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (GameModeBase->State == EGameState::FadeToEnter)
    {
        FadeEnter(InDeltaTime);
        UpdateHealth();
        UpdateSuper();
        UpdateEnemies();
    }
    else if (GameModeBase->State == EGameState::FadeToQuit)
    {
        FadeQuit(InDeltaTime);
    }
    else if (GameModeBase->State == EGameState::FadeToRetry)
    {
        FadeRetry(InDeltaTime);
    }
    else if (GameModeBase->State != EGameState::EndGame)
    {
        UpdateHealth();
        UpdateSuper();
        UpdatePause();
        UpdateEnemies();
    }
    else if (GameModeBase->State == EGameState::EndGame)
    {
        if (GameOverDelay <= 0.0f)
            DisplayGameOver();
        else
        {
            GameOverDelay -= InDeltaTime;
            HideAllButGameOver();
        }
    }
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
            if ((int)Player->Stats_Super / (int)Player->Stats_MaxSuper == 1)
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

void UPlayableWidget::UpdateEnemies()
{
    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("Enemy"), ActorsWithTag);
    FString Value = FString::Printf(TEXT("%d"), (int)ActorsWithTag.Num());

    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("EnemiesLeftNumber"));
    if (Widget)
        Widget->SetText(FText::FromString(Value));
}

void UPlayableWidget::UpdatePause()
{
    if (GameModeBase->Game_IsPaused)
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

        UButton* Button = Cast<UButton>(GetWidgetFromName("Resume"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Visible);

        Button = Cast<UButton>(GetWidgetFromName("Options"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Visible);

        Button = Cast<UButton>(GetWidgetFromName("Retry"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Visible);

        Button = Cast<UButton>(GetWidgetFromName("Quit"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("PausedText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);

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
        PauseButtons[Increment]->SetStyle(style);

        for (int i = 0; i < PauseButtons.Num(); i++)
        {
            if (PauseButtons[i]->IsHovered())
                Increment = i;
            if (i != Increment)
            {
                NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/UI/Game_Button.Game_Button")));
                brush.SetResourceObject(NormalTexture);
                brush.TintColor = FSlateColor(brush.TintColor = FSlateColor(FLinearColor(0.495466f, 0.495466f, 0.495466f, 1.0f)));
                style.SetNormal(brush);
                PauseButtons[i]->SetStyle(style);
            }
        }
    }
    else if (!GameModeBase->Game_IsPaused)
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

        UButton* Button = Cast<UButton>(GetWidgetFromName("Resume"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Hidden);

        Button = Cast<UButton>(GetWidgetFromName("Options"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Hidden);

        Button = Cast<UButton>(GetWidgetFromName("Retry"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Hidden);

        Button = Cast<UButton>(GetWidgetFromName("Quit"));
        if (Button)
            Button->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("PausedText"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);

        Increment = 0;
    }
}

void UPlayableWidget::FadeEnter(float DeltaTime)
{
    FadeTimer -= DeltaTime;
    UBorder* Border = Cast<UBorder>(GetWidgetFromName("Fade"));
    if (Border)
        Border->SetRenderOpacity(FadeTimer / 2.0f);
}

void UPlayableWidget::FadeQuit(float DeltaTime)
{
    FadeTimer += DeltaTime;
    UBorder* Border = Cast<UBorder>(GetWidgetFromName("Fade"));
    if (Border)
        Border->SetRenderOpacity(FadeTimer / 1.5f);
}

void UPlayableWidget::FadeRetry(float DeltaTime)
{
    FadeTimer += DeltaTime;
    UBorder* Border = Cast<UBorder>(GetWidgetFromName("Fade"));
    if (Border)
        Border->SetRenderOpacity(FadeTimer / 1.5f);
}

void UPlayableWidget::HideAllButGameOver()
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
    //--------------------------------------------------------------
    Image = Cast<UImage>(GetWidgetFromName("PauseScreen"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Hidden);
    UButton* Button = Cast<UButton>(GetWidgetFromName("Resume"));
    if (Button)
        Button->SetVisibility(ESlateVisibility::Hidden);
    Button = Cast<UButton>(GetWidgetFromName("Options"));
    if (Button)
        Button->SetVisibility(ESlateVisibility::Hidden);
    Button = Cast<UButton>(GetWidgetFromName("Retry"));
    if (Button)
        Button->SetVisibility(ESlateVisibility::Hidden);
    Button = Cast<UButton>(GetWidgetFromName("Quit"));
    if (Button)
        Button->SetVisibility(ESlateVisibility::Hidden);
    Widget = Cast<UTextBlock>(GetWidgetFromName("PausedText"));
    if (Widget)
        Widget->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayableWidget::DisplayGameOver()
{
    UImage* Image = Cast<UImage>(GetWidgetFromName("GameOver"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("GameOverText"));
    if (Widget)
        Widget->SetVisibility(ESlateVisibility::Visible);
    UButton* Button = Cast<UButton>(GetWidgetFromName("GameOverRetry"));
    if (Button)
        Button->SetVisibility(ESlateVisibility::Visible);
    Button = Cast<UButton>(GetWidgetFromName("GameOverQuit"));
    if (Button)
        Button->SetVisibility(ESlateVisibility::Visible);

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
    GameOverButtons[GameOverIncrement]->SetStyle(style);

    for (int i = 0; i < GameOverButtons.Num(); i++)
    {
        if (GameOverButtons[i]->IsHovered())
            GameOverIncrement = i;
        if (i != GameOverIncrement)
        {
            NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/UI/Game_Button.Game_Button")));
            brush.SetResourceObject(NormalTexture);
            brush.TintColor = FSlateColor(brush.TintColor = FSlateColor(FLinearColor(0.495466f, 0.495466f, 0.495466f, 1.0f)));
            style.SetNormal(brush);
            GameOverButtons[i]->SetStyle(style);
        }
    }
}

void UPlayableWidget::OnResume()
{
    GameModeBase->GamePause();
}

void UPlayableWidget::OnPauseOptions()
{
}

void UPlayableWidget::OnRetry()
{
    UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void UPlayableWidget::OnPauseQuit()
{
    UObject* WorldContextObject = this;
    UGameplayStatics::OpenLevel(WorldContextObject, FName(TEXT("MainMenu")));
}

void UPlayableWidget::OnPauseFadeToQuit()
{
    GameModeBase->State = EGameState::FadeToQuit;
}

void UPlayableWidget::OnPauseFadeToRetry()
{
    GameModeBase->State = EGameState::FadeToRetry;
}

void UPlayableWidget::PauseMenuNavigation(float dir)//tried FReply UPlayableWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{                                                       //didnt work
    if (GameModeBase->Game_IsPaused)
    {
        if (NavigationSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, NavigationSound, FVector());

        Increment += dir;
        if (Increment < 0)
            Increment = PauseButtons.Num() - 1;
        else if (Increment >= PauseButtons.Num())
            Increment = 0;
    }
    else if (GameModeBase->State == EGameState::EndGame)
    {
        if (NavigationSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, NavigationSound, FVector());

        GameOverIncrement += dir;
        if (GameOverIncrement < 0)
            GameOverIncrement = GameOverButtons.Num() - 1;
        else if (GameOverIncrement >= GameOverButtons.Num())
            GameOverIncrement = 0;
    }
}

void UPlayableWidget::PauseMenuPressed()
{
    if (SelectSound != nullptr)
        UGameplayStatics::PlaySoundAtLocation(this, SelectSound, FVector());

    if (GameModeBase->Game_IsPaused)
        PauseButtons[Increment]->OnClicked.Broadcast();
    else if (GameModeBase->State == EGameState::EndGame)
        GameOverButtons[GameOverIncrement]->OnClicked.Broadcast();
}
