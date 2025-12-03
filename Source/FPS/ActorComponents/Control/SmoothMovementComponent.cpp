//

// Base:
#include "SmoothMovementComponent.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/Actor.h"
//--------------------------------------------------------------------------------------


/* ---   Constructors   --- */

USmoothMovementComponent::USmoothMovementComponent()
{
    // Set this actor to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryComponentTick.bCanEverTick = true; // Warning: Принудительно
    //-------------------------------------------


    /* ---   Booleanas   --- */

    bControlSpeedAtStart = true;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

//void USmoothMovementComponent::BeginPlay()
//{
//    Super::BeginPlay();
//}

void USmoothMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    MovementForTick(DeltaTime);
}

//void USmoothMovementComponent::OnComponentCreated()
//{
//    Super::OnComponentCreated();
//}

//void USmoothMovementComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
//{
//    Super::OnComponentDestroyed(bDestroyingHierarchy);
//}
//--------------------------------------------------------------------------------------



/* ---   Actor Movement   --- */

void USmoothMovementComponent::MoveToLocation(const FVector& iPoint)
{
    OnStartMove.Broadcast();

    StartLocation = bUseRelativeLocation
        ? GetOwner()->GetRootComponent()->GetRelativeLocation() : GetOwner()->GetActorLocation();

    EndLocation = iPoint;
    bIsMovingToNewLocation = true;
    bApproachWork = false;
    bSeparationWork = false;
}

void USmoothMovementComponent::MovementForTick(float DeltaTime)
{
    // Контроль перемещения
    if (bIsMovingToNewLocation)
    {
        FVector lCurrentLocation = bUseRelativeLocation
            ? GetOwner()->GetRootComponent()->GetRelativeLocation() : GetOwner()->GetActorLocation();

        // Контроль близости к новой локации
        if ((lCurrentLocation - EndLocation).Size() < MinStep)
        {
            if (bUseRelativeLocation)
            {
                GetOwner()->SetActorRelativeLocation(EndLocation);
            }
            else
            {
                GetOwner()->SetActorLocation(EndLocation);
            }

            bIsMovingToNewLocation = false;

            OnCompletedMove.Broadcast();
        }
        else
        {
            /* ---   Приближение к Цели   --- */

            // Шаг в векторном исполнении
            FVector lNewSpeed_Vector = (EndLocation - lCurrentLocation);

            if (!bApproachWork
                && lNewSpeed_Vector.Size() <= ApproachDistance)
            {
                OnApproach.Broadcast();

                bApproachWork = true;
            }

            if (lNewSpeed_Vector.Size() * DecelerationCoefficient > MaxSpeed)
            {
                lNewSpeed_Vector = lNewSpeed_Vector.GetSafeNormal() * MaxSpeed;
            }
            else
            {
                lNewSpeed_Vector *= DecelerationCoefficient;
            }
            //-------------------------------------------


            /* ---   Отдаление от предыдущей Цели   --- */

            // Пройденная дистанция
            float lDistanceTraveled = (lCurrentLocation - StartLocation).Size();

            if (!bSeparationWork
                && lDistanceTraveled >= ApproachDistance)
            {
                OnSeparation.Broadcast();

                bSeparationWork = true;
            }

            if (bControlSpeedAtStart)
            {
                if (lNewSpeed_Vector.Size() > lDistanceTraveled * AccelerationCoefficient)
                {
                    lNewSpeed_Vector = lNewSpeed_Vector.GetSafeNormal() * (lDistanceTraveled * AccelerationCoefficient + MinStep);
                }
            }
            //-------------------------------------------

            // Сделать шаг (Плавное перемещение)
            if (bUseRelativeLocation)
            {
                GetOwner()->GetRootComponent()->AddRelativeLocation(lNewSpeed_Vector * DeltaTime);
            }
            else
            {
                GetOwner()->AddActorWorldOffset(lNewSpeed_Vector * DeltaTime);
            }
        }
    }
}
//--------------------------------------------------------------------------------------
