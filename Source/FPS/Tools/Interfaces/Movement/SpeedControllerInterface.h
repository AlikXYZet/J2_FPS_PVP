// 

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "UObject/Interface.h"

// Structs:
#include "FPS/Tools/Structs/Movement/SpeedControlData.h"

// Generated:
#include "SpeedControllerInterface.generated.h"
//--------------------------------------------------------------------------------------



// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class USpeedControllerInterface : public UInterface
{
    GENERATED_BODY()
};



/* Интерфейс Контроллера скорости
* @note Используется как шаблон для взаимодействия с "UFPS_CharacterMovementComponent"
*/
class FPS_API ISpeedControllerInterface
{
    GENERATED_BODY()

public:

    /* ---   Speed Control   --- */

    /** Задать значение скорости */
    UFUNCTION()
    virtual void SetSpeedControl(ESpeedVariations Mode) = 0;
    /*{
        // На примере "APlayerCharacter":
        if (SpeedControl != Mode)
        {
            SpeedControl = Mode;
            GetFPSCharacterMovement()->UpdateMaxSpeed();
        }
    };*/
    //-------------------------------------------



private:

    /* ---   Speed Control   --- */

    /** Базовая инициализация для работоспособности */
    virtual void InitSpeedControl() = 0;
    /*{
        // На примере "APlayerCharacter":
        GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
        SetSpeedControl(ESpeedVariations::Jog);
    };*/
    //-------------------------------------------
};
