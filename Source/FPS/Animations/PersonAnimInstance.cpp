//

// Base:
#include "PersonAnimInstance.h"

// UE:
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Interaction:
#include "FPS/ActorComponents/Data/WeaponControlComponent.h"
#include "FPS/Characters/PlayerCharacter.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UPersonAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    SoundsInit();
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

void UPersonAnimInstance::SetTimeForUpdateAnimation(const float& iTime)
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
    PlayerOwner = Cast<APlayerCharacter>(TryGetPawnOwner());

    if (PlayerOwner)
    {
        PlayerOwner->MovementModeChangedDelegate.AddDynamic(this, &UPersonAnimInstance::UpdateMovementMode);

        WeaponControlComponent = PlayerOwner->WeaponControlComp;

        if (WeaponControlComponent)
        {
            WeaponControlComponent->OnChangingWeapon.AddDynamic(this, &UPersonAnimInstance::UpdateBasicData);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: WeaponControlComponent is NOT"),
                *GetNameSafe(this), *FString(__func__));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: PlayerOwner is NOT"),
            *GetNameSafe(this), *FString(__func__));
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

bool UPersonAnimInstance::CheckAction(const EActionVariations& InAction) const
{
    return WeaponControlComponent
        && WeaponControlComponent->CheckAction(InAction);
}

void UPersonAnimInstance::UpdateBasicData()
{
    if (WeaponControlComponent)
    {
        PersonAnimations = WeaponControlComponent->GetCurrentWeaponData()->PersonAnimations;
    }
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
