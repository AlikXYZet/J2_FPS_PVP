//

// Base:
#include "WeaponControlComponent.h"

// UE:
#include "Components/ArrowComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/InputSettings.h"

// Net:
#include "Net/UnrealNetwork.h"

// Interaction:
#include "FPS/ActorComponents/Control/FPS_CharacterMovementComponent.h"
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Combat/Projectile.h"
#include "FPS/Combat/WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UWeaponControlComponent::UWeaponControlComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно

    // Компонент реплицируем по умолчанию
    SetIsReplicatedByDefault(true);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UWeaponControlComponent::BeginPlay()
{
    Super::BeginPlay();

    BaseInit();
    DataInit();
    InitSpeedControl();
}

void UWeaponControlComponent::OnComponentCreated()
{
    Super::OnComponentCreated();

    CheckNumOfSlots();
}

void UWeaponControlComponent::BaseInit()
{
    PlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (PlayerOwner)
    {
        // Компонент движения Персонажа
        PlayerOwner->GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: PlayerOwner is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void UWeaponControlComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(UWeaponControlComponent, CurrentActions, COND_SkipOwner);
}
//--------------------------------------------------------------------------------------



/* ---   Inputs   --- */

void UWeaponControlComponent::SetupPlayerInputs()
{
    UInputComponent* lInputComponent = Cast<APawn>(GetOwner())->InputComponent;

    if (lInputComponent)
    {
        /* ---   Switching   --- */

        if (ActionGroups_NextSlot != NAME_None)
            lInputComponent->BindAction(ActionGroups_NextSlot, IE_Pressed, this, &UWeaponControlComponent::ToNextSlot);

        if (ActionGroups_PrevSlot != NAME_None)
            lInputComponent->BindAction(ActionGroups_PrevSlot, IE_Pressed, this, &UWeaponControlComponent::ToPrevSlot);

        if (ActionGroups_SlotNum1 != NAME_None)
            lInputComponent->BindAction(ActionGroups_SlotNum1, IE_Pressed, this, &UWeaponControlComponent::ToSlot1);

        if (ActionGroups_SlotNum2 != NAME_None)
            lInputComponent->BindAction(ActionGroups_SlotNum2, IE_Pressed, this, &UWeaponControlComponent::ToSlot2);
        //-------------------------------------------


        /* ---   Actions   --- */

        if (ActionGroups_Aiming != NAME_None)
        {
            lInputComponent->BindAction(ActionGroups_Aiming, IE_Pressed, this, &UWeaponControlComponent::SetAiming);
            lInputComponent->BindAction(ActionGroups_Aiming, IE_Released, this, &UWeaponControlComponent::ResetAiming);
        }

        if (ActionGroups_Fire != NAME_None)
        {
            lInputComponent->BindAction(ActionGroups_Fire, IE_Pressed, this, &UWeaponControlComponent::SetShooting);
            lInputComponent->BindAction(ActionGroups_Fire, IE_Released, this, &UWeaponControlComponent::ResetShooting);
        }

        if (ActionGroups_Reload != NAME_None)
            lInputComponent->BindAction(ActionGroups_Reload, IE_Pressed, this, &UWeaponControlComponent::SetReloading);
        //-------------------------------------------
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: lInputComponent is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }


    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Inputs   --- */

    CheckInputsGroups();
    //-------------------------------------------
#endif
    //===========================================
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

const FWeaponData& UWeaponControlComponent::GetCurrentWeaponData() const
{
    return *CurrentWeaponData;
}

const FWeaponSlotData& UWeaponControlComponent::GetCurrentSlotData() const
{
    return *CurrentSlot;
}

void UWeaponControlComponent::DataInit()
{
    if (WeaponsDataTable
        && WeaponSlots.IsValidIndex(0))
    {
        CurrentSlot = &WeaponSlots[0];
        CurrentWeaponData = WeaponsDataTable->FindRow<FWeaponData>(CurrentSlot->WeaponType, "DataInit");

        if (CurrentWeaponData)
        {
            OnChangingWeapon.Broadcast();
            CurrentWeaponFrame = Cast<AWeaponFrame>(GetChildActor());

            if (CurrentWeaponFrame)
            {
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
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: lRow is NOT"),
                *GetNameSafe(this), *FString(__func__));
        }
    }
    else if (!WeaponsDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: WeaponsDataTable is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
    else if (!WeaponSlots.IsValidIndex(0))
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: WeaponSlots[0] is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Switching   --- */

void UWeaponControlComponent::SetCurrentSlotByNum(const uint8& iNum)
{
    if (WeaponSlots.IsValidIndex(iNum))
        SetChanging(&WeaponSlots[iNum]);
}

void UWeaponControlComponent::ToSlot1()
{
    SetCurrentSlotByNum(1);
}

void UWeaponControlComponent::ToSlot2()
{
    SetCurrentSlotByNum(2);
}

void UWeaponControlComponent::ToNextSlot()
{
    SetChanging(
        CurrentSlot == &WeaponSlots.Last(0)
        ? &WeaponSlots[0]
        : CurrentSlot + 1);
}

void UWeaponControlComponent::ToPrevSlot()
{
    SetChanging(
        CurrentSlot == &WeaponSlots[0]
        ? &WeaponSlots.Last(0)
        : CurrentSlot - 1);
}

void UWeaponControlComponent::CheckNumOfSlots()
{
    // Проверка количества слотов
    if (WeaponSlots.Num())
    {
        CurrentSlot = &WeaponSlots[0];
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("'%s'::%s: WeaponSlots.Num() == 0"),
            *GetNameSafe(this), *FString(__func__));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Data   --- */

void UWeaponControlComponent::UpdateCurrentActions()
{
    uint8 OldActions = CurrentActions;

    CurrentActions =

        // 0b 1000 0000
        SettingActions & (uint8)EActionVariations::Block
        ? (uint8)EActionVariations::Block

        // 0b 0000 100|
        : SettingActions & (uint8)EActionVariations::Changing
        ? SettingActions & uint8(EActionVariations::Aiming | EActionVariations::Changing)

        // 0b 0000 010|
        : SettingActions & (uint8)EActionVariations::Reloading
        ? SettingActions & uint8(EActionVariations::Aiming | EActionVariations::Reloading)

        // 0b 0000 00~~
        : SettingActions;

    // PS: CurrentActions ограничен следующими значениями по порядку приоритета от наибольшего к меньшему:
    // 0b 1000 0000 - Блокировка Действий
    // 0b 0000 100| - Действие "Смена" с отслеживанием "Прицеливания"
    // 0b 0000 010| - Действие "Перезарядка" с отслеживанием "Прицеливания"
    // 0b 0000 00~~ - Любые свободные действия ("Стрельба" и/или "Прицеливание")

    if (OldActions != CurrentActions)
    {
        Server_SetCurrentActions(CurrentActions);

        // Завершение Действия по Отключенному Биту
        switch (EActionVariations(OldActions & ~CurrentActions))
        {
        case EActionVariations::Aiming:
            StopAiming();
            break;

        case EActionVariations::Shooting:
            StopShooting();
            break;

        case EActionVariations::Reloading:
            StopReloading();
            break;

        case EActionVariations::Changing:
            StopChanging();
            break;

        case EActionVariations::Block:
            StopBlockingActions();
            break;

        default:
            break;
        }

        // Начать Действие по Включенному Биту
        switch (EActionVariations(CurrentActions & ~OldActions))
        {
        case EActionVariations::Aiming:
            StartAiming();
            break;

        case EActionVariations::Shooting:
            StartShooting();
            break;

        case EActionVariations::Reloading:
            StartReloading();
            break;

        case EActionVariations::Changing:
            StartChangeWeaponSlot();
            break;

        case EActionVariations::Block:
            break;

        default:
            break;
        }

        if (CheckAction(EActionVariations::Aiming))
        {
            SetSpeedControl(ESpeedVariations::Walk);
        }
        else if (CurrentActions > uint8(EActionVariations::Aiming))
        {
            SetSpeedControl(ESpeedVariations::Jog);
        }
        else
        {
            SetSpeedControl(ESpeedVariations::Sprint);
        }

        if (PlayerOwner)
        {
            PlayerOwner->GetFPSCharacterMovement()->UpdateMaxSpeed();
        }
    }
}

void UWeaponControlComponent::Server_SetCurrentActions_Implementation(const uint8& Value)
{
    CurrentActions = Value;
}

bool UWeaponControlComponent::CheckActions(const EActionVariations& Action, ...) const
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
//--------------------------------------------------------------------------------------



/* ---   Actions | Set   --- */

void UWeaponControlComponent::SetAiming()
{
    SetActionBit(EActionVariations::Aiming);
}

void UWeaponControlComponent::SetShooting()
{
    SetActionBit(EActionVariations::Shooting);
}

void UWeaponControlComponent::SetReloading()
{
    if (CurrentSlot && CurrentWeaponData)
    {
        bool bChecker = false;

        if (CurrentSlot->bIsWeaponLoaded)
        {
            if (CurrentSlot->NumAllCartridge > 0
                && CurrentSlot->NumPreparedCartridges < CurrentWeaponData->MaxPreparedCartridges)
            {
                bChecker = true;
            }
        }
        else
        {
            if (CurrentSlot->NumAllCartridge > 0
                || CurrentSlot->NumPreparedCartridges > 0)
            {
                bChecker = true;
            }
        }

        if (bChecker)
        {
            SetBlockingActionBit(EActionVariations::Reloading);
        }
    }
}

void UWeaponControlComponent::SetChanging(FWeaponSlotData* iNewSlot)
{
    if (iNewSlot && iNewSlot != CurrentSlot)
    {
        SetBlockingActionBit(EActionVariations::Changing);
        NewSlotForChangingWeapons = iNewSlot;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Reset   --- */

void UWeaponControlComponent::ResetAiming()
{
    ResetActionBit(EActionVariations::Aiming);
}

void UWeaponControlComponent::ResetShooting()
{
    ResetActionBit(EActionVariations::Shooting);
}

void UWeaponControlComponent::ResetReloading()
{
    ResetActionBit(EActionVariations::Reloading);
}

void UWeaponControlComponent::ResetChanging()
{
    ResetActionBit(EActionVariations::Changing);
}

void UWeaponControlComponent::ResetBlocking()
{
    ResetActionBit(EActionVariations::Block);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Stopped   --- */

void UWeaponControlComponent::StopAiming()
{
}

void UWeaponControlComponent::StopShooting()
{
    GetWorld()->GetTimerManager().ClearTimer(Timer_ActionControl);
}

void UWeaponControlComponent::StopReloading()
{
    if (CurrentSlot->NumPreparedCartridges < CurrentWeaponData->MaxPreparedCartridges
        && CurrentSlot->NumAllCartridge > 0)
    {
        int32 Difference = CurrentWeaponData->MaxPreparedCartridges - CurrentSlot->NumPreparedCartridges;
        Difference = FMath::Min(Difference, CurrentSlot->NumAllCartridge);

        CurrentSlot->NumPreparedCartridges += Difference;
        CurrentSlot->NumAllCartridge -= Difference;
    }

    if (CurrentSlot->bIsWeaponLoaded == false
        && CurrentSlot->NumPreparedCartridges > 0)
    {
        CurrentSlot->bIsWeaponLoaded = true;
        --(CurrentSlot->NumPreparedCartridges);
    }

    OnReloadingWeapon.Broadcast();
    //DropActor(CurrentWeaponData->StorageDropType, CurrentWeaponFrame->StorageDropGuidance);
}

void UWeaponControlComponent::StopChanging()
{
}

void UWeaponControlComponent::StopBlockingActions()
{
    if (CurrentActions & (uint8)EActionVariations::Block)
    {
        CurrentActions ^= (uint8)EActionVariations::Block;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Started   --- */

void UWeaponControlComponent::StartAiming()
{
}

void UWeaponControlComponent::StartShooting()
{
    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponControlComponent::ShootingWeapon,
        CurrentWeaponData->ShootingWeapon_Time,
        true,
        0.f); // Реагировать сразу
}

void UWeaponControlComponent::StartReloading()
{
    OnStartReloadingWeapon.Broadcast();

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponControlComponent::ResetReloading,
        CurrentWeaponData->ReloadingWeapon_Time,
        false);
}

void UWeaponControlComponent::StartChangeWeaponSlot()
{
    OnStartChangingWeapon.Broadcast();

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponControlComponent::ChangeWeaponSlot,
        CurrentWeaponData->RemoveWeapon_Time,
        false);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Reaction   --- */

void UWeaponControlComponent::ShootingWeapon()
{
    if (CurrentWeaponData->ProjectileType)
    {
        // Флаг проверки условий стрельбы
        bool bChecker = false;

        // Проверка
        if (CurrentSlot->NumPreparedCartridges)
        {
            // Уменьшение количества подготовленных Патронов в чём-либо (в магазине, обойме и т.п.)
            --(CurrentSlot->NumPreparedCartridges);
            bChecker = true;
        }
        else if (CurrentSlot->bIsWeaponLoaded)
        {
            // "Использование" заряженного Патрона
            CurrentSlot->bIsWeaponLoaded = false;
            bChecker = true;
        }

        // Результат
        if (bChecker)
        {
            OnShootingWeapon.Broadcast();
            //DropActor(CurrentWeaponData->ProjectileType, CurrentWeaponFrame->ShootGuidance);
            //DropActor(CurrentWeaponData->CaseDropType, CurrentWeaponFrame->CaseDropGuidance);
        }
        else
        {
            SetReloading();
        }
    }
}

void UWeaponControlComponent::ChangeWeaponSlot()
{
    CurrentSlot = NewSlotForChangingWeapons;
    CurrentWeaponData = WeaponsDataTable->FindRow<FWeaponData>(CurrentSlot->WeaponType, "ChangeWeaponSlot");
    CurrentWeaponFrame->UpdateWeaponOnSelectedData(CurrentWeaponData);

    OnChangingWeapon.Broadcast();

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponControlComponent::EndChangeWeaponSlot,
        CurrentWeaponData->TakeWeapon_Time,
        false);
}

void UWeaponControlComponent::EndChangeWeaponSlot()
{
    ResetChanging();
}
//--------------------------------------------------------------------------------------



/* ---   Character Movement Speed   --- */

void UWeaponControlComponent::SetSpeedControl(const ESpeedVariations& Mode)
{
    if (SpeedControl != Mode)
    {
        SpeedControl = Mode;
    }
}

void UWeaponControlComponent::InitSpeedControl()
{
    if (PlayerOwner)
    {
        PlayerOwner->GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
    }
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Inputs   --- */

TArray<FName> UWeaponControlComponent::GetActionGroupsNames()
{
    TArray<FName> ActionNames;

    UInputSettings::GetInputSettings()->GetActionNames(ActionNames);

    return ActionNames;
}


void UWeaponControlComponent::CheckInputsGroups()
{
    TArray<FName> lUsed = {
        /* ---   Switching   --- */
        ActionGroups_NextSlot,
        ActionGroups_PrevSlot,
        ActionGroups_SlotNum1,
        ActionGroups_SlotNum2,
        /* ---   Actions   --- */
        ActionGroups_Aiming,
        ActionGroups_Fire,
        ActionGroups_Reload,
    };

    TArray<FName> lArray_ActionNames;
    UInputSettings::GetInputSettings()->GetActionNames(lArray_ActionNames);

    for (FName& Data : lUsed)
    {
        if (Data == NAME_None)
        {
            UE_LOG(LogTemp, Warning, TEXT("'%s'::%s: Not used at least one of the Actions ('%s')"),
                *GetNameSafe(this), *FString(__func__), *Data.ToString());
        }
        else if (lArray_ActionNames.Find(Data) == INDEX_NONE)
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: '%s' is NOT an Action"),
                *GetNameSafe(this), *FString(__func__), *Data.ToString());
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

TArray<FName> UWeaponControlComponent::GetAllWeaponsNames() const
{
    if (WeaponsDataTable)
    {
        return WeaponsDataTable->GetRowNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------
#endif
//======================================================================================
