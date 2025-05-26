//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

// Generated:
#include "FPS_CharacterMovementComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При Новом режиме перемещения
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewMovementMode, EMovementMode, MovementMode, uint8, CustomMovementMode);
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UFPS_CharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    // Делегат: При Новом режиме перемещения
    UPROPERTY(BlueprintAssignable)
    FOnNewMovementMode OnNewMovementMode;
    //-------------------------------------------



    /* ---   MovementMode   --- */

    virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
    //-------------------------------------------
};
