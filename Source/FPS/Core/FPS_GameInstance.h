//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

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
    USoundClass*  MusicSoundClass = nullptr;

    // Микшер громкости для Эффектов
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings System: Sounds")
    USoundClass*  EffectsSoundClass = nullptr;
    //-------------------------------------------



private:

    /* ---   Settings System | Saving   --- */

    /** Инициализация сохранения данных Настроек */
    void SettingsSavingInit();
    //-------------------------------------------
};
