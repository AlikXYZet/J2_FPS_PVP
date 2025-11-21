//

// Base:
#include "FPS_GameInstance.h"

// UE:
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

// Interaction:
#include "FPS/Tools/Saving/Settings/SaveSettings.h"
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Общедоступный указатель на текущий экземпляр 'UFPS_GameInstance'
UFPS_GameInstance* UFPS_GameInstance::CurrentGameInstance = nullptr;
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UFPS_GameInstance::Init()
{
    InitSettingsSaving();

    Super::Init();

    IsValidStaticPointer();
}
//--------------------------------------------------------------------------------------



/* ---   Settings System | Saving   --- */

void UFPS_GameInstance::InitSettingsSaving()
{
    USaveSettings* SaveSettings = Cast<USaveSettings>(UGameplayStatics::LoadGameFromSlot("SettingsData", 0));

    if (!SaveSettings)
    {
        UGameUserSettings* lGameUserSettings = GEngine->GetGameUserSettings();

        /* ---   Auto settings:   --- */
        // Автоматическое определение, принятие и сохранение всех Настроек Видео:
        lGameUserSettings->RunHardwareBenchmark();          // Определение
        lGameUserSettings->ApplyHardwareBenchmarkResults(); // Принятие и Сохранение
        //-------------------------------------------


        /* ---   Default Video settings:   --- */

        // Полный экран
        lGameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);

        // Разрешение экрана
        TArray<FIntPoint> lAllRes; // Все доступные вариации Разрешения экрана (от меньшего к большему)
        UKismetSystemLibrary::GetSupportedFullscreenResolutions(lAllRes);
        // Принятие последнего (наибольшего) значения Разрешения экрана
        lGameUserSettings->SetScreenResolution(lAllRes[lAllRes.Num() - 1]);

        // FPS
        lGameUserSettings->SetFrameRateLimit(60);


        lGameUserSettings->ApplySettings(false);
        //-------------------------------------------


        /* ---   Settings System:   --- */

        // Создание и сохранение базовых Настроек: Игровой процесс, Звук
        SaveSettings = Cast<USaveSettings>(UGameplayStatics::CreateSaveGameObject(
            USaveSettings::StaticClass()));

        UGameplayStatics::SaveGameToSlot(SaveSettings, "SettingsData", 0);
        //-------------------------------------------


        /* ---   Default Sounds settings:   --- */

        if (SoundMix)
        {
            UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMix);

            if (MusicSoundClass)
            {
                UGameplayStatics::SetSoundMixClassOverride(
                    GetWorld(),
                    SoundMix,
                    MusicSoundClass,
                    SaveSettings->SettingsData.OverallSoundsVolume * SaveSettings->SettingsData.MusicSoundsVolume,
                    1.f,
                    0.f);
            }
            else
            {
                FPS_LOG(Error, TEXT("MusicSoundClass is NOT"));
            }

            if (EffectsSoundClass)
            {
                UGameplayStatics::SetSoundMixClassOverride(
                    GetWorld(),
                    SoundMix,
                    MusicSoundClass,
                    SaveSettings->SettingsData.OverallSoundsVolume * SaveSettings->SettingsData.EffectSoundsVolume,
                    1.f,
                    0.f);
            }
            else
            {
                FPS_LOG(Error, TEXT("EffectsSoundClass is NOT"));
            }
        }
        else
        {
            FPS_LOG(Error, TEXT("SoundMix is NOT"));
        }
        //-------------------------------------------
    }
}
//--------------------------------------------------------------------------------------
