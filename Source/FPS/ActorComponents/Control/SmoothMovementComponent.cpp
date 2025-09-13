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
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void USmoothMovementComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USmoothMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    MovementForTick(DeltaTime);
}

void USmoothMovementComponent::OnComponentCreated()
{
    Super::OnComponentCreated();

    InitCurrentComponent();
}

void USmoothMovementComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}
//--------------------------------------------------------------------------------------



/* ---   Init   --- */

void USmoothMovementComponent::InitCurrentComponent()
{
    CurrentActor = GetOwner();

    if (!CurrentActor)
    {
        FPS_LOG_Component(Error, TEXT("CurrentActor is NOT"));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actor Movement   --- */

void USmoothMovementComponent::MoveToLocation(const FVector& iPoint)
{
    if (CurrentActor)
    {
        if (OnStartMove.IsBound())
        {
            OnStartMove.Broadcast();
        }

        if (bUseRelativeLocation)
        {
            StartLocation = CurrentActor->GetRootComponent()->GetRelativeLocation();
        }
        else
        {
            StartLocation = CurrentActor->GetActorLocation();
        }

        EndLocation = iPoint;
        bIsMovingToNewLocation = true;
        bApproachWork = false;
        bSeparationWork = false;
    }
}

void USmoothMovementComponent::MovementForTick(const float& DeltaTime)
{
    // Контроль перемещения
    if (bIsMovingToNewLocation)
    {
        FVector lCurrentLocation = bUseRelativeLocation ?
            CurrentActor->GetRootComponent()->GetRelativeLocation() : CurrentActor->GetActorLocation();

        // Контроль близости к новой локации
        if ((lCurrentLocation - EndLocation).Size() < MinStep)
        {
            if (bUseRelativeLocation)
            {
                CurrentActor->SetActorRelativeLocation(EndLocation);
            }
            else
            {
                CurrentActor->SetActorLocation(EndLocation);
            }

            bIsMovingToNewLocation = false;

            if (OnCompletedMove.IsBound())
            {
                OnCompletedMove.Broadcast();
            }
        }
        else
        {
            /* ---   Приближение к Цели   --- */

            // Шаг в векторном исполнении
            FVector lNewSpeed_Vector = (EndLocation - lCurrentLocation);

            if (!bApproachWork
                && lNewSpeed_Vector.Size() <= ApproachDistance)
            {
                if (OnApproach.IsBound())
                {
                    OnApproach.Broadcast();
                }

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
                if (OnSeparation.IsBound())
                {
                    OnSeparation.Broadcast();
                }

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
                CurrentActor->GetRootComponent()->AddRelativeLocation(lNewSpeed_Vector * DeltaTime);
            }
            else
            {
                CurrentActor->AddActorWorldOffset(lNewSpeed_Vector * DeltaTime);
            }
        }
    }
}
//--------------------------------------------------------------------------------------
