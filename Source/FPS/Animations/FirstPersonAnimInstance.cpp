//

// Base:
#include "FirstPersonAnimInstance.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Interaction:
#include "FPS/ActorComponents/Data/WeaponLocalController.h"
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Combat/FirstPersonWeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UFirstPersonAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
}

void UFirstPersonAnimInstance::NativeUninitializeAnimation()
{
    Super::NativeUninitializeAnimation();
}

void UFirstPersonAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    InitIKAnim();
}
//--------------------------------------------------------------------------------------



/* ---   IK_Anim   --- */

const FTransform& UFirstPersonAnimInstance::GetRightHandTransform()
{
    return *RightHandTransform;
}

const FTransform& UFirstPersonAnimInstance::GetLeftHandTransform()
{
    return *LeftHandTransform;
}

void UFirstPersonAnimInstance::InitIKAnim()
{
    if (PlayerOwner
        && PlayerOwner->WeaponControlLocComp
        && PlayerOwner->WeaponControlLocComp->GetCurrentFPWeaponFrame())
    {
        CurrentFPWeaponFrame = PlayerOwner->WeaponControlLocComp->GetCurrentFPWeaponFrame();
    }

    if (CurrentFPWeaponFrame)
    {
        UpdateTransformPtrForHands();
    }
    else
    {
        FPS_LOG(Error, TEXT("CurrentFPWeaponFrame is NOT"));
    }
}

void UFirstPersonAnimInstance::UpdateTransformPtrForHands()
{
    RightHandTransform = &CurrentFPWeaponFrame->GripPoint->GetComponentTransform();
    LeftHandTransform = &CurrentFPWeaponFrame->HoldingPoint->GetComponentTransform();
}
//--------------------------------------------------------------------------------------
