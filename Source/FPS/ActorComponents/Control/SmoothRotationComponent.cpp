//

// Base:
#include "SmoothRotationComponent.h"

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
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: CurrentActor is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actor Rotate   --- */

void USmoothRotationComponent::RotateToLocation(const FVector& iPoint)
{
    if (CurrentActor)
    {
        StartRotation = CurrentActor->GetActorRotation();
        EndRotation = UKismetMathLibrary::FindLookAtRotation(CurrentActor->GetActorLocation(), iPoint);
        bIsRotatedToNewRotation = true;
    }
}

void USmoothRotationComponent::RotationForTick(const float& DeltaTime)
{
    // Контроль вращения
    if (bIsRotatedToNewRotation)
    {
        FRotator lNewRotation = CurrentActor->GetActorRotation();
        FRotator lCurrentRotation = lNewRotation;

        // Контроль близости к новой ротации
        if ((lCurrentRotation - EndRotation).IsNearlyZero(MinStep))
        {
            CurrentActor->SetActorRotation(EndRotation);
            bIsRotatedToNewRotation = false;

            if (OnCompletedRotate.IsBound())
            {
                OnCompletedRotate.Broadcast();
            }
        }
        else
        {
            // Рассчитанная угловая Скорость Вращения
            FRotator lNewSpeed = EndRotation - lCurrentRotation;


            /* ---   Корректировка направления   --- */
            {
                if (int(lNewSpeed.Pitch / 180))
                {
                    lNewSpeed.Pitch -= 360 * (lNewSpeed.Pitch > 0 ? 1 : -1);
                }

                if (int(lNewSpeed.Yaw / 180))
                {
                    lNewSpeed.Yaw -= 360 * (lNewSpeed.Yaw > 0 ? 1 : -1);
                }

                if (int(lNewSpeed.Roll / 180))
                {
                    lNewSpeed.Roll -= 360 * (lNewSpeed.Roll > 0 ? 1 : -1);
                }
            }


            /* ---   Приближение к Цели   --- */

            FRotator lChecker = lNewSpeed * DecelerationCoefficient;

            if (abs(lChecker.Pitch) > MaxSpeed)
            {
                // Ограничение скорости с сохранением знака
                lNewSpeed.Pitch = MaxSpeed * (lNewSpeed.Pitch > 0 ? 1 : -1);
            }
            else
            {
                lNewSpeed.Pitch *= DecelerationCoefficient;
            }

            if (abs(lChecker.Yaw) > MaxSpeed)
            {
                // Ограничение скорости с сохранением знака
                lNewSpeed.Yaw = MaxSpeed * (lNewSpeed.Yaw > 0 ? 1 : -1);
            }
            else
            {
                lNewSpeed.Yaw *= DecelerationCoefficient;
            }

            if (abs(lChecker.Roll) > MaxSpeed)
            {
                // Ограничение скорости с сохранением знака
                lNewSpeed.Roll = MaxSpeed * (lNewSpeed.Roll > 0 ? 1 : -1);
            }
            else
            {
                lNewSpeed.Roll *= DecelerationCoefficient;
            }
            //-------------------------------------------


            /* ---   Отдаление от предыдущей Цели   --- */

            if (bControlSpeedAtStart)
            {
                // Пройденный поворот
                lChecker = lCurrentRotation - StartRotation;

                // Корректировка направления
                if (int(lChecker.Pitch / 180))
                {
                    lChecker.Pitch -= 360 * (lChecker.Pitch > 0 ? 1 : -1);
                }

                if (int(lChecker.Yaw / 180))
                {
                    lChecker.Yaw -= 360 * (lChecker.Yaw > 0 ? 1 : -1);
                }

                if (int(lChecker.Roll / 180))
                {
                    lChecker.Roll -= 360 * (lChecker.Roll > 0 ? 1 : -1);
                }

                lChecker *= AccelerationCoefficient;

                if (lNewSpeed.Pitch > lChecker.Pitch)
                {
                    lNewSpeed.Pitch = (abs(lChecker.Pitch) + MinStep) * (lNewSpeed.Pitch > 0 ? 1 : -1);
                }

                if (lNewSpeed.Yaw > lChecker.Yaw)
                {
                    lNewSpeed.Yaw = (abs(lChecker.Yaw) + MinStep) * (lNewSpeed.Yaw > 0 ? 1 : -1);
                }

                if (lNewSpeed.Roll > lChecker.Roll)
                {
                    lNewSpeed.Roll = (abs(lChecker.Roll) + MinStep) * (lNewSpeed.Roll > 0 ? 1 : -1);
                }
            }
            //-------------------------------------------

            //UE_LOG(LogTemp, Error, TEXT("'%s'::%s: lCurrentRotation == %s"),
            //    *GetNameSafe(this), *FString(__func__), *lCurrentRotation.ToString());

            //UE_LOG(LogTemp, Error, TEXT("'%s'::%s: lNewSpeed == %s"),
            //    *GetNameSafe(this), *FString(__func__), *lNewSpeed.ToString());

            lNewRotation = lNewSpeed * DeltaTime;
            CurrentActor->AddActorLocalRotation(lNewRotation);
        }
    }
}
//--------------------------------------------------------------------------------------
