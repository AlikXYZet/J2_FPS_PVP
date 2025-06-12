//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

// Interaction:
#include "SettingsData.h"

// Generated:
#include "SaveSettings.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API USaveSettings : public USaveGame
{
    GENERATED_BODY()

public:

    // Данные Настроек
    UPROPERTY(BlueprintReadOnly)
    FSettingsData SettingsData;
};
//--------------------------------------------------------------------------------------
