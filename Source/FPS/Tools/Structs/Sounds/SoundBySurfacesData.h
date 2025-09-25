//

#pragma once

// Core:
#include "CoreMinimal.h"

// UE:
#include "Engine/DataTable.h"

// Generated:
#include "SoundBySurfacesData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class AProjectile;
//--------------------------------------------------------------------------------------



/* Структура Таблиц Данных:
Звуки в зависимости от Типа Поверхности
*/
USTRUCT(BlueprintType)
struct FSoundBySurfacesType : public FTableRowBase
{
    GENERATED_BODY()

    // Тип Поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EPhysicalSurface> SurfaceType = EPhysicalSurface::SurfaceType_Default;

    // Звук, соответствующий поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* Sound = nullptr;
};
//--------------------------------------------------------------------------------------
