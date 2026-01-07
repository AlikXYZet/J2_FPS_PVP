//

// Base:
#include "PlayerCharacterState.h"

// Net:
#include "Net/UnrealNetwork.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

APlayerCharacterState::APlayerCharacterState()
{
    // Установка вызова функции Tick() в каждом кадре
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    SetActorTickInterval(1.f); // 1 раз/сек.
    //-------------------------------------------


    /* ---   Role Selection   --- */

    /** "Наблюдатель" по умолчанию */
    bIsSpectator = true;
    bOnlySpectator = true;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

//void APlayerCharacterState::BeginPlay()
//{
//    Super::BeginPlay();
//
//}

//void APlayerCharacterState::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//
//}
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

