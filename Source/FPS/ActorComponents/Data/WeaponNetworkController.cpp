//

// Base:
#include "WeaponNetworkController.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

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

/** DELEGATE: Метод вызова делегата с Ретрансляцией на Сервер */
#define DELEGATE_METHOD_ServerBroadcast_cpp(PropertyName) \
void UWeaponNetworkController::Server_##PropertyName##_Implementation() \
{ \
    Multicast_##PropertyName(); \
}

/** DELEGATE: Метод вызова делегата с Ретрансляцией для Всех */
#define DELEGATE_METHOD_MulticastBroadcast_cpp(PropertyName) \
void UWeaponNetworkController::Multicast_##PropertyName##_Implementation() \
{ \
    ##PropertyName.Broadcast(*CurrentWeaponData); \
}

/** DELEGATE: Метод вызова делегата с Ретрансляцией по Сети */
#define DELEGATE_METHOD_Broadcast_cpp(PropertyName) \
    DELEGATE_METHOD_ServerBroadcast_cpp(PropertyName)\
    DELEGATE_METHOD_MulticastBroadcast_cpp(PropertyName)
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

// Массив Типов Объектов, что отслеживаются Hitscan-методом
const TArray<TEnumAsByte<EObjectTypeQuery>> UWeaponNetworkController::ObjectTypesForHitscan
{
    // Соответствие проверено опытным путём:
    EOT_Pawn,
    EOT_Vehicle,
    EOT_Destructible,

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

//DELEGATE_METHOD_Broadcast_cpp(OnShootingWeapon);      // Заменяется на RPC 'DropProjectile' с вызовом '.Broadcast()'
//DELEGATE_METHOD_Broadcast_cpp(OnReloadingWeapon);     // Заменяется на RPC 'DropSleeve' с вызовом '.Broadcast()'
//DELEGATE_METHOD_Broadcast_cpp(OnStartChangingWeapon); // Заменяется на 'DELEGATE_METHOD_ServerBroadcast_cpp'
//DELEGATE_METHOD_Broadcast_cpp(OnChangingWeapon);      // Заменяется на RPC 'SetCurrentWeaponDataByNum' с вызовом '.Broadcast()'

DELEGATE_METHOD_ServerBroadcast_cpp(OnStartChangingWeapon)
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UWeaponNetworkController::OnComponentCreated()
{
    Super::OnComponentCreated();

    if (!GetOwner<APlayerCharacter>())
    {
        FPS_Error_Component("`GetPlayerOwner()` is NOT 'APlayerCharacter'");
    }
}

void UWeaponNetworkController::CreateChildActor()
{
    Super::CreateChildActor();

    ChildWeaponFrame = Cast<AWeaponFrame>(GetChildActor());

    if (ChildWeaponFrame)
    {
        if (GetCurrentWeaponData())
        {
            ChildWeaponFrame->UpdateWeaponOnSelectedData(GetCurrentWeaponData());
        }
    }
    else if (GetPlayerOwner()->HasAuthority())
    {
        FPS_Error_Component("'%s' is NOT 'AWeaponFrame' (Check `Child Actor`)",
            GetChildActor()
            ? *GetChildActor()->GetName()
            : *FString("None"));
    }
}

void UWeaponNetworkController::InitializeComponent()
{
    Super::InitializeComponent();
}

void UWeaponNetworkController::BeginPlay()
{
    Super::BeginPlay();
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void UWeaponNetworkController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UWeaponNetworkController, CurrentActions, COND_SkipOwner);
    DOREPLIFETIME(UWeaponNetworkController, ChildWeaponFrame);
}

void UWeaponNetworkController::InitializeForFirstPersonDisplay()
{
    if (GetCurrentWeaponFrame())
    {
        // Отключить визуальное отображение
        GetCurrentWeaponFrame()->GetRootComponent()->SetVisibility(false, true);

        // Получить все UPrimitiveComponent
        TInlineComponentArray<UPrimitiveComponent*> PrimComponents;
        GetCurrentWeaponFrame()->GetComponents(PrimComponents);

        // Включить тень во всех UPrimitiveComponent
        for (UPrimitiveComponent* Comp : PrimComponents)
        {
            Comp->SetCastHiddenShadow(true);
        }
    }
    else
    {
        FPS_Error_Component("'GetCurrentWeaponFrame()' is NOT");
    }
}

