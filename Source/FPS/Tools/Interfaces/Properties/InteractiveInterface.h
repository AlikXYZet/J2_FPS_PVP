// 

#pragma once

// Core:
//#include "CoreMinimal.h"

// Base:
#include "UObject/Interface.h"

// Generated:
#include "InteractiveInterface.generated.h"
//--------------------------------------------------------------------------------------



// This class does not need to be modified.
UINTERFACE(Blueprintable)
class FPS_API UInteractiveInterface : public UInterface
{
    GENERATED_BODY()
};



/* Интерфейс Интерактивности Актора
* @note Используется как шаблон для взаимодействия с "UInteractiveComponent" */
class FPS_API IInteractiveInterface
{
    GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Проверка Объекта на реализацию данного Интерфейса */
    FORCEINLINE static bool CheckImplementation(const UObject* O)
    {
        return O->GetClass()->ImplementsInterface(UInteractiveInterface::StaticClass());
    };
    //-------------------------------------------



    /* ---   Highlighting   --- */

    /** Получить компоненты, которые требуется подсветить */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
        Category = "Interactive|Highlighting")
    TArray<FComponentRendering> GetUsedComponents();

    virtual TArray<FComponentRendering> GetUsedComponents_Implementation() { return TArray<FComponentRendering>(); };

    /** Предикат условия выделения (подсвечивания) Компонента */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
        Category = "Interactive|Highlighting")
    bool CheckHighlightCondition();

    virtual bool CheckHighlightCondition_Implementation() { return true; };
    //-------------------------------------------



    /* ---   Actions   --- */

    /** Предикат условия взаимодействия с Актором */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable,
        Category = "Interactive|Actions")
    bool CheckActionConditions(const FKey& ButtonReleased);

    virtual bool CheckActionConditions_Implementation(const FKey& ButtonReleased)
    {
        return true;
        // PS:  Вызов метода 'CheckHighlightCondition' из среды 'Blueprint' на данном этапе НЕ возможен,
        //      поэтому 'CheckActionConditions' не может дублировать его по умолчанию.
        //      В других случаях - вызов осуществляется через 'Execute_CheckHighlightCondition(UObject* O)'
    };
    //-------------------------------------------
};

