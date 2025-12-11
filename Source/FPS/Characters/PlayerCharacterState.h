//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/PlayerState.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// Generated:
#include "PlayerCharacterState.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APlayerCharacterState : public APlayerState
{
    GENERATED_BODY()

public:

    /* ---   Net   --- */

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Установить Текущий сетевой статус Игрока */
    FORCEINLINE void SetCurrentNetStatus(ENetMode Value)
    {
        CurrentNetStatus = Value;
    }

    /** Получить Текущий сетевой статус Игрока */
    FORCEINLINE ENetMode GetCurrentNetStatus() const
    {
        return ENetMode(CurrentNetStatus);
    };
    //-------------------------------------------



private:

    /* ---   Net   --- */

    // Текущий сетевой статус Игрока
    UPROPERTY(Replicated)
    uint8 CurrentNetStatus = ENetMode::NM_Client;
    //-------------------------------------------
};
