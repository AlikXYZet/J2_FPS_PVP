//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameplayEffectExecutionCalculation.h"

// Generated:
#include "ExplosionDamageExecution.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UExplosionDamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UExplosionDamageExecution();
    //-------------------------------------------



public:

    /* ---   Calculation   --- */

    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
    //-------------------------------------------
};
