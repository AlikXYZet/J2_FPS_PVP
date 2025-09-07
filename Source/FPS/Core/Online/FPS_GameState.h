//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/GameState.h"

// Structs:
#include "FPS/Tools/Structs/Statistics/PlayerStatisticsData.h"

// Generated:
#include "FPS_GameState.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class AFPS_GameMode;
class APlayerCharacter;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AFPS_GameState : public AGameState
{
    GENERATED_BODY()

    friend AFPS_GameMode;

public:

    /* ---   Constructors   --- */

    AFPS_GameState();
    //-------------------------------------------



    /* ---   Net   --- */

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



    /* ---   Statistics   --- */


    //-------------------------------------------



private:

    /* ---   Statistics   --- */

    /** Быстро реплицируемый Контейнер о Статистиках всех активных Игроков */
    UPROPERTY(Replicated)
    FPlayerStatisticsArray PlayersStatistics;
    //-------------------------------------------
};
