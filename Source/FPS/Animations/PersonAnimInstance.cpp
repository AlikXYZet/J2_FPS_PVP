//

// Base:
#include "PersonAnimInstance.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/CharacterMovementComponent.h"

// Interaction:
#include "FPS/ActorComponents/Data/WeaponNetworkController.h"
#include "FPS/Characters/PlayerCharacter.h"
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
}
//--------------------------------------------------------------------------------------



/* ---   Customized Base   --- */

void UPersonAnimInstance::CustomUpdateAnimation()
{
    CurrentMovementSpeed = PlayerOwner->GetVelocity().Size();
    CurrentMovementDirection = CalculateDirection(PlayerOwner->GetVelocity(), PlayerOwner->GetActorRotation());

    // Задействуем готовую в APawn реплицируемую переменную Вертикального Взора.
    // Дабы избежать "разрыва" при плавной анимации между значениями 254 и 0, сделаем преобразование в знаковый тип данных "int8"
    if (PlayerOwner)
    {
        if (PlayerOwner->IsLocallyControlled())
        {
            // Для Локального контроллера
            if (PlayerOwner->GetController())
            {
                float NewRemoteViewPitch = FRotator::ClampAxis(PlayerOwner->GetController()->GetControlRotation().Pitch);
                CurrentVerticalViewingAngle = (int8)(NewRemoteViewPitch * 255.f / 360.f);
            }
            // PS: PlayerOwner->RemoteViewPitch не реплицируется для локального (не сетевого) Контроллера
        }
        else
        {
            // Для Сетевого контроллера
            CurrentVerticalViewingAngle = (int8)PlayerOwner->RemoteViewPitch;
            // @warning:
            // RemoteViewPitch изменяется в следующем диапазоне (вниз-вверх):
            // 191-254 и 0-63 (byte, uint8)
            // Для удобства анимации, конвертируем в 'int8' и получаем следующий Непрерывный диапазон:
            // -65 - 63 
        }
    }

    Event_CustomUpdateAnimation();
}

void UPersonAnimInstance::SetTimeForUpdateAnimation(float iTime)
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
    PlayerOwner = Cast<APlayerCharacter>(GetOwningActor());

    if (PlayerOwner)
    {
        PlayerOwner->MovementModeChangedDelegate.AddDynamic(this, &UPersonAnimInstance::UpdateMovementMode);

        WeaponControlNetComp = PlayerOwner->WeaponControlNetComp;

        if (WeaponControlNetComp)
        {
            WeaponControlNetComp->OnChangingWeapon.AddDynamic(this, &UPersonAnimInstance::UpdateBasicData);
        }
        else
        {
            FPS_LOG(Error, TEXT("WeaponControlNetComp is NOT"));
        }
    }
    else
    {
        FPS_LOG(Error, TEXT("PlayerOwner is NOT"));
    }

    // Timer_CustomUpdateAnimation
    GetWorld()->GetTimerManager().SetTimer(Timer_CustomUpdateAnimation, this, &UPersonAnimInstance::CustomUpdateAnimation, TimeForCustomUpdateAnimation, true);
}
//--------------------------------------------------------------------------------------



/* ---   Movements   --- */

void UPersonAnimInstance::UpdateMovementMode(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
    CurrentMovementMode = Character->GetCharacterMovement()->MovementMode;

    Event_UpdateMovementMode(CurrentMovementMode, Character->GetCharacterMovement()->CustomMovementMode);
}
//--------------------------------------------------------------------------------------



/* ---   Player Actions   --- */

bool UPersonAnimInstance::CheckAction(EActionVariations InAction) const
{
    return WeaponControlNetComp
        && WeaponControlNetComp->CheckAction(InAction);
}

bool UPersonAnimInstance::CheckAllActions(const EActionVariations A, const EActionVariations B) const
{
    return WeaponControlNetComp
        && !(~(WeaponControlNetComp->CurrentActions) & uint8(A | B));
}

bool UPersonAnimInstance::CheckOneOfActions(const EActionVariations A, const EActionVariations B) const
{
    return WeaponControlNetComp
        && WeaponControlNetComp->CurrentActions & uint8(A | B);
}

bool UPersonAnimInstance::CheckExceptActions(const EActionVariations A, const EActionVariations B) const
{
    return WeaponControlNetComp
        && WeaponControlNetComp->CurrentActions & ~uint8(A | B);
}

void UPersonAnimInstance::UpdateBasicData(const FWeaponData& CurrentNewWeaponData)
{
    PersonAnimations = CurrentNewWeaponData.PersonAnimations;
}
//--------------------------------------------------------------------------------------
