//

// Base:
#include "SettingsWidget.h"

// Win:
#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include <windows.h> // Conflict header file
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#endif

// UE:
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

// Interaction:
#include "FPS/Core/FPS_GameInstance.h"
#include "FPS/Tools/Saving/Settings/SaveSettings.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void USettingsWidget::NativeOnInitialized()
{
    InitWidgetData();

    Super::NativeOnInitialized();
}

void USettingsWidget::NativeConstruct()
{
    Super::NativeConstruct();
}
//--------------------------------------------------------------------------------------



/* ---   Settings System | Saving   --- */

const int32 USettingsWidget::GetDisplayFrequency() const
{
    int32 lFPS = 60;

    // Ограничение по платформе: Только WINDOWS
#if PLATFORM_WINDOWS
    DEVMODE dm;
    dm.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
    lFPS = dm.dmDisplayFrequency;
#endif

    return lFPS;
}

void USettingsWidget::LoadSettings()
{
    if (NewSettingsData != SaveSettings->SettingsData)
    {
        NewSettingsData = SaveSettings->SettingsData;

        UpdateSounds();
    }

    if (bIsVideoSettingsDirty
        || GameUserSettings->IsDirty())
    {
        GameUserSettings->LoadSettings();
        bIsVideoSettingsDirty = false;
    }
}

void USettingsWidget::ApplySettings()
{
    if (NewSettingsData != SaveSettings->SettingsData)
    {
        SaveSettings->SettingsData = NewSettingsData;
        UGameplayStatics::SaveGameToSlot(SaveSettings, SettingsDataSlot, 0);
    }

    if (bIsVideoSettingsDirty
        || GameUserSettings->IsDirty())
    {
        GameUserSettings->ValidateSettings();
        GameUserSettings->ApplyHardwareBenchmarkResults();
        GameUserSettings->ApplySettings(false);
        bIsVideoSettingsDirty = false;
    }
}

bool USettingsWidget::IsSettingsDirty()
{
    return bIsVideoSettingsDirty
        || NewSettingsData != SaveSettings->SettingsData
        || GEngine->GetGameUserSettings()->IsDirty();
}

void USettingsWidget::InitWidgetData()
{
    SaveSettings = Cast<USaveSettings>(UGameplayStatics::LoadGameFromSlot(SettingsDataSlot, 0));
    NewSettingsData = SaveSettings->SettingsData;

    GameUserSettings = GEngine->GetGameUserSettings();
}
//--------------------------------------------------------------------------------------



/* ---   Settings System | Sounds   --- */

void USettingsWidget::UpdateSounds()
{
    UFPS_GameInstance* lGameInstance = Cast<UFPS_GameInstance>(GetGameInstance());

    if (lGameInstance
        && lGameInstance->SoundMix)
    {
        UGameplayStatics::PushSoundMixModifier(GetWorld(), lGameInstance->SoundMix);

        if (lGameInstance->MusicSoundClass)
        {
            UGameplayStatics::SetSoundMixClassOverride(
                GetWorld(),
                lGameInstance->SoundMix,
                lGameInstance->MusicSoundClass,
                SaveSettings->SettingsData.OverallSoundsVolume * SaveSettings->SettingsData.MusicSoundsVolume,
                1.f,
                0.f);
        }

        if (lGameInstance->EffectsSoundClass)
        {
            UGameplayStatics::SetSoundMixClassOverride(
                GetWorld(),
                lGameInstance->SoundMix,
                lGameInstance->MusicSoundClass,
                SaveSettings->SettingsData.OverallSoundsVolume * SaveSettings->SettingsData.EffectSoundsVolume,
                1.f,
                0.f);
        }
    }
}
//--------------------------------------------------------------------------------------
