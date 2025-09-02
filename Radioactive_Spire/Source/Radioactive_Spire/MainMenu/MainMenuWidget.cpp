// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "MainMenu_GameModeBase.h"
#include "Radioactive_Spire_GameInstance.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"

#include "Kismet/GameplayStatics.h"
#include "MainMenuController.h"
#include "Math/UnrealMathUtility.h"

#include "Sound/SoundClass.h"
#include "Engine/Engine.h"

bool UMainMenuWidget::Initialize()
{
    GameInstance = Cast<URadioactive_Spire_GameInstance>(GetGameInstance());
    FadeTimer = 4.0f;
    CharacterScreenAnimateTimer = 0.2f;
    CharacterScreenAnimateIncrement = 0;
    GameModeBase = GetWorld()->GetAuthGameMode<AMainMenu_GameModeBase>();
    State = EMainMenuState::Intro;
    FWidgetTransform CurrentTransform = GetRenderTransform();
    CurrentTransform.Translation = FVector2D(0.0f, 1080.0f);
    SetRenderTransform(CurrentTransform);

    if (SelectSound != nullptr)
        UGameplayStatics::PrimeSound(SelectSound);
    if (NavigationSound != nullptr)
        UGameplayStatics::PrimeSound(NavigationSound);
    if (CancelSound != nullptr)
        UGameplayStatics::PrimeSound(CancelSound);

    ResetIncrement();

    SetIsFocusable(true);
    SetKeyboardFocus();

    bool bResult = Super::Initialize();
    if (!bResult)
        return false;
    //-------------------------------------------------------------------------------------------------
    //main/intro
    UImage* Image = Cast<UImage>(GetWidgetFromName("Intro"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    Image = Cast<UImage>(GetWidgetFromName("MainMenu"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    Image = Cast<UImage>(GetWidgetFromName("Options"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);
    Image = Cast<UImage>(GetWidgetFromName("CharacterSelect1"));
    if (Image)
    {
        Image->SetVisibility(ESlateVisibility::Visible);
        CharacterScreenAnimate.Add(Image);
    }
    Image = Cast<UImage>(GetWidgetFromName("CharacterSelect2"));
    if (Image)
    {
        Image->SetVisibility(ESlateVisibility::Hidden);
        CharacterScreenAnimate.Add(Image);
    }
    Image = Cast<UImage>(GetWidgetFromName("CharacterSelect3"));
    if (Image)
    {
        Image->SetVisibility(ESlateVisibility::Hidden);
        CharacterScreenAnimate.Add(Image);
    }
    Image = Cast<UImage>(GetWidgetFromName("GameName"));
    if (Image)
        Image->SetVisibility(ESlateVisibility::Visible);

    UBorder* Border = Cast<UBorder>(GetWidgetFromName("Fade"));
    if (Border)
        Border->SetVisibility(ESlateVisibility::Visible);

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
    //-------------------------------------------------------------------------------------------------
    //options
    USlider* Slider = Cast<USlider>(GetWidgetFromName("MusicSlider"));
    if (Slider)
    {
        Slider->SetVisibility(ESlateVisibility::Visible);
        Slider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnMusicChange);
        if (GameInstance)
            Slider->SetValue(GameInstance->MusicValue);

        VolumeSliders.Add(Slider);
    }
    Slider = Cast<USlider>(GetWidgetFromName("SoundEffectsSlider"));
    if (Slider)
    {
        Slider->SetVisibility(ESlateVisibility::Visible);
        Slider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnSFXChange);
        if (GameInstance)
            Slider->SetValue(GameInstance->SFXValue);

        VolumeSliders.Add(Slider);
    }
    Slider = Cast<USlider>(GetWidgetFromName("AmbienceSlider"));
    if (Slider)
    {
        Slider->SetVisibility(ESlateVisibility::Visible);
        Slider->OnValueChanged.AddDynamic(this, &UMainMenuWidget::OnAmbienceChange);
        if (GameInstance)
            Slider->SetValue(GameInstance->AmbienceValue);

        VolumeSliders.Add(Slider);
    }

    Button = Cast<UButton>(GetWidgetFromName("MusicButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnMusicPressed);
        VolumeButtons.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("SoundEffectsButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSFXPressed);
        VolumeButtons.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("AmbienceButton"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnAmbiencePressed);
        VolumeButtons.Add(Button);
    }

    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("MusicValue"));
    if (Widget)
        Widget->SetText(FText::FromString("Music"));
    Widget = Cast<UTextBlock>(GetWidgetFromName("SFXValue"));
    if (Widget)
        Widget->SetText(FText::FromString("SFX"));
    Widget = Cast<UTextBlock>(GetWidgetFromName("AmbienceValue"));
    if (Widget)
        Widget->SetText(FText::FromString("Ambience"));

    //-------------------------------------------------------------------------------------------------
    //character
    FSlateBrush brush;
    UTexture2D* NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/MainMenu/MainMenu_CharacterSelectBatterIcon.MainMenu_CharacterSelectBatterIcon")));
    brush.SetResourceObject(NormalTexture);
    FButtonStyle style;
    style.SetNormal(brush);
    style.SetHovered(brush);
    style.SetPressed(brush);
    Button = Cast<UButton>(GetWidgetFromName("Batter_Select"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->OnClicked.AddDynamic(this, &UMainMenuWidget::OnBatterSelect);
        Button->SetStyle(style);
        CharacterSelections.Add(Button);
    }
    NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/MainMenu/MainMenu_CharacterSelectLockedIcon.MainMenu_CharacterSelectLockedIcon")));
    brush.SetResourceObject(NormalTexture);
    style.SetNormal(brush);
    style.SetHovered(brush);
    style.SetPressed(brush);
    Button = Cast<UButton>(GetWidgetFromName("Locked1"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->SetStyle(style);
        CharacterSelections.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("Locked2"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->SetStyle(style);
        CharacterSelections.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("Locked3"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->SetStyle(style);
        CharacterSelections.Add(Button);
    }
    Button = Cast<UButton>(GetWidgetFromName("Locked4"));
    if (Button)
    {
        Button->SetVisibility(ESlateVisibility::Visible);
        Button->SetStyle(style);
        CharacterSelections.Add(Button);
    }
    Border = Cast<UBorder>(GetWidgetFromName("CharacterFade"));
    if (Border)
    {
        Border->SetVisibility(ESlateVisibility::Visible);
        Border->SetRenderOpacity(0.0f);
    }

    Widget = Cast<UTextBlock>(GetWidgetFromName("NameText"));
    if (Widget)
        Widget->SetText(FText::FromString("Name"));

    return true;
}

void UMainMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (State == EMainMenuState::MainMenu)
    {
        MainMenuMoveWidget(0.0f, 0.0f, InDeltaTime);

        UpdateMainMenu();
    }
    else if (State == EMainMenuState::Options)
    {
        MainMenuMoveWidget(1920.0f, 0.0f, InDeltaTime);

        UpdateOptions();
    }
    else if (State == EMainMenuState::Character)
    {
        MainMenuMoveWidget(0.0f, -1080.0f, InDeltaTime);

        UpdateCharacterSelect(InDeltaTime);
    }
    else if (State == EMainMenuState::Intro)
    {
        UpdateIntro(InDeltaTime);
    }
    else if (State == EMainMenuState::Loading)
    {
        FadeTimer += InDeltaTime;
        UBorder* Border = Cast<UBorder>(GetWidgetFromName("CharacterFade"));
        if (Border)
            Border->SetRenderOpacity(FadeTimer / 1.5f);

        if (FadeTimer >= 3.5f)
            CharacterSelections[CharacterIncrement]->OnClicked.Broadcast();
    }
}

void UMainMenuWidget::UpdateIntro(float DeltaTime)
{
    FadeTimer -= DeltaTime;
    UBorder* Border = Cast<UBorder>(GetWidgetFromName("Fade"));
    if (Border)
        Border->SetRenderOpacity((FadeTimer - 2.0f) / 2.0f);


    if (FadeTimer <= 0.0f)
    {
        MainMenuMoveWidget(0.0, 0.0f, DeltaTime, 56.0f);
        FWidgetTransform CurrentTransform = GetRenderTransform();
        if (FMath::IsNearlyEqual(CurrentTransform.Translation.Y, 0.0f, 6.0f))
        {
            State = EMainMenuState::MainMenu;
            CurrentTransform.Translation = FVector2D(0.0f, 0.0f);
            SetRenderTransform(CurrentTransform);
            FadeTimer = 0.0f;
        }
    }
}

void UMainMenuWidget::UpdateMainMenu()
{
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

void UMainMenuWidget::UpdateOptions()
{
    FString Value = FString::Printf(TEXT("%d"), (FMath::RoundToInt(GameInstance->MusicValue * 10.0f)));
    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("MusicValue"));
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
    VolumeButtons[OptionIncrement]->SetStyle(style);

    for (int i = 0; i < VolumeButtons.Num(); i++)
    {
        if (VolumeButtons[i]->IsHovered())
            OptionIncrement = i;
        if (i != OptionIncrement)
        {
            NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, TEXT("/Game/Game/UI/Game_Button.Game_Button")));
            brush.SetResourceObject(NormalTexture);
            brush.TintColor = FSlateColor(brush.TintColor = FSlateColor(FLinearColor(0.495466f, 0.495466f, 0.495466f, 1.0f)));
            style.SetNormal(brush);
            VolumeButtons[i]->SetStyle(style);
        }
    }

    UpdateSoundChange();
}

void UMainMenuWidget::UpdateCharacterSelect(float DeltaTime, float speed)
{
    //100% know there's a for loop for this but too dumb to do that

    int i = CharacterIncrement;
    FWidgetTransform CurrentTransform = CharacterSelections[i]->GetRenderTransform();
    float x = FMath::FInterpTo(0.0f, CurrentTransform.Translation.X, DeltaTime, speed);
    float y = FMath::FInterpTo(200.0f, CurrentTransform.Translation.Y, DeltaTime, speed);
    CurrentTransform.Translation = FVector2D(x, y);
    CharacterSelections[i]->SetRenderTransform(CurrentTransform);


    UTextBlock* Widget = Cast<UTextBlock>(GetWidgetFromName("NameText"));
    if (Widget)
    {
        if (CharacterSelections[i]->GetFName() == TEXT("Batter_Select"))
            Widget->SetText(FText::FromString("Batter"));
        else
            Widget->SetText(FText::FromString("Locked"));
    }
    i + 1 > CharacterSelections.Num() - 1 ? i = 0 : i++;

    CurrentTransform = CharacterSelections[i]->GetRenderTransform();
    x = FMath::FInterpTo(200.0f, CurrentTransform.Translation.X, DeltaTime, speed);
    y = FMath::FInterpTo(50.0f, CurrentTransform.Translation.Y, DeltaTime, speed);
    CurrentTransform.Translation = FVector2D(x, y);
    CharacterSelections[i]->SetRenderTransform(CurrentTransform);
    i + 1 > CharacterSelections.Num() - 1 ? i = 0 : i++;

    CurrentTransform = CharacterSelections[i]->GetRenderTransform();
    x = FMath::FInterpTo(100.0f, CurrentTransform.Translation.X, DeltaTime, speed);
    y = FMath::FInterpTo(-200.0f, CurrentTransform.Translation.Y, DeltaTime, speed);
    CurrentTransform.Translation = FVector2D(x, y);
    CharacterSelections[i]->SetRenderTransform(CurrentTransform);
    i + 1 > CharacterSelections.Num() - 1 ? i = 0 : i++;

    CurrentTransform = CharacterSelections[i]->GetRenderTransform();
    x = FMath::FInterpTo(-100.0f, CurrentTransform.Translation.X, DeltaTime, speed);
    y = FMath::FInterpTo(-200.0f, CurrentTransform.Translation.Y, DeltaTime, speed);
    CurrentTransform.Translation = FVector2D(x, y);
    CharacterSelections[i]->SetRenderTransform(CurrentTransform);
    i + 1 > CharacterSelections.Num() - 1 ? i = 0 : i++;

    CurrentTransform = CharacterSelections[i]->GetRenderTransform();
    x = FMath::FInterpTo(-200.0f, CurrentTransform.Translation.X, DeltaTime, speed);
    y = FMath::FInterpTo(50.0f, CurrentTransform.Translation.Y, DeltaTime, speed);
    CurrentTransform.Translation = FVector2D(x, y);
    CharacterSelections[i]->SetRenderTransform(CurrentTransform);

    CharacterScreenAnimateTimer -= DeltaTime;
    if (CharacterScreenAnimateTimer <= 0)
    {
        CharacterScreenAnimate[CharacterScreenAnimateIncrement]->SetVisibility(ESlateVisibility::Visible);
        for (int j = 0; j < CharacterScreenAnimate.Num(); j++)
            if (j != CharacterScreenAnimateIncrement)
                CharacterScreenAnimate[j]->SetVisibility(ESlateVisibility::Hidden);

        CharacterScreenAnimateTimer = 0.2f;
        CharacterScreenAnimateIncrement++;
        if (CharacterScreenAnimateIncrement >= CharacterScreenAnimate.Num())
            CharacterScreenAnimateIncrement = 0;
    }
}

void UMainMenuWidget::OnCharacterSelect()
{
    if (State == EMainMenuState::MainMenu || State == EMainMenuState::Options)
    {
        State = EMainMenuState::Character;
        ResetIncrement();
    }
}

void UMainMenuWidget::OnMainMenuOptions()
{
    if (State == EMainMenuState::MainMenu || State == EMainMenuState::Character)
    {
        State = EMainMenuState::Options;
        ResetIncrement();
    }
}

void UMainMenuWidget::OnMainMenuQuit()
{
    AMainMenuController* PlayableController = Cast<AMainMenuController>(GameModeBase->GetInstigatorController());
    UKismetSystemLibrary::QuitGame(GetWorld(), PlayableController, EQuitPreference::Quit, true);
}

void UMainMenuWidget::MainMenuNavigation(float dir)
{
    if (State != EMainMenuState::Loading && State != EMainMenuState::Intro)
    {
        if (NavigationSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, NavigationSound, FVector());

        if (State == EMainMenuState::MainMenu)
        {
            Increment += dir;
            if (Increment < 0)
                Increment = MainMenuButtons.Num() - 1;
            else if (Increment >= MainMenuButtons.Num())
                Increment = 0;
        }
        else if (State == EMainMenuState::Options)
        {
            OptionIncrement += dir;
            if (OptionIncrement < 0)
                OptionIncrement = VolumeButtons.Num() - 1;
            else if (OptionIncrement >= VolumeButtons.Num())
                OptionIncrement = 0;
        }
        else if (State == EMainMenuState::Character)
        {
            CharacterIncrement += dir;
            if (CharacterIncrement < 0)
                CharacterIncrement = CharacterSelections.Num() - 1;
            else if (CharacterIncrement >= CharacterSelections.Num())
                CharacterIncrement = 0;
        }
    }
}

void UMainMenuWidget::MainMenuPressed()
{
    if (State != EMainMenuState::Loading && State != EMainMenuState::Intro)
    {
        if (SelectSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, SelectSound, FVector());

        if (State == EMainMenuState::MainMenu)
            MainMenuButtons[Increment]->OnClicked.Broadcast();
        else if (State == EMainMenuState::Character)
        {
            if (CharacterSelections[CharacterIncrement]->OnClicked.IsBound())
                State = EMainMenuState::Loading;
        }
        else if (State == EMainMenuState::Options)
            VolumeButtons[OptionIncrement]->OnClicked.Broadcast();
    }
}

void UMainMenuWidget::MainMenuBackPressed()
{
    if ((State == EMainMenuState::Character || 
        State == EMainMenuState::Options) && 
        State != EMainMenuState::Loading || 
        State != EMainMenuState::Intro || 
        State != EMainMenuState::MainMenu)
    {
        if (CancelSound != nullptr)
            UGameplayStatics::PlaySoundAtLocation(this, CancelSound, FVector());

        State = EMainMenuState::MainMenu;
    }
}

void UMainMenuWidget::MainMenuMoveWidget(float posx, float posy, float DeltaTime, float speed)
{
    FWidgetTransform CurrentTransform = GetRenderTransform();

    float x = FMath::FInterpTo(posx, CurrentTransform.Translation.X, DeltaTime, speed);
    float y = FMath::FInterpTo(posy, CurrentTransform.Translation.Y, DeltaTime, speed);

    CurrentTransform.Translation = FVector2D(x, y);
    SetRenderTransform(CurrentTransform);
}

void UMainMenuWidget::OnMusicChange(float v)
{
    GameInstance->MusicValue = v;
}

void UMainMenuWidget::OnMusicPressed()
{
    if (GameInstance)
    {
        #define musVal GameInstance->MusicValue 

        musVal += 0.1;
        musVal = truncf(musVal * 1000.0f) / 1000.0f;
        if (musVal > 1.0)
            musVal = 0.0f;

        VolumeSliders[0]->SetValue(musVal);
    }
}

void UMainMenuWidget::OnSFXChange(float v)
{
    GameInstance->SFXValue = v;
}

void UMainMenuWidget::OnSFXPressed()
{
    if (GameInstance)
    {
        #define sfxVal GameInstance->SFXValue 

        sfxVal += 0.1;
        sfxVal = truncf(sfxVal * 1000.0f) / 1000.0f;
        if (sfxVal > 1.0)
            sfxVal = 0.0f;

        VolumeSliders[1]->SetValue(sfxVal);
    }
}

void UMainMenuWidget::OnAmbienceChange(float v)
{
    GameInstance->AmbienceValue = v;
}

void UMainMenuWidget::OnAmbiencePressed()
{
    if (GameInstance)
    {
        #define ambienceVal GameInstance->AmbienceValue 

        ambienceVal += 0.1;
        ambienceVal = truncf(ambienceVal * 1000.0f) / 1000.0f;
        if (ambienceVal > 1.0)
            ambienceVal = 0.0f;

        VolumeSliders[2]->SetValue(ambienceVal);
    }
}

void UMainMenuWidget::OnBatterSelect()
{
    UObject* WorldContextObject = this;
    UGameplayStatics::OpenLevel(WorldContextObject, FName(TEXT("Game")));
}

void UMainMenuWidget::ResetIncrement()
{
    Increment = 0;
    OptionIncrement = 0;
    CharacterIncrement = 0;
}

void UMainMenuWidget::UpdateSoundChange()
{
    if (SelectSound != nullptr && GameInstance != nullptr)
        SelectSound->GetSoundClass()->Properties.Volume = GameInstance->SFXValue;
    if (NavigationSound != nullptr && GameInstance != nullptr)
        NavigationSound->GetSoundClass()->Properties.Volume = GameInstance->SFXValue;
    if (CancelSound != nullptr && GameInstance != nullptr)
        CancelSound->GetSoundClass()->Properties.Volume = GameInstance->SFXValue;
}
