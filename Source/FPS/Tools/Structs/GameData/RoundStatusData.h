//

#pragma once

// Core:
#include "CoreMinimal.h"

// Generated:
#include "RoundStatusData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Enums   --- */

UENUM()
enum struct ERoundStatus : uint8
{
    // Период Выбора До начала Раунда
    Selection,
    // Период Раунда
    Round,
    // Период Результатов После завершения Раунда
    Results,

    // Максимальное значение (НЕ состояние)
    MAX   UMETA(Hidden)
};
//--------------------------------------------------------------------------------------
