//

// Base:
#include "FPS_GameInstance.h"

// UE:
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"

// Interaction:
#include "FPS/Tools/Saving/Settings/SaveSettings.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UFPS_GameInstance::Init()
{
    SettingsSavingInit();

    Super::Init();
}
//--------------------------------------------------------------------------------------



/* ---   Settings System | Saving   --- */

void UFPS_GameInstance::SettingsSavingInit()
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
                UE_LOG(LogTemp, Error, TEXT("'%s'::%s: MusicSoundClass is NOT"),
                    *GetNameSafe(this), *FString(__func__));
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
                UE_LOG(LogTemp, Error, TEXT("'%s'::%s: EffectsSoundClass is NOT"),
                    *GetNameSafe(this), *FString(__func__));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: SoundMix is NOT"),
                *GetNameSafe(this), *FString(__func__));
        }
        //-------------------------------------------
    }
}
//--------------------------------------------------------------------------------------
