//

#pragma once

// Base:
#include "CoreMinimal.h"

// Generated:
#include "SpeedControlData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Enums   --- */

/** Вариации Скорости Игрока */
UENUM(BlueprintType)
enum struct ESpeedVariations : uint8
{
    // Спринт
    Sprint,
    // Бег
    Jog,
    // Ходьба
    Walk,

    // Праздный, Скорость на нуле
    Idle UMETA(Hidden), // Всегда предпоследний для максимального приоритета

    COUNT UMETA(Hidden) // Максимальное значение для определения количества
};
//--------------------------------------------------------------------------------------
