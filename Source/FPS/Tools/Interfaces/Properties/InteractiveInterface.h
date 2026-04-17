// 

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
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

    /* ---   Interactive : Base   --- */

    /** Инициализация компонента Интерактивности */
    virtual void InitInteractiveComponent() = 0;
    //{
        // Пример:
        //InteractiveComponent->UsedComponents.AddUnique({ BlockMesh, 0 });
        //InteractiveComponent->AddNamePredicate("CheckHighlighting");
        //InteractiveComponent->OnOwnerWasClickedLocally.AddDynamic(this, &IInteractiveInterface::ProcessInteractiveAction);
    //};
    //-------------------------------------------



    /* ---   Interactive : Highlighting   --- */

    /** Предикат условия выделения */
    UFUNCTION()
    virtual bool CheckHighlighting() const { return true; };
    //-------------------------------------------



    /* ---   Interactive : Actions   --- */

    /** Обработка интерактивного действия (клик по Актеру) */
    UFUNCTION()
    virtual void ProcessInteractiveAction(const FKey& ButtonReleased) {};
    //-------------------------------------------
};

