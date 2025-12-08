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

@note   Используем '*FString(__FUNCTION__)', так как экземпляр класса может не существовать */
#define FPS_LOG_Static(Verbosity, Format, ...) \
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

FPS_LOG_Static(Error, "lInputComponent is NOT");

*/

//--------------------------------------------------------------------------------------



/* ---   GEngine Message   --- */

/** Макрос, который выводит на экран сообщение с указанным цветом
@param  Color - Цвет текста
@param  Format - Формат текста */
#define FPS_ColorMessage_Empty(Color, Format, ...) \
{ \
    GEngine->AddOnScreenDebugMessage(-1, 600.f, Color, FString::Printf(TEXT("" Format), ##__VA_ARGS__)); \
}

/** Макрос, который выводит на экран сообщение (красным цветом)
@param  Format - Формат текста */
#define FPS_Message_Empty(Format, ...) FPS_ColorMessage_Empty(FColor::Red, Format, ##__VA_ARGS__)


/** Макрос, который выводит на экран сообщение с указанным цветом
    и дополнительную информацию о экземпляре класса и методе класса, где было вызвано сообщение
@param  Color - Цвет текста
@param  Format - Формат текста */
#define FPS_ColorMessage(Color, Format, ...) \
    FPS_ColorMessage_Empty(\
        Color,\
        TEXT("'%s'::'%s': " Format), \
        *GetNameSafe(this), *FString(__func__), \
        ##__VA_ARGS__)

/** Макрос, который выводит на экран сообщение (красным цветом)
и дополнительную информацию о экземпляре класса и методе класса, где было вызвано сообщение
@param  Format - Формат текста */
#define FPS_Message(Format, ...) FPS_ColorMessage(FColor::Red, Format, ##__VA_ARGS__)

/** Макрос, который выводит на экран сообщение с указанным цветом
    и дополнительную информацию о классе и методе класса, где было вызвано сообщение

@param  Color - Цвет текста
@param  Format - Формат текста

@note   Используем '*FString(__FUNCTION__)', так как экземпляр класса может не существовать */
#define FPS_ColorMessage_Static(Color, Format, ...) \
    FPS_ColorMessage_Empty(\
        Color,\
        TEXT("'%s': " Format), \
        *FString(__FUNCTION__),\
        ##__VA_ARGS__)

/** Макрос, который выводит на экран сообщение (красным цветом)
    и дополнительную информацию о классе и методе класса, где было вызвано сообщение

@param  Format - Формат текста

@note   Используем '*FString(__FUNCTION__)', так как экземпляр класса может не существовать */
#define FPS_Message_Static(Format, ...) FPS_ColorMessage_Static(FColor::Red, Format, ##__VA_ARGS__)
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



/* ---   Booleanas   --- */

/** Макрос: Преобразование 'bool' в указатель 'FString' для использования в макросах 'LOG' и 'Message' */
#define BoolToString(BOOL) *FString(BOOL ? "TRUE" : "FALSE")
//--------------------------------------------------------------------------------------



/* ---   Примечание   --- */

/** Используемые форматы:
@see Engine/Source/Runtime/Core/Public/Templates/UnrealTypeTraits.h

// Базовые:
Expose_TFormatSpecifier(bool, "%i")     // Не работает для `bool`
Expose_TFormatSpecifier(uint8, "%u")
Expose_TFormatSpecifier(uint16, "%u")
Expose_TFormatSpecifier(uint32, "%u")
Expose_TFormatSpecifier(uint64, "%llu") // Подходит и для указателей
Expose_TFormatSpecifier(int8, "%d")
Expose_TFormatSpecifier(int16, "%d")
Expose_TFormatSpecifier(int32, "%d")
Expose_TFormatSpecifier(int64, "%lld")
Expose_TFormatSpecifier(float, "%f")
Expose_TFormatSpecifier(double, "%f")
Expose_TFormatSpecifier(long double, "%f")
Expose_TFormatSpecifier(long, "%ld")
Expose_TFormatSpecifier(unsigned long, "%lu")
*/
//--------------------------------------------------------------------------------------
