#pragma once

// Base:
#include "CoreMinimal.h"

// C++:
#include <random>
//--------------------------------------------------------------------------------------



/* ---   My Random   --- */

// Получение рандомного значения [min, max]
FORCEINLINE int32 GetRandom(const int32& max, const int32& min = 0)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32> distrib(min, max);

    return distrib(gen);
};

// Получение рандомного bool значения
FORCEINLINE bool GetRandom(const float& Chance = 0.5f)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(Chance);

    return d(gen);
};

// Получение рандомного float значения
FORCEINLINE float GetRandomFloat(const float& max = 1.f, const float& min = 0.f)
{
    return GetRandom(max * 256, min * 256) / 256.f;
};
//--------------------------------------------------------------------------------------
