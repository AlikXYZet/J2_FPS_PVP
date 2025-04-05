//

// Base:
#include "PersonAnimInstance.h"

// UE:
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Interaction:
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/ActorComponents/FPS_CharacterMovementComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UPersonAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    SoundsInit();
}

void UPersonAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    BaseInit();
    MovementInit();
}
//--------------------------------------------------------------------------------------



/* ---   Customized Base   --- */

void UPersonAnimInstance::SetTimeForUpdateAnimation(const float iTime)
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
    // PlayerOwner

    PlayerOwner = Cast<APlayerCharacter>(TryGetPawnOwner());

    if (!PlayerOwner)
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::BaseInit: PlayerOwner is NOT"),
            *GetNameSafe(this));
    }

    // Timer_CustomUpdateAnimation
    GetWorld()->GetTimerManager().SetTimer(Timer_CustomUpdateAnimation, this, &UPersonAnimInstance::CustomUpdateAnimation, TimeForCustomUpdateAnimation, true);
}

void UPersonAnimInstance::CustomUpdateAnimation()
{
    CurrentMovementSpeed = PlayerOwner->GetVelocity().Size();
    CurrentMovementDirection = CalculateDirection(PlayerOwner->GetVelocity(), PlayerOwner->GetActorRotation());

    EventCustomUpdateAnimation();
}
//--------------------------------------------------------------------------------------



/* ---   Movement   --- */

void UPersonAnimInstance::MovementInit()
{
    if (PlayerOwner)
    {
        PlayerOwner->GetFPSCharacterMovement()->OnNewMovementMode.AddDynamic(this, &UPersonAnimInstance::UpdateMovementMode);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::MovementInit: PlayerOwner is NOT"),
            *GetNameSafe(this));
    }
}

void UPersonAnimInstance::UpdateMovementMode(EMovementMode iMovementMode, uint8 iCustomMovementMode)
{
    CurrentMovementMode = iMovementMode;

    EventUpdateMovementMode(iMovementMode, iCustomMovementMode);
}
//--------------------------------------------------------------------------------------



/* ---   Sounds   --- */

void UPersonAnimInstance::PlaySurfaceSound(const TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*>& iSoundsMap)
{
    if (PlayerOwner && iSoundsMap.Num())
    {
        FVector lLocation = PlayerOwner->GetActorLocation();
        FHitResult lHitResult;

        // Трассировка поверхности
        if (UKismetSystemLibrary::LineTraceSingle(
            GetWorld(),
            lLocation,
            lLocation - FVector(0.f, 0.f, 200.f),
            ETraceTypeQuery::TraceTypeQuery1,
            false,
            TArray<AActor*>{PlayerOwner},
            EDrawDebugTrace::Type::None,
            lHitResult,
            true))
        {
            // Поиск Звука из TMap
            USoundBase* lSound = *iSoundsMap.Find(UGameplayStatics::GetSurfaceType(lHitResult));

            if (!lSound)
            {
                // Поиск "резервного" Звука из TMap
                lSound = *iSoundsMap.Find(EPhysicalSurface::SurfaceType_Default);
            }

            if (lSound)
            {
                // Воспроизвести звук
                UGameplayStatics::PlaySoundAtLocation(GetWorld(), lSound, lHitResult.Location);
            }
        }
    }
}

TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> UPersonAnimInstance::GetMapForSound(const UDataTable* SoundTable)
{
    if (SoundTable)
    {
        TArray<FSoundBySurfaceType*> lAllRows;

        SoundTable->GetAllRows<FSoundBySurfaceType>("UPersonAnimInstance::GetMapForSound", lAllRows);

        if (lAllRows.Num())
        {
            TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> lResult;

            for (const FSoundBySurfaceType* lRow : lAllRows)
            {
                lResult.Add(EPhysicalSurface(lRow->SurfaceType), lRow->Sound);
            }

            return lResult;
        }
    }

    return TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*>();
}

void UPersonAnimInstance::SoundsInit()
{
    MapOfSurfaceSoundsWhenStep = GetMapForSound(SurfaceSoundsWhenStep);

    MapOfSurfaceSoundsWhenLanding = GetMapForSound(SurfaceSoundsWhenLanding);
}
//--------------------------------------------------------------------------------------
