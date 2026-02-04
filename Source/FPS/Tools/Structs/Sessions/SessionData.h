//

#pragma once

// Core:
#include "CoreMinimal.h"

// UE:
#include "Engine/DataTable.h"

// Generated:
#include "SessionData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
//class ;
//--------------------------------------------------------------------------------------



/* Структура Таблиц Данных:
* Выбор Карты уровня */
USTRUCT(BlueprintType)
struct FMapSelection : public FTableRowBase
{
    GENERATED_BODY()

    // Тип Поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MapName = FString("None");

    // Звук, соответствующий поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UWorld> MenuMap;
};
//--------------------------------------------------------------------------------------
