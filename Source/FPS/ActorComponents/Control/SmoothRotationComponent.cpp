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
        && LockRotation != ERotationVariations::XYZ)
    {
        // Получение Текущей ротации
        FRotator lCurrentRotation = bUseRelativeRotation
            ? GetOwner()->GetRootComponent()->GetRelativeRotation() : GetOwner()->GetActorRotation();

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
            FRotator lNewSpeed = FRotator::ZeroRotator;

            // Переменная, используемая для проверки и изменении данных
            float lFloatChecker;

            if (!CheckLock(ERotationVariations::Pitch))
            {
                // Рассчитываем остаточный поворот
                lNewSpeed.Pitch = EndRotation.Pitch - lCurrentRotation.Pitch;

                // Корректировка направления
                if (-180 > lNewSpeed.Pitch || lNewSpeed.Pitch > 180)
                {
                    lNewSpeed.Pitch -= 360 * (lNewSpeed.Pitch < 0.f ? -1.f : 1.f);
                }

                // Корректировка коэффициентом
                lNewSpeed.Pitch *= DecelerationCoefficient;

                // Ограничение скорости с сохранением знака
                if (abs(lNewSpeed.Pitch) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Pitch = MaxSpeed * (lNewSpeed.Pitch < 0.f ? -1.f : 1.f);
                }

                // Отдаление от предыдущей Цели
                if (bControlSpeedAtStart)
                {
                    // Рассчитываем пройденный поворот
                    lFloatChecker = lCurrentRotation.Pitch - StartRotation.Pitch;

                    // Корректировка направления
                    if (-180 > lFloatChecker || lFloatChecker > 180)
                    {
                        lFloatChecker -= 360 * (lFloatChecker < 0.f ? -1.f : 1.f);
                    }

                    lFloatChecker *= AccelerationCoefficient;

                    if (abs(lNewSpeed.Pitch) > abs(lFloatChecker))
                    {
                        // Ограничение скорости с сохранением знака и начальным "толчком"
                        lNewSpeed.Pitch = lFloatChecker ? lFloatChecker : (MinStep * (lNewSpeed.Pitch < 0.f ? -1.f : 1.f));
                    }
                }
            }

            if (!CheckLock(ERotationVariations::Yaw))
            {
                // Рассчитываем остаточный поворот
                lNewSpeed.Yaw = EndRotation.Yaw - lCurrentRotation.Yaw;

                // Корректировка направления
                if (-180 > lNewSpeed.Yaw || lNewSpeed.Yaw > 180)
                {
                    lNewSpeed.Yaw -= 360 * (lNewSpeed.Yaw < 0.f ? -1.f : 1.f);
                }

                // Корректировка коэффициентом
                lNewSpeed.Yaw *= DecelerationCoefficient;

                // Ограничение скорости с сохранением знака
                if (abs(lNewSpeed.Yaw) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Yaw = MaxSpeed * (lNewSpeed.Yaw < 0.f ? -1.f : 1.f);
                }

                // Отдаление от предыдущей Цели
                if (bControlSpeedAtStart)
                {
                    // Рассчитываем пройденный поворот
                    lFloatChecker = lCurrentRotation.Yaw - StartRotation.Yaw;

                    // Корректировка направления
                    if (-180 > lFloatChecker || lFloatChecker > 180)
                    {
                        lFloatChecker -= 360 * (lFloatChecker < 0.f ? -1.f : 1.f);
                    }

                    lFloatChecker *= AccelerationCoefficient;

                    if (abs(lNewSpeed.Yaw) > abs(lFloatChecker))
                    {
                        // Ограничение скорости с сохранением знака и начальным "толчком"
                        lNewSpeed.Yaw = lFloatChecker ? lFloatChecker : (MinStep * (lNewSpeed.Yaw < 0.f ? -1.f : 1.f));
                    }
                }
            }

            if (!CheckLock(ERotationVariations::Roll))
            {
                // Рассчитываем остаточный поворот
                lNewSpeed.Roll = EndRotation.Roll - lCurrentRotation.Roll;

                // Корректировка направления
                if (-180 > lNewSpeed.Roll || lNewSpeed.Roll > 180)
                {
                    lNewSpeed.Roll -= 360 * (lNewSpeed.Roll < 0.f ? -1.f : 1.f);
                }

                // Корректировка коэффициентом
                lNewSpeed.Roll *= DecelerationCoefficient;

                // Ограничение скорости с сохранением знака
                if (abs(lNewSpeed.Roll) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Roll = MaxSpeed * (lNewSpeed.Roll < 0.f ? -1.f : 1.f);
                }

                // Отдаление от предыдущей Цели
                if (bControlSpeedAtStart)
                {
                    // Рассчитываем пройденный поворот
                    lFloatChecker = lCurrentRotation.Roll - StartRotation.Roll;

                    // Корректировка направления
                    if (-180 > lFloatChecker || lFloatChecker > 180)
                    {
                        lFloatChecker -= 360 * (lFloatChecker < 0.f ? -1.f : 1.f);
                    }

                    lFloatChecker *= AccelerationCoefficient;

                    if (abs(lNewSpeed.Roll) > abs(lFloatChecker))
                    {
                        // Ограничение скорости с сохранением знака и начальным "толчком"
                        lNewSpeed.Roll = lFloatChecker ? lFloatChecker : (MinStep * (lNewSpeed.Roll < 0.f ? -1.f : 1.f));
                    }
                }
            }

            // Расчёт Смещения ротации для плавного поворота
            lCurrentRotation = lNewSpeed * DeltaTime;

            if (bUseRelativeRotation)
            {
                GetOwner()->GetRootComponent()->AddRelativeRotation(lCurrentRotation);
            }
            else
            {
                GetOwner()->AddActorLocalRotation(lCurrentRotation);
            }
        }
    }
}
//--------------------------------------------------------------------------------------
