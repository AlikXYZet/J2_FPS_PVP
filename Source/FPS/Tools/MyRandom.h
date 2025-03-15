#pragma once

// Base:
#include "CoreMinimal.h"

// C++:
#include <random>
//--------------------------------------------------------------------------------------



/* ---   My Random   --- */

// Получение рандомного значения [min, max]
int32 GetRandom(const int32& max, const int32& min = 0);

// Получение рандомного bool значения
bool GetRandom(const float& Chance = 0.5f);

// Получение рандомного float значения
float GetRandomFloat(const float& max = 1.f, const float& min = 0.f);
//--------------------------------------------------------------------------------------
