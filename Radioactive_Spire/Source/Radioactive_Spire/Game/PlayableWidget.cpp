// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Slider.h"

#include "Kismet/GameplayStatics.h"

#include "RadioactiveSpire_GameModeBase.h"
#include "RadioactiveSpire_GameStateBase.h"
#include "../MainMenu/Radioactive_Spire_GameInstance.h"

#include "../Players/Inheritance/PlayableCharacter.h"
#include "../Players/Inheritance/PlayableCharacterState.h"
#include "../Players/Inheritance/PlayableController.h"

#include "Input/ReplyBase.h"

#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"

#include "Sound/SoundClass.h"
#include "Engine/Engine.h"

bool UPlayableWidget::Initialize()
{
    GameInstance = Cast<URadioactive_Spire_GameInstance>(GetGameInstance());
    GameModeBase = GetWorld()->GetAuthGameMode<ARadioactiveSpire_GameModeBase>();
    Increment = 0;
    FadeTimer = 2.5f;
    GameOverDelay = 4.0f;

    State = EPauseMenuState::PauseMenu;

    SetIsFocusable(true);
    SetKeyboardFocus();

    //if (GameInstance && GameInstance->SelectSound != nullptr)
    //    UGameplayStatics::PrimeSound(GameInstance->SelectSound);
    //if (GameInstance && GameInstance->NavigationSound != nullptr)
    //    UGameplayStatics::PrimeSound(GameInstance->NavigationSound);
    //if (GameInstance && GameInstance->CancelSound != nullptr)
    //    UGameplayStatics::PrimeSound(GameInstance->CancelSound);

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
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseFadeToQuit);
        GameOverButtons.Add(Button);
    }
    //------------------------------------------------------------------------
    //options
    Image = Cast<UImage>(GetWidgetFromName("OptionsScreen"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Hidden);
    Image = Cast<UImage>(GetWidgetFromName("OptionsScreen1"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Hidden);

    USlider* Slider = Cast<USlider>(GetWidgetFromName("MusicSlider"));
    if (Slider)
    {
        Slider->SetVisibility(ESlateVisibility::Visible);
        Slider->OnValueChanged.AddDynamic(this, &UPlayableWidget::OnPauseMusicChange);
        if (GameInstance)
            Slider->SetValue(GameInstance->MusicValue);

        Slider->SetVisibility(ESlateVisibility::Hidden);

        PauseVolumeSliders.Add(Slider);
    }
    Slider = Cast<USlider>(GetWidgetFromName("SoundEffectsSlider"));
    if (Slider)
    {
        Slider->SetVisibility(ESlateVisibility::Visible);
        Slider->OnValueChanged.AddDynamic(this, &UPlayableWidget::OnPauseSFXChange);
        if (GameInstance)
            Slider->SetValue(GameInstance->SFXValue);

        Slider->SetVisibility(ESlateVisibility::Hidden);

        PauseVolumeSliders.Add(Slider);
    }
    Slider = Cast<USlider>(GetWidgetFromName("AmbienceSlider"));
    if (Slider)
    {
        Slider->SetVisibility(ESlateVisibility::Visible);
        Slider->OnValueChanged.AddDynamic(this, &UPlayableWidget::OnPauseAmbienceChange);
        if (GameInstance)
            Slider->SetValue(GameInstance->AmbienceValue);

        Slider->SetVisibility(ESlateVisibility::Hidden);

        PauseVolumeSliders.Add(Slider);
    }

    Button = Cast<UButton>(GetWidgetFromName("MusicButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseMusicPressed);
        PauseVolumeButtons.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("SoundEffectsButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseSFXPressed);
        PauseVolumeButtons.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("AmbienceButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Hidden);
        Button->OnClicked.AddDynamic(this, &UPlayableWidget::OnPauseAmbiencePressed);
        PauseVolumeButtons.Add(Button);
    }

    Widget = Cast<UTextBlock>(GetWidgetFromName("MusicValue"));
    if (Widget)
    {
        Widget->SetText(FText::FromString("Music"));
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }
    Widget = Cast<UTextBlock>(GetWidgetFromName("SFXValue"));
    if (Widget)
    {
        Widget->SetText(FText::FromString("SFX"));
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }
    Widget = Cast<UTextBlock>(GetWidgetFromName("AmbienceValue"));
    if (Widget)
    {
        Widget->SetText(FText::FromString("Ambience"));
        Widget->SetVisibility(ESlateVisibility::Hidden);
    }

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
        if (State == EPauseMenuState::PauseMenu)
            PauseMoveWidget(0.0f, 0.0f, InDeltaTime);
        else if (State == EPauseMenuState::OptionsMenu)
            PauseMoveWidget(1920.0f, 0.0f, InDeltaTime);

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
        //------------------------------------------------------------------------
        //options
        Image = Cast<UImage>(GetWidgetFromName("OptionsScreen"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Visible);
        Image = Cast<UImage>(GetWidgetFromName("OptionsScreen1"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Visible);
        for (int i = 0; i < PauseVolumeButtons.Num(); i++)
            PauseVolumeButtons[i]->SetVisibility(ESlateVisibility::Visible);
        for (int i = 0; i < PauseVolumeSliders.Num(); i++)
            PauseVolumeSliders[i]->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("MusicValue"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("SFXValue"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Visible);
        Widget = Cast<UTextBlock>(GetWidgetFromName("AmbienceValue"));
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
        if (State == EPauseMenuState::PauseMenu)
        {
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
        else if (State == EPauseMenuState::OptionsMenu)
        {
            PauseVolumeButtons[OptionsIncrement]->SetStyle(style);

            for (int i = 0; i < PauseVolumeButtons.Num(); i++)
            {
                if (PauseVolumeButtons[i]->IsHovered())
                    OptionsIncrement = i;
                if (i != OptionsIncrement)
                {
                    NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/UI/Game_Button.Game_Button")));
                    brush.SetResourceObject(NormalTexture);
                    brush.TintColor = FSlateColor(brush.TintColor = FSlateColor(FLinearColor(0.495466f, 0.495466f, 0.495466f, 1.0f)));
                    style.SetNormal(brush);
                    PauseVolumeButtons[i]->SetStyle(style);
                }
            }

            FString Value = FString::Printf(TEXT("%d"), (FMath::RoundToInt(GameInstance->MusicValue * 10.0f)));
            Widget = Cast<UTextBlock>(GetWidgetFromName("MusicValue"));
            if (Widget)
                Widget->SetText(FText::FromString(Value));
            Value = FString::Printf(TEXT("%d"), (FMath::RoundToInt(GameInstance->SFXValue * 10.0f)));
            Widget = Cast<UTextBlock>(GetWidgetFromName("SFXValue"));
            if (Widget)
                Widget->SetText(FText::FromString(Value));
            Value = FString::Printf(TEXT("%d"), (FMath::RoundToInt(GameInstance->AmbienceValue * 10.0f)));
            Widget = Cast<UTextBlock>(GetWidgetFromName("AmbienceValue"));
            if (Widget)
                Widget->SetText(FText::FromString(Value));

            UpdatePauseSoundChange();
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
        //------------------------------------------------------------------------
        //options
        Image = Cast<UImage>(GetWidgetFromName("OptionsScreen"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Hidden);
        Image = Cast<UImage>(GetWidgetFromName("OptionsScreen1"));
        if (Image)
            Image->SetVisibility(ESlateVisibility::Hidden);
        for (int i = 0; i < PauseVolumeButtons.Num(); i++)
            PauseVolumeButtons[i]->SetVisibility(ESlateVisibility::Hidden);
        for (int i = 0; i < PauseVolumeSliders.Num(); i++)
            PauseVolumeSliders[i]->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("MusicValue"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("SFXValue"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);
        Widget = Cast<UTextBlock>(GetWidgetFromName("AmbienceValue"));
        if (Widget)
            Widget->SetVisibility(ESlateVisibility::Hidden);

        Increment = 0;
        State = EPauseMenuState::PauseMenu;
        FWidgetTransform CurrentTransform = GetRenderTransform();

        CurrentTransform.Translation = FVector2D(0.0f, 0.0f);
        SetRenderTransform(CurrentTransform);
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
    if (State == EPauseMenuState::PauseMenu)
    {
        State = EPauseMenuState::OptionsMenu;
        ResetPauseIncrement();
    }
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
        if (GameInstance && GameInstance->NavigationSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, GameInstance->NavigationSound, FVector());

        if (State == EPauseMenuState::PauseMenu)
        {
            Increment += dir;
            if (Increment < 0)
                Increment = PauseButtons.Num() - 1;
            else if (Increment >= PauseButtons.Num())
                Increment = 0;
        }
        else if (State == EPauseMenuState::OptionsMenu)
        {
            OptionsIncrement += dir;
            if (OptionsIncrement < 0)
                OptionsIncrement = PauseVolumeButtons.Num() - 1;
            else if (OptionsIncrement >= PauseVolumeButtons.Num())
                OptionsIncrement = 0;
        }
    }
    else if (GameModeBase->State == EGameState::EndGame)
    {
        if (GameInstance && GameInstance->NavigationSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, GameInstance->NavigationSound, FVector());

        GameOverIncrement += dir;
        if (GameOverIncrement < 0)
            GameOverIncrement = GameOverButtons.Num() - 1;
        else if (GameOverIncrement >= GameOverButtons.Num())
            GameOverIncrement = 0;
    }
}

void UPlayableWidget::PauseMenuPressed()
{
    if (GameInstance && GameInstance->SelectSound != nullptr)
        UGameplayStatics::PlaySoundAtLocation(this, GameInstance->SelectSound, FVector());

    if (GameModeBase->Game_IsPaused)
    {
        if (State == EPauseMenuState::PauseMenu)
            PauseButtons[Increment]->OnClicked.Broadcast();
        else if (State == EPauseMenuState::OptionsMenu)
            PauseVolumeButtons[OptionsIncrement]->OnClicked.Broadcast();
    }
    else if (GameModeBase->State == EGameState::EndGame)
        GameOverButtons[GameOverIncrement]->OnClicked.Broadcast();
}

void UPlayableWidget::PauseMenuBackPressed()
{
    if (State == EPauseMenuState::OptionsMenu && GameModeBase->State != EGameState::EndGame)
    {
        if (GameInstance && GameInstance->CancelSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, GameInstance->CancelSound, FVector());

        State = EPauseMenuState::PauseMenu;
    }
    else if (State == EPauseMenuState::PauseMenu)
        GameModeBase->GamePause();
}

void UPlayableWidget::PauseMoveWidget(float posx, float posy, float DeltaTime, float speed)
{
    FWidgetTransform CurrentTransform = GetRenderTransform();

    float x = FMath::FInterpTo(posx, CurrentTransform.Translation.X, DeltaTime, speed);
    float y = FMath::FInterpTo(posy, CurrentTransform.Translation.Y, DeltaTime, speed);

    CurrentTransform.Translation = FVector2D(x, y);
    SetRenderTransform(CurrentTransform);
}

void UPlayableWidget::OnPauseMusicChange(float v)
{
    GameInstance->MusicValue = v;
}

void UPlayableWidget::OnPauseMusicPressed()
{
    if (GameInstance)
    {
        #define musVal GameInstance->MusicValue 

        musVal += 0.1;
        musVal = truncf(musVal * 1000.0f) / 1000.0f;
        if (musVal > 1.0)
            musVal = 0.0f;

        PauseVolumeSliders[0]->SetValue(musVal);
    }
}

void UPlayableWidget::OnPauseSFXChange(float v)
{
    GameInstance->SFXValue = v;
}

void UPlayableWidget::OnPauseSFXPressed()
{
    if (GameInstance)
    {
        #define sfxVal GameInstance->SFXValue 

        sfxVal += 0.1;
        sfxVal = truncf(sfxVal * 1000.0f) / 1000.0f;
        if (sfxVal > 1.0)
            sfxVal = 0.0f;

        PauseVolumeSliders[1]->SetValue(sfxVal);
    }
}

void UPlayableWidget::OnPauseAmbienceChange(float v)
{
    GameInstance->AmbienceValue = v;
}

void UPlayableWidget::OnPauseAmbiencePressed()
{
    if (GameInstance)
    {
        #define ambienceVal GameInstance->AmbienceValue 

        ambienceVal += 0.1;
        ambienceVal = truncf(ambienceVal * 1000.0f) / 1000.0f;
        if (ambienceVal > 1.0)
            ambienceVal = 0.0f;

        PauseVolumeSliders[2]->SetValue(ambienceVal);
    }
}

void UPlayableWidget::ResetPauseIncrement()
{
    Increment = 0;
    OptionsIncrement = 0;
}

void UPlayableWidget::UpdatePauseSoundChange()
{
    if (GameInstance->Music != nullptr && GameInstance != nullptr)
        GameInstance->Music->Properties.Volume = GameInstance->MusicValue;
    if (GameInstance->SFX != nullptr && GameInstance != nullptr)
        GameInstance->SFX->Properties.Volume = GameInstance->SFXValue;
    if (GameInstance->Ambience != nullptr && GameInstance != nullptr)
        GameInstance->Ambience->Properties.Volume = GameInstance->MusicValue;
}
