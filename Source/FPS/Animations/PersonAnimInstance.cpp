//

// Base:
#include "PersonAnimInstance.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Interaction:
#include "FPS/ActorComponents/Control/FPS_CharacterMovementComponent.h"
#include "FPS/ActorComponents/Data/WeaponControlComponent.h"
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Combat/WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UPersonAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
}

void UPersonAnimInstance::NativeUninitializeAnimation()
{
    Super::NativeUninitializeAnimation();
}

void UPersonAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    BaseInit();
    MovementInit();
}
//--------------------------------------------------------------------------------------



/* ---   Customized Base   --- */

void UPersonAnimInstance::CustomUpdateAnimation()
{
    CurrentMovementSpeed = PlayerOwner->GetVelocity().Size();

    EventCustomUpdateAnimation();
}

void UPersonAnimInstance::SetTimeForUpdateAnimation(const float& iTime)
{
    if (iTime > 0)
    {
        TimeForCustomUpdateAnimation = iTime;

        GetWorld()->GetTimerManager().SetTimer(Timer_CustomUpdateAnimation, this, &UPersonAnimInstance::CustomUpdateAnimation, TimeForCustomUpdateAnimation, true);

    }
    else if (iTime == 0)
    {
        TimeForCustomUpdateAnimation = 0;

        GetWorld()->GetTimerManager().ClearTimer(Timer_CustomUpdateAnimation);
    }
}

void UPersonAnimInstance::BaseInit()
{
    PlayerOwner = Cast<APlayerCharacter>(TryGetPawnOwner());

    if (PlayerOwner)
    {
        WeaponControlComponent = PlayerOwner->WeaponControlComp;

        if (!WeaponControlComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: WeaponControlComponent is NOT"),
                *GetNameSafe(this), *FString(__func__));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: PlayerOwner is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }

    // Timer_CustomUpdateAnimation
    GetWorld()->GetTimerManager().SetTimer(Timer_CustomUpdateAnimation, this, &UPersonAnimInstance::CustomUpdateAnimation, TimeForCustomUpdateAnimation, true);
}
//--------------------------------------------------------------------------------------



/* ---   Movement Mode   --- */

void UPersonAnimInstance::MovementInit()
{
    if (PlayerOwner)
    {
        PlayerOwner->MovementModeChangedDelegate.AddDynamic(this, &UPersonAnimInstance::UpdateMovementMode);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: PlayerOwner is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
}

void UPersonAnimInstance::UpdateMovementMode(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
    CurrentMovementMode = Character->GetCharacterMovement()->MovementMode;

    EventUpdateMovementMode(CurrentMovementMode, Character->GetCharacterMovement()->CustomMovementMode);
}
//--------------------------------------------------------------------------------------



/* ---   Player Actions   --- */

bool UPersonAnimInstance::CheckAction(const EActionVariations& InAction) const
{
    return WeaponControlComponent
        && WeaponControlComponent->CheckAction(InAction);
}
//--------------------------------------------------------------------------------------
