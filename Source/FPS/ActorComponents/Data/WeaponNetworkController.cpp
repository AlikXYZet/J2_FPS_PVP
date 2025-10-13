//

// Base:
#include "WeaponNetworkController.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Net:
#include "Net/UnrealNetwork.h"

// GAS:
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

// Interaction:
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Combat/Projectile.h"
#include "FPS/Combat/WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** DELEGATE: Метод вызова делегата с Ретрансляцией по Сети */
#define DELEGATE_METHOD_Broadcast_cpp(PropertyName) \
void UWeaponNetworkController::Server_##PropertyName##_Implementation() \
{ \
    Multicast_##PropertyName(); \
} \
void UWeaponNetworkController::Multicast_##PropertyName##_Implementation() \
{ \
    ##PropertyName.Broadcast(*CurrentWeaponData); \
}
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Замена конструктора с требуемым параметром для переменной 'UWeaponNetworkController::SpawnParameters'
static FActorSpawnParameters GetSpawnParameters()
{
    FActorSpawnParameters lResult;
    lResult.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    return lResult;

    // PS: У 'FActorSpawnParameters' нет перегруженных конструкторов, поэтому используется данный метод
};

// Параметр создания выбрасываемых Акторов посредством метода DropActor()
const FActorSpawnParameters UWeaponNetworkController::SpawnParameters = GetSpawnParameters();

const TArray<TEnumAsByte<EObjectTypeQuery>> UWeaponNetworkController::ObjectTypesForHitscan
{
    // Соответствие проверено опытным путём:
    EObjectTypeQuery::ObjectTypeQuery3, // [2]: for ECollisionChannel::ECC_Pawn, [2]
    EObjectTypeQuery::ObjectTypeQuery5, // [4]: for ECollisionChannel::ECC_Vehicle, [6]
    EObjectTypeQuery::ObjectTypeQuery6, // [5]: for ECollisionChannel::ECC_Destructible, [7]

    // Следующий метод инициализации данных вызывает блокировку запуска любого билда без какого-либо уведомления:
    //UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn),
    //UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Vehicle),
    //UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Destructible)
};
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UWeaponNetworkController::UWeaponNetworkController()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно

    // If true, we call the virtual InitializeComponent()
    bWantsInitializeComponent = true;

    // Компонент реплицируем по умолчанию
    SetIsReplicatedByDefault(true);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

//DELEGATE_METHOD_Broadcast_cpp(OnShootingWeapon); // Заменяется на RPC 'DropProjectile' с вызовом '.Broadcast()'
//DELEGATE_METHOD_Broadcast_cpp(OnReloadingWeapon); // Заменяется на RPC 'DropSleeve' с вызовом '.Broadcast()'
DELEGATE_METHOD_Broadcast_cpp(OnStartChangingWeapon);
//DELEGATE_METHOD_Broadcast_cpp(OnChangingWeapon); // Заменяется на RPC 'SetCurrentWeaponDataByNum' с вызовом '.Broadcast()'
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UWeaponNetworkController::OnComponentCreated()
{
    Super::OnComponentCreated();

    BaseInit();
}

void UWeaponNetworkController::InitializeComponent()
{
    Super::InitializeComponent();
}

void UWeaponNetworkController::BeginPlay()
{
    Super::BeginPlay();
}

void UWeaponNetworkController::BaseInit()
{
    PlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (!PlayerOwner)
    {
        FPS_LOG(Error, TEXT("PlayerOwner is NOT"));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void UWeaponNetworkController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UWeaponNetworkController, CurrentActions, COND_SkipOwner);
}

