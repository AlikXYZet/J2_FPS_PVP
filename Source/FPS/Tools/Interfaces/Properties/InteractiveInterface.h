// 

#pragma once

// Base:
#include "CoreMinimal.h"
#include "UObject/Interface.h"

// Generated:
#include "InteractiveInterface.generated.h"
//--------------------------------------------------------------------------------------



// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractiveInterface : public UInterface
{
    GENERATED_BODY()
};



/* Интерфейс Интерактивности Актора
* @note Используется как шаблон для взаимодействия с "UInteractiveComponent"
*/
class FPS_API IInteractiveInterface
{
    GENERATED_BODY()

public:

    /* ---   Base   --- */

    // Базовая инициализация для работоспособности
    virtual void InitInteractiveComponent() = 0;
    /*{
        // Пример:
        InteractiveComponent->UsedComponents.AddUnique({ BlockMesh, 0 });
        InteractiveComponent->AddNamePredicate("CheckHighlighting");
        InteractiveComponent->OnOwnerWasClicked.AddDynamic(this, &IInteractiveInterface::ProcessInteractiveAction);
    };*/
    //-------------------------------------------



    /* ---   Highlighting   --- */

    /* Предикат условия выделения */
    UFUNCTION()
    virtual bool CheckHighlighting() const = 0;
    //-------------------------------------------



    /* ---   Actions   --- */

    // Обработка интерактивного действия (клик по Актеру)
    UFUNCTION()
    virtual void ProcessInteractiveAction(const FKey& ButtonReleased) = 0;
    //-------------------------------------------
};
