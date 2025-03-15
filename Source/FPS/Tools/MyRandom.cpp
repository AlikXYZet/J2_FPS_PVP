//

// Base:
#include "MyRandom.h"
//--------------------------------------------------------------------------------------



/* ---   My Random   --- */

int32 GetRandom(const int32& max, const int32& min)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32> distrib(min, max);

    return distrib(gen);
}

bool GetRandom(const float& iChance)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(iChance);

    return d(gen);
}

float GetRandomFloat(const float& max, const float& min)
{
    return GetRandom(max * 256, min * 256) / 256.f;
}
//--------------------------------------------------------------------------------------
