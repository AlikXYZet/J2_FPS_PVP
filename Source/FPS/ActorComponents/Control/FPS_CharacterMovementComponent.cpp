//

// Base:
#include "FPS_CharacterMovementComponent.h"

// UE:
#include "GameFramework/Character.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UFPS_CharacterMovementComponent::UFPS_CharacterMovementComponent()
{
    // Резервирование памяти массива: Два элемента используются по умолчанию, а третий -- запасной
    SpeedControllers.Reserve(3);
}
//--------------------------------------------------------------------------------------



/* ---   MovementMode   --- */

void UFPS_CharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    if (MovementMode == MOVE_Custom && CustomMovementMode != PreviousCustomMode)
    {
        SetSpeedType(ESpeedVariations(CustomMovementMode));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Speed Control   --- */

void UFPS_CharacterMovementComponent::UpdateMaxSpeed()
{
    if (SpeedControllers.Num())
    {
        ESpeedVariations lMax = ESpeedVariations::COUNT;

        for (ESpeedVariations* Data : SpeedControllers)
        {
            if (Data)
            {
                if (lMax == ESpeedVariations::COUNT || (lMax < *Data && *Data < ESpeedVariations::COUNT))
                {
                    lMax = *Data;
                }
            }
            else
            {
                SpeedControllers.Remove(Data);
            }
        }

        if (lMax != ESpeedVariations::COUNT)
        {
            SetSpeedType(lMax);
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Speed Control | MaxWalkSpeed   --- */

void UFPS_CharacterMovementComponent::SetMaxWalkSpeed(float Value)
{
    Server_SetMaxWalkSpeed(Value);
}

void UFPS_CharacterMovementComponent::Server_SetMaxWalkSpeed_Implementation(float Value)
{
    Multicast_SetMaxWalkSpeed(Value);
}

void UFPS_CharacterMovementComponent::Multicast_SetMaxWalkSpeed_Implementation(float Value)
{
    MaxWalkSpeed = Value;
}
//--------------------------------------------------------------------------------------
