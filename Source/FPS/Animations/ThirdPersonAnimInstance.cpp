//

// Base:
#include "ThirdPersonAnimInstance.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Interaction:
#include "FPS/Characters/PlayerCharacter.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UThirdPersonAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    InitSounds();
}

void UThirdPersonAnimInstance::NativeUninitializeAnimation()
{
    Super::NativeUninitializeAnimation();
}

void UThirdPersonAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
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
            EDrawDebugTrace::None,
            lHitResult,
            true))
        {
            // Поиск Звука из TMap
            USoundBase* const* lpSound = iSoundsMap.Find(UGameplayStatics::GetSurfaceType(lHitResult));
            if (!lpSound)
            {
                // Повторный поиск, но со значением по умолчанию
                lpSound = iSoundsMap.Find(EPhysicalSurface::SurfaceType_Default);
            }

            if (lpSound)
            {
                USoundBase* lSound = *lpSound;
                if (lSound)
                {
                    // Воспроизвести звук
                    UGameplayStatics::PlaySoundAtLocation(GetWorld(), lSound, lHitResult.Location);
                }
            }
        }
    }
}

TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> UThirdPersonAnimInstance::GetMapForSound(const UDataTable* SoundTable)
{
    if (SoundTable)
    {
        TArray<FSoundBySurfacesType*> lAllRows;

        SoundTable->GetAllRows<FSoundBySurfacesType>(__FUNCTION__, lAllRows);

        if (lAllRows.Num())
        {
            TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> lResult;
            lResult.Reserve(lAllRows.Num());

            for (const FSoundBySurfacesType* lRow : lAllRows)
            {
                lResult.Add(EPhysicalSurface(lRow->SurfaceType), lRow->Sound);
            }

            return lResult;
        }
    }

    return TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*>();
}

void UThirdPersonAnimInstance::InitSounds()
{
    MapOfSurfaceSoundsWhenStep.Empty();
    MapOfSurfaceSoundsWhenStep = GetMapForSound(SurfaceSoundsWhenStep);
    MapOfSurfaceSoundsWhenLanding.Empty();
    MapOfSurfaceSoundsWhenLanding = GetMapForSound(SurfaceSoundsWhenLanding);
}
//--------------------------------------------------------------------------------------
