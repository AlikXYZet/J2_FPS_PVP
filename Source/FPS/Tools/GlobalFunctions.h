#pragma once

// Core:
#include "CoreMinimal.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
//--------------------------------------------------------------------------------------



/* ---   constexpr: Шаблоны проверки соответствия типа на уровне компилятора   --- */

/** Является ли классом типа 'AActor' */
template<class T>
constexpr bool is_Actor = std::is_convertible<T*, AActor*>::value;


/** Является ля контейнером */
template<typename, typename = void>
constexpr bool is_container = false;

template<typename T>
constexpr bool is_container<
    T,
    std::void_t<    // Проверки на наличие функций и переменных
    decltype(std::declval<T>().begin()),
    decltype(std::declval<T>().end()),
    typename T::value_type
    >
> = true;
//----------------------------------------------------------------------------------------------------



/* ---   Statics   --- */

/** Получить Ряд Чисел в диапазоне [Begin; End)
@warning    Предполагается, что 'End > Begin' */
static TArray<int32> GetNumbersSeries(int32 Begin, int32 End)
{
    TArray<int32> Result;
    Result.Empty(End - Begin);

    while (Begin != End)
    {
        Result.Add(Begin);
        ++Begin;
    }

    return Result;
};

/** Получить первый элемент мира соответствующего класса
@note   Возможно, можно сильнее упростить получение "первого" элемента на карте по его типу.
        Однако, в местах текущего использования это не столь важно... */
template <class ActorType,
    class = std::enable_if_t<is_Actor<ActorType>>>
static ActorType* GetFirstActorOfType()
{
    TActorIterator<ActorType> It(GEngine->GameViewport->GetWorld(), ActorType::StaticClass());
    if (It)
    {
        return *It;
    }
    else
    {
        FPS_LOG_Static(Error,
            "At Level there is NO Actor of '%s' class",
            *ActorType::StaticClass()->GetName());
        return nullptr;
    }
}
//--------------------------------------------------------------------------------------