void UWeaponNetworkController::OnRep_ChildWeaponFrame()
{
    if (ChildWeaponFrame && GetCurrentWeaponData())
    {
        ChildWeaponFrame->UpdateWeaponOnSelectedData(GetCurrentWeaponData());

        if (GetPlayerOwner()->IsLocallyControlled())
        {
            InitializeForFirstPersonDisplay();
        }
    }
    else if (!ChildWeaponFrame)
    {
        FPS_Error_Component("'ChildWeaponFrame' is NOT");
    }
    else if (!GetCurrentWeaponData())
    {
        FPS_Error_Component("'GetCurrentWeaponData()' is NOT");
    }
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

void UWeaponNetworkController::InitData()
{
    if (WeaponDataSlots.IsValidIndex(0)
        && (CurrentWeaponData = WeaponDataSlots[0]))
    {
        if (ChildWeaponFrame)
        {
            ChildWeaponFrame->UpdateWeaponOnSelectedData(GetCurrentWeaponData());
        }
    }
    else
    {
        FPS_Error_Component("CurrentWeaponData is NOT");
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Data   --- */

void UWeaponNetworkController::Server_SetCurrentActions_Implementation(uint8 Value)
{
    CurrentActions = Value;
}

bool UWeaponNetworkController::CheckActions(EActionVariations Action, ...) const
{
    uint8 bResult = 0;
    const EActionVariations* p = &Action;

    while (p)
    {
        // Включение всех проверяемых бит
        bResult |= uint8(*p);
        ++p;
    }

    return CheckActions(bResult);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Switching   --- */

void UWeaponNetworkController::Multicast_OnStartChangingWeapon_Implementation()
{
    PlaySound(GetCurrentWeaponData()->RemoveSound, GetCurrentWeaponFrame()->GetActorLocation());
    OnStartChangingWeapon.Broadcast(*CurrentWeaponData);
}


void UWeaponNetworkController::SetCurrentWeaponDataByNum(uint8 iNum)
{
    if (WeaponDataSlots.IsValidIndex(iNum))
    {
        Server_SetCurrentWeaponDataByNum(iNum);
    }
}

void UWeaponNetworkController::Server_SetCurrentWeaponDataByNum_Implementation(uint8 iNum)
{
    Multicast_SetCurrentWeaponDataByNum(iNum);
}

void UWeaponNetworkController::Multicast_SetCurrentWeaponDataByNum_Implementation(uint8 iNum)
{
    CurrentWeaponData = WeaponDataSlots[iNum];
    if (GetChildActor())
    {
        GetCurrentWeaponFrame()->UpdateWeaponOnSelectedData(GetCurrentWeaponData());
    }
    PlaySound(GetCurrentWeaponData()->TakeSound, GetCurrentWeaponFrame()->GetActorLocation());
    OnChangingWeapon.Broadcast(*GetCurrentWeaponData());
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
    AProjectile* lProjectile = DropActor<AProjectile>(GetCurrentWeaponData()->ProjectileType.Get(), Location, Rotation);

    if (lProjectile)
    {
        if (GetPlayerOwner()->HasAuthority())
        {
            if (GetCurrentWeaponData()->DamageEffects.Num())
            {
                lProjectile->OnActorHit.AddDynamic(this, &UWeaponNetworkController::OnProjectileHitForServer);
            }
            else
            {
                FPS_Error_Component("'Damage Effect' is NOT. See 'Current Weapon Data' and 'Weapon Data Table'");
            }
        }
        else
        {
            lProjectile->OnActorHit.AddDynamic(this, &UWeaponNetworkController::OnProjectileHit);
        }
    }

    PlaySound(GetCurrentWeaponData()->ShootingSound, Location);

    OnShootingWeapon.Broadcast(*GetCurrentWeaponData());
}


void UWeaponNetworkController::TraceProjectile(const FVector& StartLocation, const FVector& EndLocation)
{
    Server_TraceProjectile(StartLocation, EndLocation);
}

void UWeaponNetworkController::Server_TraceProjectile_Implementation(const FVector& StartLocation, const FVector& EndLocation)
{
    if (GetCurrentWeaponData()->DamageEffects.Num())
    {
        TArray<FHitResult> lHitResult;

        UKismetSystemLibrary::LineTraceMultiForObjects(
            GetWorld(),
            StartLocation,
            EndLocation,
            ObjectTypesForHitscan,
            false,
            TArray<AActor*>{GetOwner()},
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
        FPS_Error_Component("'Damage Effect' is NOT. See 'Current Weapon Data' and 'Weapon Data Table'");
    }

    FRotator lRotator = UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation);

    Multicast_TraceProjectile(StartLocation, EndLocation);
}

void UWeaponNetworkController::Multicast_TraceProjectile_Implementation(const FVector& StartLocation, const FVector& EndLocation)
{
    if (GetCurrentWeaponData()->FXTracer)
    {
        FRotator lRotator = UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation);
        DropActor(GetCurrentWeaponData()->FXTracer.Get(), StartLocation, lRotator);
    }

    PlaySound(GetCurrentWeaponData()->ShootingSound, StartLocation);
    PlaySound(GetCurrentWeaponData()->HitSound, EndLocation);

    OnShootingWeapon.Broadcast(*GetCurrentWeaponData());
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
    PlaySound(GetCurrentWeaponData()->HitSound, Hit.Location);

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
            for (auto& Effect : GetCurrentWeaponData()->DamageEffects)
            {
                GetPlayerOwner()->AbilitySystemComp->ApplyGameplayEffectToTarget(
                    Effect.GetDefaultObject(),
                    TargetInterface->GetAbilitySystemComponent());
            }
        }
    }
}


void UWeaponNetworkController::DropSleeve(const FVector& Location, const FRotator& Rotation)
{
    if (GetCurrentWeaponData()->SleeveType.Get())
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
    DropActor(GetCurrentWeaponData()->SleeveType.Get(), Location, Rotation);
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
    DropActor(GetCurrentWeaponData()->StorageType.Get(), Location, Rotation);
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
            FPS_Error_Component("WeaponFrameType is NOT");
        }
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}

//void UWeaponNetworkController::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
//{
//    Super::PostEditChangeChainProperty(PropertyChangedEvent);
//}
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
