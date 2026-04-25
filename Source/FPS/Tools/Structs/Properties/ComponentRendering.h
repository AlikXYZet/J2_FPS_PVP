//

#pragma once

// Core:
#include "CoreMinimal.h"

// Generated:
#include "ComponentRendering.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
//class ;
//--------------------------------------------------------------------------------------



/** Рендеринг (Визуализация) Компонентов */
USTRUCT(BlueprintType)
struct FComponentRendering
{
    GENERATED_BODY()

    // Используемый Компонент
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UPrimitiveComponent* Component = nullptr;

    // Значение глубины Выделения
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 DepthStencilValue = 0;
    //-------------------------------------------


    /* ---   Constructors   --- */

    FComponentRendering() {};
    FComponentRendering(UPrimitiveComponent* UsedComponent) : Component(UsedComponent) {};
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
