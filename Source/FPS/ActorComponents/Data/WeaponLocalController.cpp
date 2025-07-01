//

// Base:
#include "WeaponLocalController.h"

// UE:
#include "Components/ArrowComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/InputSettings.h"

// Net:
#include "Net/UnrealNetwork.h"

// Interaction:
#include "FPS/ActorComponents/Control/FPS_CharacterMovementComponent.h"
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Combat/FirstPersonWeaponFrame.h"
#include "FPS/Combat/Projectile.h"
#include "FPS/Combat/WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UWeaponLocalController::UWeaponLocalController()
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



/* ---   Base   --- */

void UWeaponLocalController::OnComponentCreated()
{
    Super::OnComponentCreated();

    BaseInit();

#if WITH_EDITOR
    CheckNumOfSlots();
#endif
}

void UWeaponLocalController::InitializeComponent()
{
    Super::InitializeComponent();

    DataInit();
}

void UWeaponLocalController::BeginPlay()
{
    Super::BeginPlay();

    SpeedControlInit();
}

void UWeaponLocalController::BaseInit()
{
    PlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (PlayerOwner)
    {
        WeaponControlNetComp = PlayerOwner->WeaponControlNetComp;

        if (!WeaponControlNetComp)
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: WeaponControlNetComp is NOT"),
                *GetNameSafe(this), *FString(__func__));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: PlayerOwner is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }

    if (FPWeaponFrameType)
    {
        SetChildActorClass(FPWeaponFrameType);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: FPWeaponFrameType is NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Inputs   --- */

void UWeaponLocalController::SetupPlayerInputs()
{
    UInputComponent* lInputComponent = Cast<APawn>(GetOwner())->InputComponent;

    if (lInputComponent)
    {
        /* ---   Switching   --- */

        if (ActionGroups_NextSlot != NAME_None)
            lInputComponent->BindAction(ActionGroups_NextSlot, IE_Pressed, this, &UWeaponLocalController::ToNextSlot);

        if (ActionGroups_PrevSlot != NAME_None)
            lInputComponent->BindAction(ActionGroups_PrevSlot, IE_Pressed, this, &UWeaponLocalController::ToPrevSlot);

        if (ActionGroups_SlotNum1 != NAME_None)
            lInputComponent->BindAction(ActionGroups_SlotNum1, IE_Pressed, this, &UWeaponLocalController::ToSlot1);

        if (ActionGroups_SlotNum2 != NAME_None)
            lInputComponent->BindAction(ActionGroups_SlotNum2, IE_Pressed, this, &UWeaponLocalController::ToSlot2);
        //-------------------------------------------


        /* ---   Actions   --- */

        if (ActionGroups_Aiming != NAME_None)
        {
            lInputComponent->BindAction(ActionGroups_Aiming, IE_Pressed, this, &UWeaponLocalController::SetAiming);
            lInputComponent->BindAction(ActionGroups_Aiming, IE_Released, this, &UWeaponLocalController::ResetAiming);
        }

        if (ActionGroups_Fire != NAME_None)
        {
            lInputComponent->BindAction(ActionGroups_Fire, IE_Pressed, this, &UWeaponLocalController::SetShooting);
            lInputComponent->BindAction(ActionGroups_Fire, IE_Released, this, &UWeaponLocalController::ResetShooting);
        }

        if (ActionGroups_Reload != NAME_None)
            lInputComponent->BindAction(ActionGroups_Reload, IE_Pressed, this, &UWeaponLocalController::SetReloading);
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

const FWeaponData& UWeaponLocalController::BP_GetCurrentWeaponData() const
{
    return *GetCurrentWeaponData();
}

const FWeaponSlotData& UWeaponLocalController::BP_GetCurrentSlotData() const
{
    return *GetCurrentSlotData();
}

void UWeaponLocalController::DataInit()
{
    if (WeaponsDataTable
        && WeaponSlots.IsValidIndex(0))
    {
        CurrentSlot = &WeaponSlots[0];

        if (WeaponControlNetComp)
        {
            WeaponControlNetComp->WeaponDataSlots.Empty(WeaponsDataTable->GetRowNames().Num());

            for (FWeaponSlotData& Data : WeaponSlots)
            {
                WeaponControlNetComp->WeaponDataSlots.Add(
                    WeaponsDataTable->FindRow<FWeaponData>(Data.WeaponType, "DataInit"));
            }

            WeaponControlNetComp->CurrentWeaponData = WeaponControlNetComp->WeaponDataSlots[0];
            WeaponControlNetComp->DataInit();
        }

        if (GetCurrentWeaponData())
        {
            CurrentFPWeaponFrame = Cast<AFirstPersonWeaponFrame>(GetChildActor());

            if (CurrentFPWeaponFrame)
            {
                CurrentFPWeaponFrame->AttachToComponent(
                    PlayerOwner->GetMesh(),
                    FAttachmentTransformRules::KeepRelativeTransform,
                    WeaponSocketInFPMesh);

                CurrentFPWeaponFrame->UpdateWeaponOnSelectedData(GetCurrentWeaponData());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("'%s'::%s: Child Actor is NOT AFirstPersonWeaponFrame (Check CurrentWeaponFrame)"),
                    *GetNameSafe(this), *FString(__func__));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: CurrentWeaponData is NOT"),
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

void UWeaponLocalController::SetCurrentSlotByNum(const uint8& iNum)
{
    if (WeaponSlots.IsValidIndex(iNum))
        SetChanging(iNum);
}

void UWeaponLocalController::ToSlot1()
{
    SetCurrentSlotByNum(1);
}

void UWeaponLocalController::ToSlot2()
{
    SetCurrentSlotByNum(2);
}

void UWeaponLocalController::ToNextSlot()
{
    SetCurrentSlotByNum(
        CurrentSlot == &WeaponSlots.Last(0)
        ? 0
        : CurrentSlot - &WeaponSlots[0] + 1);
}

void UWeaponLocalController::ToPrevSlot()
{
    SetCurrentSlotByNum(
        CurrentSlot == &WeaponSlots[0]
        ? WeaponSlots.Num() - 1
        : CurrentSlot - &WeaponSlots[0] - 1);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Data   --- */

void UWeaponLocalController::UpdateCurrentActions()
{
    uint8 OldActions = GetCurrentActions();

    GetCurrentActions() =

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

    if (OldActions != GetCurrentActions())
    {
        WeaponControlNetComp->Server_SetCurrentActions(GetCurrentActions());

        // Завершение Действия по Отключенному Биту
        switch (EActionVariations(OldActions & ~GetCurrentActions()))
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
        switch (EActionVariations(GetCurrentActions() & ~OldActions))
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
        else if (GetCurrentActions() > uint8(EActionVariations::Aiming))
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

bool UWeaponLocalController::CheckActions(const EActionVariations& Action, ...) const
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

void UWeaponLocalController::SetAiming()
{
    SetActionBit(EActionVariations::Aiming);
}

void UWeaponLocalController::SetShooting()
{
    SetActionBit(EActionVariations::Shooting);
}

void UWeaponLocalController::SetReloading()
{
    if (CurrentSlot && GetCurrentWeaponData())
    {
        bool bChecker = false;

        if (CurrentSlot->bIsWeaponLoaded)
        {
            if (CurrentSlot->NumAllCartridge > 0
                && CurrentSlot->NumPreparedCartridges < GetCurrentWeaponData()->MaxPreparedCartridges)
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

void UWeaponLocalController::SetChanging(const uint8& iNum)
{
    if (&WeaponSlots[iNum] != CurrentSlot)
    {
        SetBlockingActionBit(EActionVariations::Changing);
        NewSlotNum = iNum;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Reset   --- */

void UWeaponLocalController::ResetAiming()
{
    ResetActionBit(EActionVariations::Aiming);
}

void UWeaponLocalController::ResetShooting()
{
    ResetActionBit(EActionVariations::Shooting);
}

void UWeaponLocalController::ResetReloading()
{
    ResetActionBit(EActionVariations::Reloading);
}

void UWeaponLocalController::ResetChanging()
{
    ResetActionBit(EActionVariations::Changing);
}

void UWeaponLocalController::ResetBlocking()
{
    ResetActionBit(EActionVariations::Block);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Stopped   --- */

void UWeaponLocalController::StopAiming()
{
}

void UWeaponLocalController::StopShooting()
{
    GetWorld()->GetTimerManager().ClearTimer(Timer_ActionControl);
}

void UWeaponLocalController::StopReloading()
{
    if (CurrentSlot->NumPreparedCartridges < GetCurrentWeaponData()->MaxPreparedCartridges
        && CurrentSlot->NumAllCartridge > 0)
    {
        int32 Difference = GetCurrentWeaponData()->MaxPreparedCartridges - CurrentSlot->NumPreparedCartridges;
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

    FTransform lTransform = CurrentFPWeaponFrame->ShootGuidance->GetComponentTransform();

    WeaponControlNetComp->DropStorage(
        lTransform.GetLocation(),
        lTransform.Rotator());
}

void UWeaponLocalController::StopChanging()
{
}

void UWeaponLocalController::StopBlockingActions()
{
    if (GetCurrentActions() & (uint8)EActionVariations::Block)
    {
        GetCurrentActions() ^= (uint8)EActionVariations::Block;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Started   --- */

void UWeaponLocalController::StartAiming()
{
}

void UWeaponLocalController::StartShooting()
{
    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::ShootingWeapon,
        GetCurrentWeaponData()->ShootingWeapon_Time,
        true,
        0.f); // Реагировать сразу
}

void UWeaponLocalController::StartReloading()
{
    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::ResetReloading,
        GetCurrentWeaponData()->ReloadingWeapon_Time,
        false);
}

void UWeaponLocalController::StartChangeWeaponSlot()
{
    WeaponControlNetComp->Broadcast_OnStartChangingWeapon();

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::ChangeWeaponSlot,
        GetCurrentWeaponData()->RemoveWeapon_Time,
        false);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Reaction   --- */

void UWeaponLocalController::ShootingWeapon()
{
    if (GetCurrentWeaponData()->ProjectileType)
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
            FTransform lTransform = CurrentFPWeaponFrame->ShootGuidance->GetComponentTransform();

            WeaponControlNetComp->DropProjectile(
                lTransform.GetLocation(),
                lTransform.Rotator());

            lTransform = CurrentFPWeaponFrame->CaseDropGuidance->GetComponentTransform();

            WeaponControlNetComp->DropSleeve(
                lTransform.GetLocation(),
                lTransform.Rotator());
        }
        else
        {
            SetReloading();
        }
    }
}

void UWeaponLocalController::ChangeWeaponSlot()
{
    CurrentSlot = &WeaponSlots[NewSlotNum];

    WeaponControlNetComp->SetCurrentWeaponDataByNum(NewSlotNum);

    CurrentFPWeaponFrame->UpdateWeaponOnSelectedData(GetCurrentWeaponData());

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::EndChangeWeaponSlot,
        GetCurrentWeaponData()->TakeWeapon_Time,
        false);
}

void UWeaponLocalController::EndChangeWeaponSlot()
{
    ResetChanging();
}
//--------------------------------------------------------------------------------------



/* ---   Character Movement Speed   --- */

void UWeaponLocalController::SetSpeedControl(const ESpeedVariations& Mode)
{
    if (SpeedControl != Mode)
    {
        SpeedControl = Mode;
    }
}

void UWeaponLocalController::SpeedControlInit()
{
    if (PlayerOwner && PlayerOwner->IsLocallyControlled())
    {
        PlayerOwner->GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
    }
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Local   --- */

TArray<FName> UWeaponLocalController::GetBoneSocketsInFPMesh() const
{
    APlayerCharacter* lPlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (lPlayerOwner && lPlayerOwner->FPMesh)
    {
        return lPlayerOwner->FPMesh->GetAllSocketNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------



/* ---   Inputs   --- */

TArray<FName> UWeaponLocalController::GetActionGroupsNames()
{
    TArray<FName> ActionNames;

    UInputSettings::GetInputSettings()->GetActionNames(ActionNames);

    return ActionNames;
}


void UWeaponLocalController::CheckInputsGroups()
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

TArray<FName> UWeaponLocalController::GetAllWeaponsNames() const
{
    if (WeaponsDataTable)
    {
        return WeaponsDataTable->GetRowNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------



/* ---   Switching   --- */

void UWeaponLocalController::CheckNumOfSlots()
{
    // Проверка наличия слотов
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
#endif
//======================================================================================
