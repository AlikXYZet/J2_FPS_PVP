//

// Base:
#include "WeaponNetworkController.h"

// Net:
#include "Net/UnrealNetwork.h"

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
    /* Фильтрация, если вызвал Владелец */ \
    if (PlayerOwner->IsLocallyControlled()) \
        return; \
    ##PropertyName.Broadcast(); \
}
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
    //CheckNumOfSlots();
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
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: PlayerOwner is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }

    if (WeaponFrameType)
    {
        SetChildActorClass(WeaponFrameType);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: WeaponFrameType is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void UWeaponNetworkController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UWeaponNetworkController, CurrentActions, COND_SkipOwner);
}

void UWeaponNetworkController::InitializeFirstPersonWeaponFrame()
{
    if (CurrentWeaponFrame)
    {
        CurrentWeaponFrame->WeaponSkeletalMesh->SetVisibility(false);
        CurrentWeaponFrame->WeaponSkeletalMesh->SetCastHiddenShadow(true);

        CurrentWeaponFrame->WeaponSkeletalMesh->SetVisibility(false);
        CurrentWeaponFrame->WeaponSkeletalMesh->SetCastHiddenShadow(true);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

const FWeaponData& UWeaponNetworkController::BP_GetCurrentWeaponData() const
{
    return *GetCurrentWeaponData();
}

void UWeaponNetworkController::DataInit()
{
    if (CurrentWeaponData)
    {
        CurrentWeaponFrame = Cast<AWeaponFrame>(GetChildActor());

        if (CurrentWeaponFrame)
        {
            CurrentWeaponFrame->AttachToComponent(
                PlayerOwner->GetMesh(),
                FAttachmentTransformRules::KeepRelativeTransform,
                WeaponSocketInMesh);

            CurrentWeaponFrame->UpdateWeaponOnSelectedData(CurrentWeaponData);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: Child Actor is NOT AWeaponFrame (Check CurrentWeaponFrame)"),
                *GetNameSafe(this), *FString(__func__));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: CurrentWeaponData is NOT"),
            *GetNameSafe(this), *FString(__func__));
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
        // Параметр создания: Всегда появляется
        FActorSpawnParameters lSpawnParameters;
        lSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* Test = GetWorld()->SpawnActor<AActor>(
            iActorType.Get(),
            iLocation,
            iRotation,
            lSpawnParameters);

        return Test;
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
    ChangingCurrentWeaponDataByNum(iNum);
}

void UWeaponNetworkController::ChangingCurrentWeaponDataByNum(const uint8& iNum)
{
    CurrentWeaponData = WeaponDataSlots[iNum];
    CurrentWeaponFrame->UpdateWeaponOnSelectedData(CurrentWeaponData);
    OnChangingWeapon.Broadcast();
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
    CreateProjectile(Location, Rotation);
}

void UWeaponNetworkController::CreateProjectile(const FVector& Location, const FRotator& Rotation)
{
    DropActor(CurrentWeaponData->ProjectileType.Get(), Location, Rotation);
    OnShootingWeapon.Broadcast();
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

void UWeaponNetworkController::Multicast_DropStorage_Implementation(const FVector& Location, const FRotator& Rotation)
{
    Multicast_DropStorage(Location, Rotation);
}

void UWeaponNetworkController::Server_DropStorage_Implementation(const FVector& Location, const FRotator& Rotation)
{
    DropActor(CurrentWeaponData->StorageType.Get(), Location, Rotation);
    OnReloadingWeapon.Broadcast();
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Net   --- */

TArray<FName> UWeaponNetworkController::GetBoneSocketsInMesh() const
{
    APlayerCharacter* lPlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (lPlayerOwner && lPlayerOwner->GetMesh())
    {
        return lPlayerOwner->GetMesh()->GetAllSocketNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------
#endif
//======================================================================================
