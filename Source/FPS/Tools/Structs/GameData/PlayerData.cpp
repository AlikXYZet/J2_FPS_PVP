// 

// Base:
#include "PlayerData.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"
//--------------------------------------------------------------------------------------



/* ---   Data Methods: Player State   --- */

void FPlayerData::UpdateDataOnPlayerState()
{
    PlayerNetStatus = AFPS_GameState::GetPlayerNetStatus(PlayerState);

    // Обновить Имя на актуальный
    if (PlayerNetStatus != EPlayerNetworkStatus::NONE
        && PlayerName != PlayerState->GetPlayerName())
    {
        PlayerName = PlayerState->GetPlayerName();
    }
}
//--------------------------------------------------------------------------------------
