//

// Base:
#include "FPS_GameState.h"

// Net:
#include "Net/UnrealNetwork.h"
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Общедоступный указатель на текущий экземпляр класса 'AFPS_GameState'
AFPS_GameState* AFPS_GameState::CurrentGameState = nullptr;
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFPS_GameState::AFPS_GameState()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно

    // Настройка репликации
    bReplicates = true;
    SetReplicateMovement(false);
    //-------------------------------------------


    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'AFPS_GameState'
    CurrentGameState = this;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void AFPS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //-------------------------------------------


    /* ---   Statistics   --- */

    DOREPLIFETIME(AFPS_GameState, PlayersStatistics);
    DOREPLIFETIME(AFPS_GameState, CurrentRoundStatus);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Round Control   --- */

//--------------------------------------------------------------------------------------
