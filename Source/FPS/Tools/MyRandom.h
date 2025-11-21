#pragma once

// Core:
#include "CoreMinimal.h"

// C++:
#include <random>
//--------------------------------------------------------------------------------------



/* ---   My Random   --- */

// Получение рандомного значения [min, max]
FORCEINLINE int32 GetRandom(int32 max, int32 min = 0)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32> distrib(min, max);

    return distrib(gen);
};

// Получение рандомного bool значения
FORCEINLINE bool GetRandom(float Chance = 0.5f)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(Chance);

    return d(gen);
};

// Получение рандомного float значения
FORCEINLINE float GetRandomFloat(float max = 1.f, float min = 0.f)
{
    return GetRandom(max * 256, min * 256) / 256.f;
};
//--------------------------------------------------------------------------------------
