//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/GameState.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Structs:
#include "FPS/Tools/Structs/Statistics/PlayerStatisticsData.h"

// Generated:
#include "FPS_GameState.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
//class ;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AFPS_GameState : public AGameState
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    AFPS_GameState();

    ~AFPS_GameState()
    {
        CurrentGameState = nullptr;
    };
    //-------------------------------------------



public:

    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'AFPS_GameState'
    // @note    Используется для уменьшения зависимостей и использования излишних функций
    //          Например, функций 'Cast<>' и методов Инициализации в других классах
    static AFPS_GameState* CurrentGameState;

    //

    /** Метод проверки валидности статического указателя 'Current Game State' */
    FORCEINLINE static bool IsValidStaticPointer()
    {
        if (!CurrentGameState)
        {
            FPS_LOG_Empty(Error,
                "Current GameState is NOT 'AFPS_GameState' class. "
                "See Settings of current 'Game Mode'");
        }

        return bool(CurrentGameState);
    };
    //-------------------------------------------



    /* ---   Net   --- */

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



private:

    /* ---   Statistics   --- */

    /** Быстро реплицируемый Контейнер о Статистиках всех активных Игроков */
    UPROPERTY(Replicated)
    FPlayerStatisticsArray PlayersStatistics;
    //-------------------------------------------



    /* ---   friends   --- */

    friend class AFPS_GameMode;
    friend class UPlayerStatisticsWidget;
    //-------------------------------------------
};
