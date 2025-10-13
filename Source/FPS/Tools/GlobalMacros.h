#pragma once

// Core:
#include "CoreMinimal.h"

/* Пример:

// Macros:
#include "FPS/Tools/GlobalMacros.h"

*/
//--------------------------------------------------------------------------------------



/* ---   Init LogFPS   --- */

FPS_API DECLARE_LOG_CATEGORY_EXTERN(LogFPS, All, All);
//--------------------------------------------------------------------------------------



/* ---   Collision Channels   --- */

#define ECC_Projectiles ECC_GameTraceChannel1
//--------------------------------------------------------------------------------------



/* ---   UE_LOG   --- */

/** Макрос, который выводит отформатированное сообщение в журнал под категорией 'LogFPS'
@param  Verbosity - Уровень информации (Error, Warning и др.)
@param  Format - Формат текста */
#define FPS_LOG_Empty(Verbosity, Format, ...) UE_LOG(LogFPS, Verbosity, TEXT("" Format), ##__VA_ARGS__)

/** Макрос, который выводит отформатированное сообщение в журнал под категорией 'LogFPS'
    и дополнительную информацию о экземпляре класса и методе класса, где было вызвано сообщение

@param  Verbosity - Уровень информации (Error, Warning и др.)
@param  Format - Формат текста

@note   Не используем '*FString(__FUNCTION__)', так как требуется знать экземпляр класса */
#define FPS_LOG(Verbosity, Format, ...) \
{ \
	UE_LOG(LogFPS, Verbosity, \
        TEXT("'%s'::'%s': " Format), \
        *GetNameSafe(this), *FString(__func__),\
        ##__VA_ARGS__); \
}

/** Макрос, который выводит отформатированное сообщение в журнал под категорией 'LogFPS'
    и дополнительную информацию о структуре и его методе, где было вызвано сообщение

@param  Verbosity - Уровень информации (Error, Warning и др.)
@param  Format - Формат текста

@note   Можно было бы заменить на универсальный '*FString(__FUNCTION__)' */
#define FPS_LOG_Struct(Verbosity, Format, ...) \
{ \
	UE_LOG(LogFPS, Verbosity, \
        TEXT("'%s'::'%s': " Format), \
        *FString(typeid(*this).name()), *FString(__func__), \
        ##__VA_ARGS__); \
}

/** Макрос, который выводит отформатированное сообщение в журнал под категорией 'LogFPS'
    и дополнительную информацию о экземплярах класса-владельца и текущего класса-компонента,
    а также о методе класса, где было вызвано сообщение

@param  Verbosity - Уровень информации (Error, Warning и др.)
@param  Format - Формат текста

@note   Не используем '*FString(__FUNCTION__)', так как требуется знать экземпляр класса */
#define FPS_LOG_Component(Verbosity, Format, ...) \
{ \
	UE_LOG(LogFPS, Verbosity, \
        TEXT("'%s'::'%s'::'%s': " Format), \
        *GetNameSafe(GetOwner()), *GetNameSafe(this), *FString(__func__), \
        ##__VA_ARGS__); \
}

/** Примеры использования:

FPS_LOG_Empty(Warning, "CurrentWeaponData is NOT");

FPS_LOG(Error, "CurrentWeaponData is NOT");

FPS_LOG(Error, "lNewSpeed == %s",
    *lNewSpeed.ToString());

FPS_LOG(Warning, "'%s' is NOT 'AFPS_GameState'",
    GetWorld()->GetGameState<AGameStateBase>()
    ? *GetWorld()->GetGameState<AGameStateBase>()->GetFName().ToString()
    : *FString("None"));

FPS_LOG_Struct(Error, "lInputComponent is NOT");

*/

//--------------------------------------------------------------------------------------



/* ---   GEngine Message   --- */

/** Макрос, который выводит на экран сообщение (красным цветом)
@param  Format - Формат текста */
#define FPS_Message(Format, ...) FPS_ColorMessage(FColor::Red, Format, ##__VA_ARGS__)

/** Макрос, который выводит на экран сообщение с указанным цветом
@param  Color - Цвет текста
@param  Format - Формат текста */
#define FPS_ColorMessage(Color, Format, ...) \
{ \
    GEngine->AddOnScreenDebugMessage(-1, 60.f, Color, FString::Printf(TEXT("" Format), ##__VA_ARGS__)); \
}
//--------------------------------------------------------------------------------------



/* ---   Sorting Predicate   --- */

/** Макрос: Предикат сортировки
@param  Value - Переменная класса или структуры, значение которого используется для сортировки
@param  Operator - Оператор сортировки */
#define SORTING_PREDICATE(Value, Operator) \
    [](const auto& first, const auto& second) \
    { \
        return \
            first.##Value \
            ##Operator## \
            second.##Value; \
    };

/** Макрос: Предикат сортировки по Указателю
@param  Ptr - Переменная класса или структуры, что является Указателем
@param  Value - Переменная из Указателя, значение которого используется для сортировки
@param  Operator - Оператор сортировки */
#define SORTING_PREDICATE_ByPtr(Ptr, Value, NoneValue, Operator) \
    [](const auto& first, const auto& second) \
    { \
        return \
            (IsValid(first.##Ptr) ? first.##Ptr->##Value : ##NoneValue) \
            ##Operator## \
            (IsValid(second.##Ptr) ? second.##Ptr->##Value : ##NoneValue); \
    };
//--------------------------------------------------------------------------------------
