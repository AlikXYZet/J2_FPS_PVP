//

#pragma once

// Core:
#include "CoreMinimal.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Generated:
#include "NetworkInteractionType.generated.h"
//--------------------------------------------------------------------------------------



/* ---   ENums   --- */

// Тип Сетевого взаимодействия
UENUM(BlueprintType)
enum struct ENetworkInteractionType : uint8
{
    Local       UMETA(ToolTip = "Local Only"),
    ToServer    UMETA(DisplayName = "To Server",    ToolTip = "Local and Server"),
    ToOwner     UMETA(DisplayName = "To Owner",     ToolTip = "Local and Owner"),
    ToMulticast UMETA(DisplayName = "To Multicast", ToolTip = "All (Multicast)"),
};
ENUM_COMPARISON_OPERATORS(ENetworkInteractionType);
//----------------------------------------------------------------------------------------

