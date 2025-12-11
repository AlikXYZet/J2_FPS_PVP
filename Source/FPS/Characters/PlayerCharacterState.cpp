//

// Base:
#include "PlayerCharacterState.h"

// Net:
#include "Net/UnrealNetwork.h"
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void APlayerCharacterState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //-------------------------------------------


    /* ---   Statistics   --- */

    DOREPLIFETIME(APlayerCharacterState, CurrentNetStatus);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------

