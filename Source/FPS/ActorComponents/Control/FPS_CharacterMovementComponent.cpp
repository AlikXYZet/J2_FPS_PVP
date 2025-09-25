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

void UFPS_CharacterMovementComponent::SetCustomMovementMode(const ESpeedVariations& Mode)
{
    SetMovementMode(EMovementMode::MOVE_Custom, (uint8)Mode);
}
//--------------------------------------------------------------------------------------



/* ---   Speed Control   --- */

void UFPS_CharacterMovementComponent::AddSpeedControl(ESpeedVariations& Controller)
{
    if (SpeedControllers.Find(&Controller) == INDEX_NONE)
    {
        SpeedControllers.Add(&Controller);
        UpdateMaxSpeed();
    }
}

void UFPS_CharacterMovementComponent::RemoveSpeedControl(ESpeedVariations& Controller)
{
    SpeedControllers.Remove(&Controller);
    UpdateMaxSpeed();
}

void UFPS_CharacterMovementComponent::UpdateMaxSpeed()
{
    if (SpeedControllers.Num())
    {
        ESpeedVariations* lMax = nullptr;

        for (ESpeedVariations* Data : SpeedControllers)
        {
            if (Data)
            {
                if (!lMax || (*lMax < *Data && *Data < ESpeedVariations::COUNT))
                {
                    lMax = Data;
                }
            }
            else
            {
                SpeedControllers.Remove(Data);
            }
        }

        if (lMax)
        {
            SetSpeedType(*lMax);
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Speed Control | MaxWalkSpeed   --- */

void UFPS_CharacterMovementComponent::SetSpeedType(const ESpeedVariations& Type)
{
    if (Type < ESpeedVariations::COUNT
        && MaxWalkSpeed != BaseMovementSpeeds[(uint8)Type])
    {
        // Быстрое применение значения у Клиента-Владельца
        SetMaxWalkSpeed(BaseMovementSpeeds[(uint8)Type]);
    }
}

void UFPS_CharacterMovementComponent::SetMaxWalkSpeed(const float& Value)
{
    Server_SetMaxWalkSpeed(Value);
}

void UFPS_CharacterMovementComponent::Server_SetMaxWalkSpeed_Implementation(const float& Value)
{
    Multicast_SetMaxWalkSpeed(Value);
}

void UFPS_CharacterMovementComponent::Multicast_SetMaxWalkSpeed_Implementation(const float& Value)
{
    MaxWalkSpeed = Value;
}
//--------------------------------------------------------------------------------------
