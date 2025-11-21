//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Engine/GameInstance.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Generated:
#include "FPS_GameInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class USaveSettings;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UFPS_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:

    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'UFPS_GameInstance'
    // @note    Используется для уменьшения зависимостей и использования излишних функций
    //          Например, функций 'Cast<>' и методов Инициализации в других классах
    static UFPS_GameInstance* CurrentGameInstance;

    //

    /** Метод проверки валидности статического указателя 'Current Game Instance' */
    FORCEINLINE static bool IsValidStaticPointer()
    {
        if (!IsValid(CurrentGameInstance))
        {
            FPS_LOG_Empty(Error,
                "Current GameInstance is NOT 'UFPS_GameInstance' class. "
                "See 'Project Settings'/'Maps & Modes'/'Game Instance Class'");

            return false;
        }
        return true;
    };
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UFPS_GameInstance()
    {
        CurrentGameInstance = this;
    };
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Инициализация данных GameInstance */
    virtual void Init() override;
    //-------------------------------------------



    /* ---   Settings System | Sounds   --- */

    // Микшер громкости
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings System: Sounds")
    USoundMix* SoundMix = nullptr;

    // Микшер громкости для Музыки
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings System: Sounds")
    USoundClass* MusicSoundClass = nullptr;

    // Микшер громкости для Эффектов
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings System: Sounds")
    USoundClass* EffectsSoundClass = nullptr;
    //-------------------------------------------



private:

    /* ---   Settings System | Saving   --- */

    /** Инициализация сохранения данных Настроек */
    void InitSettingsSaving();
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

/** Получить текущий экземпляр класса 'UFPS_GameInstance' */
FORCEINLINE static UFPS_GameInstance* const GetFPSGameInstance()
{
    return UFPS_GameInstance::CurrentGameInstance;
};
//--------------------------------------------------------------------------------------
