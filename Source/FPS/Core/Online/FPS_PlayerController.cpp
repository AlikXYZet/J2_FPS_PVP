//

// Base:
#include "FPS_PlayerController.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFPS_PlayerController::AFPS_PlayerController()
{
    // Установка вызова функции Tick() в каждом кадре.
    PrimaryActorTick.bCanEverTick = true; // Принудительно
    //-------------------------------------------


    /* ---   Role Selection   --- */

    /** "Наблюдатель" по умолчанию
    @note   Заменяем `PC->StartSpectatingOnly();`, так как в данный момент:
    * `StateName == NAME_Spectating`;
    * `PlayerState == nullptr`. */
    bPlayerIsWaiting = false;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

//void AFPS_PlayerController::BeginPlay()
//{
//    Super::BeginPlay();
//
//}

void AFPS_PlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    SetMouseToCenter();
}
//--------------------------------------------------------------------------------------



/* ---   Mouse To Center   --- */

FORCEINLINE void AFPS_PlayerController::SetMouseToCenter()
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    if (bMouseToCenter)
    {
        if (GetPawn())
        {
            if (!IsPaused()
                && GetMousePosition(MousePositionX, MousePositionY))
            {
                GetViewportSize(SizeCenterX, SizeCenterY);

                if (MousePositionX != int32(SizeCenterX / 2) || MousePositionY != int32(SizeCenterY / 2))
                {
                    SetMouseLocation(SizeCenterX / 2, SizeCenterY / 2);
                }
            }
        }
        else
        {
            // Флаг: Прекратить контроль Мыши
            bMouseToCenter = false;
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Match Management   --- */

const AFPS_GameState* AFPS_PlayerController::BP_GetFPSGameState()
{
    return GetFPSGameState();
}

bool AFPS_PlayerController::IsMatchInProgress() const
{
    return GetFPSGameState()->IsMatchInProgress();
}
//--------------------------------------------------------------------------------------



/* ---   Role Selection   --- */

void AFPS_PlayerController::Server_GoToSpectators_Implementation() const
{
    GetFPSGameState()->ClientGoToSpectators(PlayerState);
}

void AFPS_PlayerController::Server_GoToPlayers_Implementation() const
{
    GetFPSGameState()->ClientGoToPlayers(PlayerState);
}

void AFPS_PlayerController::Server_SetMatchReadiness_Implementation(bool bReadiness) const
{
    GetFPSGameState()->SetPlayerReadiness(PlayerState, bReadiness);
}

void AFPS_PlayerController::Client_ChangedSelectedRole_Implementation(bool bIsPlayer) const
{
    OnSelectedRoleChange.Broadcast(bIsPlayer);

    // Если роль сменилась на "Наблюдатель", то уведомить о смене готовности игрока как "false"
    if (!bIsPlayer)
    {
        OnPlayerReadinessChange.Broadcast(false);
    }
}

void AFPS_PlayerController::Client_ChangedMatchReadiness_Implementation(bool bReadiness) const
{
    OnPlayerReadinessChange.Broadcast(bReadiness);
}
//--------------------------------------------------------------------------------------
