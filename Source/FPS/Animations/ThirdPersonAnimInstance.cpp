//

// Base:
#include "ThirdPersonAnimInstance.h"

// UE:
#include "Kismet/GameplayStatics.h"

// Interaction:
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Combat/WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UThirdPersonAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    SoundsInit();
}

void UThirdPersonAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
}
//--------------------------------------------------------------------------------------



/* ---   Customized Base   --- */

void UThirdPersonAnimInstance::CustomUpdateAnimation()
{
    Super::CustomUpdateAnimation();

    CurrentMovementDirection = CalculateDirection(PlayerOwner->GetVelocity(), PlayerOwner->GetActorRotation());

    // Задействуем готовую в APawn реплицируемую переменную Вертикального Взора.
    // Дабы избежать "разрыва" при плавной анимации между значениями 254 и 0, сделаем преобразование в знаковый тип данных "int8"
    if (PlayerOwner)
        CurrentVerticalViewingAngle = (int8)PlayerOwner->RemoteViewPitch;
    // @warning:
    // RemoteViewPitch изменяется в следующем диапазоне (вниз-вверх)
    // 191-254 и 0-63 (byte)
    // 
    // @note 0-255 -- полный оборот со всеми значениями byte
}
//--------------------------------------------------------------------------------------



/* ---   Sounds   --- */

void UThirdPersonAnimInstance::PlaySurfaceSound(const TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*>& iSoundsMap)
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

TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> UThirdPersonAnimInstance::GetMapForSound(const UDataTable* SoundTable)
{
    if (SoundTable)
    {
        TArray<FSoundBySurfaceType*> lAllRows;

        SoundTable->GetAllRows<FSoundBySurfaceType>("UThirdPersonAnimInstance::GetMapForSound", lAllRows);

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

void UThirdPersonAnimInstance::SoundsInit()
{
    MapOfSurfaceSoundsWhenStep = GetMapForSound(SurfaceSoundsWhenStep);

    MapOfSurfaceSoundsWhenLanding = GetMapForSound(SurfaceSoundsWhenLanding);
}
//--------------------------------------------------------------------------------------