void UWeaponNetworkController::InitializeForFirstPersonDisplay()
{
    if (CurrentWeaponFrame)
    {
        // Отключить визуальное отображение
        CurrentWeaponFrame->GetRootComponent()->SetVisibility(false, true);

        // Получить все UPrimitiveComponent
        TInlineComponentArray<UPrimitiveComponent*> PrimComponents;
        CurrentWeaponFrame->GetComponents(PrimComponents);

        // Включить тень во всех UPrimitiveComponent
        for (UPrimitiveComponent* Comp : PrimComponents)
        {
            Comp->SetCastHiddenShadow(true);
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

const FWeaponData& UWeaponNetworkController::BP_GetCurrentWeaponData() const
{
    return *GetCurrentWeaponData();
}

void UWeaponNetworkController::InitData()
{
    if (CurrentWeaponData)
    {
        CurrentWeaponFrame = Cast<AWeaponFrame>(GetChildActor());

        if (CurrentWeaponFrame)
        {
            CurrentWeaponFrame->UpdateWeaponOnSelectedData(CurrentWeaponData);

            /* Исправление Привязки дочернего актора
            @note   На стороне клиента слетает привязанность Дочернего Актора не смотря на то,
                    что данный компонент уже привязан к нужному сокету. Поэтому исправляем данным кодом */
            if (PlayerOwner
                && CurrentWeaponFrame->GetAttachParentSocketName() != WeaponSocketInMesh)
            {
                CurrentWeaponFrame->AttachToComponent(
                    PlayerOwner->GetMesh(),
                    FAttachmentTransformRules::KeepWorldTransform,
                    WeaponSocketInMesh);
            }
        }
        else
        {
            FPS_LOG(Error, TEXT("'%s' is NOT AWeaponFrame (Check CurrentWeaponFrame)"),
                GetChildActor()
                ? *GetChildActor()->GetFName().ToString()
                : *FString("None"));
        }
    }
    else
    {
        FPS_LOG(Error, TEXT("CurrentWeaponData is NOT"));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Data   --- */

void UWeaponNetworkController::Server_SetCurrentActions_Implementation(const uint8& Value)
{
    CurrentActions = Value;
}

bool UWeaponNetworkController::CheckActions(const EActionVariations& Action, ...) const
{
    uint8 bResult = 0;
    const EActionVariations* p = &Action;

    while (p)
    {
        bResult |= uint8(*p);
        ++p;
    }

    return bResult && CheckActions(bResult);
}

AActor* UWeaponNetworkController::DropActor(const TSubclassOf<AActor>& iActorType, const FVector& iLocation, const FRotator& iRotation)
{
    if (iActorType.Get())
    {
        return GetWorld()->SpawnActor(
            iActorType.Get(),
            &iLocation,
            &iRotation,
            SpawnParameters);
    }

    return nullptr;
}

template<class T>
T* UWeaponNetworkController::DropActor(const TSubclassOf<T>& iActorType, const FVector& iLocation, const FRotator& iRotation)
{
    if (iActorType.Get())
    {
        return GetWorld()->SpawnActor<T>(
            iActorType.Get(),
            iLocation,
            iRotation,
            SpawnParameters);
    }

    return nullptr;
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Switching   --- */

void UWeaponNetworkController::SetCurrentWeaponDataByNum(const uint8& iNum)
{
    if (WeaponDataSlots.IsValidIndex(iNum))
    {
        Server_SetCurrentWeaponDataByNum(iNum);
    }
}

void UWeaponNetworkController::Server_SetCurrentWeaponDataByNum_Implementation(const uint8& iNum)
{
    Multicast_SetCurrentWeaponDataByNum(iNum);
}

void UWeaponNetworkController::Multicast_SetCurrentWeaponDataByNum_Implementation(const uint8& iNum)
{
    CurrentWeaponData = WeaponDataSlots[iNum];
    CurrentWeaponFrame->UpdateWeaponOnSelectedData(CurrentWeaponData);
    OnChangingWeapon.Broadcast(*CurrentWeaponData);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Shooting   --- */

void UWeaponNetworkController::DropProjectile(const FVector& Location, const FRotator& Rotation)
{
    Server_DropProjectile(Location, Rotation);
}

void UWeaponNetworkController::Server_DropProjectile_Implementation(const FVector& Location, const FRotator& Rotation)
{
    Multicast_DropProjectile(Location, Rotation);
}

void UWeaponNetworkController::Multicast_DropProjectile_Implementation(const FVector& Location, const FRotator& Rotation)
{
    AProjectile* lProjectile = DropActor<AProjectile>(CurrentWeaponData->ProjectileType.Get(), Location, Rotation);

    if (lProjectile)
    {
        if (PlayerOwner->HasAuthority())
        {
            if (GetCurrentWeaponData()->DamageEffect)
            {
                lProjectile->OnActorHit.AddDynamic(this, &UWeaponNetworkController::OnProjectileHitForServer);
            }
            else
            {
                FPS_LOG(Error, "'Damage Effect' is NOT. See 'Current Weapon Data' and 'Weapon Data Table'");
            }
        }
        else
        {
            lProjectile->OnActorHit.AddDynamic(this, &UWeaponNetworkController::OnProjectileHit);
        }
    }

    OnShootingWeapon.Broadcast(*CurrentWeaponData);
}


void UWeaponNetworkController::TraceProjectile(const FVector& StartLocation, const FVector& EndLocation)
{
    Server_TraceProjectile(StartLocation, EndLocation);
}

void UWeaponNetworkController::Server_TraceProjectile_Implementation(const FVector& StartLocation, const FVector& EndLocation)
{
    if (GetCurrentWeaponData()->DamageEffect)
    {
        TArray<FHitResult> lHitResult;

        UKismetSystemLibrary::LineTraceMultiForObjects(
            GetWorld(),
            StartLocation,
            EndLocation,
            ObjectTypesForHitscan,
            false,
            TArray<AActor*>{PlayerOwner},
            EDrawDebugTrace::None,
            lHitResult,
            true);

        for (FHitResult& lHit : lHitResult)
        {
            ProjectileDamage(lHit);
        }
    }
    else
    {
        FPS_LOG(Error, "'Damage Effect' is NOT. See 'Current Weapon Data' and 'Weapon Data Table'");
    }

    FRotator lRotator = UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation);

    Multicast_TraceProjectile(StartLocation, lRotator);
}

void UWeaponNetworkController::Multicast_TraceProjectile_Implementation(const FVector& Location, const FRotator& Rotation)
{
    if (GetCurrentWeaponData()->FXTracer)
    {
        DropActor(GetCurrentWeaponData()->FXTracer.Get(), Location, Rotation);
    }

    OnShootingWeapon.Broadcast(*CurrentWeaponData);
}


void UWeaponNetworkController::OnProjectileHitForServer(
    AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    ProjectileDamage(Hit);

    OnProjectileHit(SelfActor, OtherActor, NormalImpulse, Hit);
}

void UWeaponNetworkController::OnProjectileHit(
    AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    if (SelfActor)
    {
        SelfActor->Destroy();
    }
}

void UWeaponNetworkController::ProjectileDamage(const FHitResult& Hit)
{
    if (Hit.Actor.IsValid())
    {
        IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(Hit.Actor);
        if (TargetInterface)
        {
            PlayerOwner->AbilitySystemComp->ApplyGameplayEffectToTarget(
                GetCurrentWeaponData()->DamageEffect.GetDefaultObject(),
                TargetInterface->GetAbilitySystemComponent(),
                0,
                FGameplayEffectContextHandle());
        }
    }
}


void UWeaponNetworkController::DropSleeve(const FVector& Location, const FRotator& Rotation)
{
    if (CurrentWeaponData->SleeveType.Get())
    {
        Server_DropSleeve(Location, Rotation);
    }
}

void UWeaponNetworkController::Server_DropSleeve_Implementation(const FVector& Location, const FRotator& Rotation)
{
    Multicast_DropSleeve(Location, Rotation);
}

void UWeaponNetworkController::Multicast_DropSleeve_Implementation(const FVector& Location, const FRotator& Rotation)
{
    DropActor(CurrentWeaponData->SleeveType.Get(), Location, Rotation);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Reloading   --- */

void UWeaponNetworkController::DropStorage(const FVector& Location, const FRotator& Rotation)
{
    Server_DropStorage(Location, Rotation);
}

void UWeaponNetworkController::Server_DropStorage_Implementation(const FVector& Location, const FRotator& Rotation)
{
    Multicast_DropStorage(Location, Rotation);
}

void UWeaponNetworkController::Multicast_DropStorage_Implementation(const FVector& Location, const FRotator& Rotation)
{
    DropActor(CurrentWeaponData->StorageType.Get(), Location, Rotation);
    OnReloadingWeapon.Broadcast();
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Base: Debugs   --- */

void UWeaponNetworkController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property
        && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponNetworkController, WeaponFrameType))
    {
        if (WeaponFrameType)
        {
            SetChildActorClass(WeaponFrameType);
        }
        else
        {
            FPS_LOG(Error, TEXT("WeaponFrameType is NOT"));
        }
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UWeaponNetworkController::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

TArray<FName> UWeaponNetworkController::GetSocketNamesInMesh() const
{
    APlayerCharacter* lPlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (lPlayerOwner && lPlayerOwner->GetMesh())
    {
        return lPlayerOwner->GetMesh()->GetAllSocketNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
