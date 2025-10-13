#pragma once

// Core:
#include "CoreMinimal.h"
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

/** Получить Ряд Чисел в диапазоне [Begin; End)
    @warning    Предполагается, что 'End > Begin' */
static TArray<int32> GetNumbersSeries(int32 Begin, const int32& End)
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
//--------------------------------------------------------------------------------------
