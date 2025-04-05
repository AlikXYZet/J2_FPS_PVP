//

// Base:
#include "FPS_CharacterMovementComponent.h"
//--------------------------------------------------------------------------------------



/* ---   MovementMode   --- */

void UFPS_CharacterMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
    if (OnNewMovementMode.IsBound())
    {
        EMovementMode lCurrentMode = MovementMode;
        
        Super::SetMovementMode(NewMovementMode, NewCustomMode);

        if (lCurrentMode != MovementMode)
        {
            OnNewMovementMode.Broadcast(MovementMode, CustomMovementMode);
        }
    }
    else
    {
        Super::SetMovementMode(NewMovementMode, NewCustomMode);
    }
}
//--------------------------------------------------------------------------------------
