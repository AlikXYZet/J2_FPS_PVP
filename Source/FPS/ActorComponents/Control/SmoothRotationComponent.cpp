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

void USmoothRotationComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USmoothRotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    RotationForTick(DeltaTime);
}

void USmoothRotationComponent::OnComponentCreated()
{
    Super::OnComponentCreated();

    InitCurrentComponent();
}

void USmoothRotationComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}
//--------------------------------------------------------------------------------------



/* ---   Init   --- */

void USmoothRotationComponent::InitCurrentComponent()
{
    CurrentActor = GetOwner();

    if (!CurrentActor)
    {
        FPS_LOG_Component(Error, TEXT("CurrentActor is NOT"));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actor Rotate   --- */

void USmoothRotationComponent::RotateToLocation(const FVector& iPoint)
{
    RotateToRotator(UKismetMathLibrary::FindLookAtRotation(CurrentActor->GetActorLocation(), iPoint));
}

void USmoothRotationComponent::RotateToRotator(const FRotator& iRotator)
{
    if (CurrentActor)
    {
        StartRotation = bUseRelativeRotation
            ? CurrentActor->GetRootComponent()->GetRelativeRotation() : CurrentActor->GetActorRotation();

        EndRotation = iRotator;
        bIsRotatedToNewRotation = true;
    }
}

void USmoothRotationComponent::RotationForTick(const float& DeltaTime)
{
    // Контроль вращения
    if (bIsRotatedToNewRotation
        && !CheckRotationLock(ERotationVariations::XYZ))
    {
        FRotator lNewRotation = bUseRelativeRotation
            ? CurrentActor->GetRootComponent()->GetRelativeRotation() : CurrentActor->GetActorRotation();

        FRotator lCurrentRotation = lNewRotation;

        // Контроль близости к новой ротации
        if ((lCurrentRotation - EndRotation).IsNearlyZero(MinStep))
        {
            if (bUseRelativeRotation)
            {
                CurrentActor->SetActorRelativeRotation(EndRotation);
            }
            else
            {
                CurrentActor->SetActorRotation(EndRotation);
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
                if (int(lNewSpeed.Pitch / 180))
                {
                    lNewSpeed.Pitch -= 360 * (lNewSpeed.Pitch > 0 ? 1 : -1);
                }

                // Приближение к Цели
                if (abs(lChecker.Pitch) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Pitch = MaxSpeed * (lNewSpeed.Pitch > 0 ? 1 : -1);
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
                    if (int(lChecker.Pitch / 180))
                    {
                        lChecker.Pitch -= 360 * (lChecker.Pitch > 0 ? 1 : -1);
                    }

                    lChecker.Pitch *= AccelerationCoefficient;

                    if (lNewSpeed.Pitch > lChecker.Pitch)
                    {
                        lNewSpeed.Pitch = (abs(lChecker.Pitch) + MinStep) * (lNewSpeed.Pitch > 0 ? 1 : -1);
                    }
                }
            }

            if (!CheckRotationLock(ERotationVariations::Yaw))
            {
                // Корректировка направления
                if (int(lNewSpeed.Yaw / 180))
                {
                    lNewSpeed.Yaw -= 360 * (lNewSpeed.Yaw > 0 ? 1 : -1);
                }

                // Приближение к Цели
                if (abs(lChecker.Yaw) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Yaw = MaxSpeed * (lNewSpeed.Yaw > 0 ? 1 : -1);
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
                    if (int(lChecker.Yaw / 180))
                    {
                        lChecker.Yaw -= 360 * (lChecker.Yaw > 0 ? 1 : -1);
                    }

                    lChecker.Yaw *= AccelerationCoefficient;

                    if (lNewSpeed.Yaw > lChecker.Yaw)
                    {
                        lNewSpeed.Yaw = (abs(lChecker.Yaw) + MinStep) * (lNewSpeed.Yaw > 0 ? 1 : -1);
                    }
                }
            }

            if (!CheckRotationLock(ERotationVariations::Roll))
            {
                // Корректировка направления
                if (int(lNewSpeed.Roll / 180))
                {
                    lNewSpeed.Roll -= 360 * (lNewSpeed.Roll > 0 ? 1 : -1);
                }

                // Приближение к Цели
                if (abs(lChecker.Roll) > MaxSpeed)
                {
                    // Ограничение скорости с сохранением знака
                    lNewSpeed.Roll = MaxSpeed * (lNewSpeed.Roll > 0 ? 1 : -1);
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
                    if (int(lChecker.Roll / 180))
                    {
                        lChecker.Roll -= 360 * (lChecker.Roll > 0 ? 1 : -1);
                    }

                    lChecker.Roll *= AccelerationCoefficient;

                    if (lNewSpeed.Roll > lChecker.Roll)
                    {
                        lNewSpeed.Roll = (abs(lChecker.Roll) + MinStep) * (lNewSpeed.Roll > 0 ? 1 : -1);
                    }
                }
            }

            lNewRotation = lNewSpeed * DeltaTime;

            if (bUseRelativeRotation)
            {
                CurrentActor->GetRootComponent()->AddRelativeRotation(lNewRotation);
            }
            else
            {
                CurrentActor->AddActorLocalRotation(lNewRotation);
            }
        }
    }
}
//--------------------------------------------------------------------------------------
