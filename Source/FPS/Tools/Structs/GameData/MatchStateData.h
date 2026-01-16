//

#pragma once

// Core:
#include "CoreMinimal.h"

// Generated:
#include "MatchStateData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   ENums   --- */

// Состояние матча
UENUM(BlueprintType)
enum struct EMatchState : uint8
{
    // 'EnteringMap' or 'WaitingToStart'
    WaitingToStart      UMETA(DisplayName = "Waiting to Start"),
    PreProgress         UMETA(DisplayName = "Pre Progress"),
    InProgress          UMETA(DisplayName = "In Progress"),
    WaitingPostMatch    UMETA(DisplayName = "Waiting post Match"),
    LeavingMap          UMETA(DisplayName = "Leaving Map"),
    Aborted,
    Custom,
};
//----------------------------------------------------------------------------------------
