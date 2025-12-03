//

// Base:
#include "SmoothRotationComponent.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "Kismet/KismetMathLibrary.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

USmoothRotationComponent::USmoothRotationComponent()
{
    // Set this actor to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryComponentTick.bCanEverTick = true; // Warning: Принудительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

//void USmoothRotationComponent::BeginPlay()
//{
//    Super::BeginPlay();
//}

void USmoothRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    RotationForTick(DeltaTime);
}

//void USmoothRotationComponent::OnComponentCreated()
//{
//    Super::OnComponentCreated();
//}

//void USmoothRotationComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
//{
//    Super::OnComponentDestroyed(bDestroyingHierarchy);
//}
//--------------------------------------------------------------------------------------



/* ---   Actor Rotate   --- */

void USmoothRotationComponent::RotateToLocation(const FVector& iPoint)
{
    RotateToRotator(UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), iPoint));
}

void USmoothRotationComponent::RotateToRotator(const FRotator& iRotator)
{
    StartRotation = bUseRelativeRotation
        ? GetOwner()->GetRootComponent()->GetRelativeRotation() : GetOwner()->GetActorRotation();

    EndRotation = iRotator;
    bIsRotatedToNewRotation = true;
}

void USmoothRotationComponent::RotationForTick(float DeltaTime)
{
    // Контроль вращения
    if (bIsRotatedToNewRotation
        && !CheckRotationLock(ERotationVariations::XYZ))
    {
        FRotator lNewRotation = bUseRelativeRotation
            ? GetOwner()->GetRootComponent()->GetRelativeRotation() : GetOwner()->GetActorRotation();

        FRotator lCurrentRotation = lNewRotation;

        // Контроль близости к новой ротации
        if ((lCurrentRotation - EndRotation).IsNearlyZero(MinStep))
        {
            if (bUseRelativeRotation)
            {
                GetOwner()->SetActorRelativeRotation(EndRotation);
            }
            else
            {
                GetOwner()->SetActorRotation(EndRotation);
            }

            bIsRotatedToNewRotation = false;

            OnCompletedRotate.Broadcast();
        }
        else
        {
            // Рассчитанная угловая Скорость Вращения
            FRotator lNewSpeed = EndRotation - lCurrentRotation;

            // Переменная, используемая для проверки и изменении данных
            FRotator lChecker = lNewSpeed * DecelerationCoefficient;

            if (!CheckRotationLock(ERotationVariations::Pitch))
            {
                // Корректировка направления
                if (-180 > lNewSpeed.Pitch || lNewSpeed.Pitch > 180)
                {
                    lNewSpeed.Pitch -= 360 * signed(lNewSpeed.Pitch);
                }

                // Приближение к Цели
                if (abs(lChecker.Pitch) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Pitch = MaxSpeed * signed(lNewSpeed.Pitch);
                }
                else
                {
                    lNewSpeed.Pitch *= DecelerationCoefficient;
                }

                // Отдаление от предыдущей Цели
                if (bControlSpeedAtStart)
                {
                    // Пройденный поворот
                    lChecker.Pitch = (lCurrentRotation - StartRotation).Pitch;

                    // Корректировка направления
                    if (-180 > lChecker.Pitch || lChecker.Pitch > 180)
                    {
                        lChecker.Pitch -= 360 * signed(lChecker.Pitch);
                    }

                    lChecker.Pitch *= AccelerationCoefficient;

                    if (lNewSpeed.Pitch > lChecker.Pitch)
                    {
                        lNewSpeed.Pitch = (abs(lChecker.Pitch) + MinStep) * signed(lNewSpeed.Pitch);
                    }
                }
            }

            if (!CheckRotationLock(ERotationVariations::Yaw))
            {
                // Корректировка направления
                if (-180 > lNewSpeed.Yaw || lNewSpeed.Yaw > 180)
                {
                    lNewSpeed.Yaw -= 360 * signed(lNewSpeed.Yaw);
                }

                // Приближение к Цели
                if (abs(lChecker.Yaw) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Yaw = MaxSpeed * signed(lNewSpeed.Yaw);
                }
                else
                {
                    lNewSpeed.Yaw *= DecelerationCoefficient;
                }

                // Отдаление от предыдущей Цели
                if (bControlSpeedAtStart)
                {
                    // Пройденный поворот
                    lChecker.Yaw = (lCurrentRotation - StartRotation).Yaw;

                    // Корректировка направления
                    if (-180 > lChecker.Yaw || lChecker.Yaw > 180)
                    {
                        lChecker.Yaw -= 360 * signed(lChecker.Yaw);
                    }

                    lChecker.Yaw *= AccelerationCoefficient;

                    if (lNewSpeed.Yaw > lChecker.Yaw)
                    {
                        lNewSpeed.Yaw = (abs(lChecker.Yaw) + MinStep) * signed(lNewSpeed.Yaw);
                    }
                }
            }

            if (!CheckRotationLock(ERotationVariations::Roll))
            {
                // Корректировка направления
                if (-180 > lNewSpeed.Roll || lNewSpeed.Roll > 180)
                {
                    lNewSpeed.Roll -= 360 * signed(lNewSpeed.Roll);
                }

                // Приближение к Цели
                if (abs(lChecker.Roll) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Roll = MaxSpeed * signed(lNewSpeed.Roll);
                }
                else
                {
                    lNewSpeed.Roll *= DecelerationCoefficient;
                }

                // Отдаление от предыдущей Цели
                if (bControlSpeedAtStart)
                {
                    // Пройденный поворот
                    lChecker.Roll = (lCurrentRotation - StartRotation).Roll;

                    // Корректировка направления
                    if (-180 > lChecker.Roll || lChecker.Roll > 180)
                    {
                        lChecker.Roll -= 360 * signed(lChecker.Roll);
                    }

                    lChecker.Roll *= AccelerationCoefficient;

                    if (lNewSpeed.Roll > lChecker.Roll)
                    {
                        lNewSpeed.Roll = (abs(lChecker.Roll) + MinStep) * signed(lNewSpeed.Roll);
                    }
                }
            }

            lNewRotation = lNewSpeed * DeltaTime;

            if (bUseRelativeRotation)
            {
                GetOwner()->GetRootComponent()->AddRelativeRotation(lNewRotation);
            }
            else
            {
                GetOwner()->AddActorLocalRotation(lNewRotation);
            }
        }
    }
}
//--------------------------------------------------------------------------------------
