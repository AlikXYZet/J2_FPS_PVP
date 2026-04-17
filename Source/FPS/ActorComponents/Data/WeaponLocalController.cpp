//

// Base:
#include "WeaponLocalController.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/KismetMathLibrary.h"

// Interaction:
#include "FPS/ActorComponents/Control/FPS_CharacterMovementComponent.h"
#include "FPS/ActorComponents/Control/SmoothMovementComponent.h"
#include "FPS/ActorComponents/Control/SmoothRotationComponent.h"
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Combat/FirstPersonWeaponFrame.h"
#include "FPS/Combat/Projectile.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UWeaponLocalController::UWeaponLocalController()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно

    // If true, we call the virtual InitializeComponent()
    bWantsInitializeComponent = true;

    // Компонент НЕ реплицируем по умолчанию
    SetIsReplicatedByDefault(false);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UWeaponLocalController::OnComponentCreated()
{
    Super::OnComponentCreated();

    if (GetOwner<APlayerCharacter>())
    {
        if (!GetWeaponControlNetComp())
        {
            FPS_Error_Component("'GetWeaponControlNetComp()' is NOT");
        }
    }
    else
    {
        FPS_Error_Component("`GetPlayerOwner()` is NOT 'APlayerCharacter'");
    }

#if WITH_EDITOR
    CheckNumOfSlots();
#endif // WITH_EDITOR
}

void UWeaponLocalController::CreateChildActor()
{
    Super::CreateChildActor();

    if (!Cast<AFirstPersonWeaponFrame>(GetChildActor()))
    {
        FPS_Error_Component("'%s' is NOT 'AFirstPersonWeaponFrame' (Check `Child Actor`)",
            GetChildActor()
            ? *GetChildActor()->GetName()
            : *FString("None"));
    }
}

void UWeaponLocalController::InitializeComponent()
{
    Super::InitializeComponent();

    InitData();
}

void UWeaponLocalController::BeginPlay()
{
    Super::BeginPlay();
}
//--------------------------------------------------------------------------------------



/* ---   Local   --- */

FORCEINLINE UWeaponNetworkController* UWeaponLocalController::GetWeaponControlNetComp() const
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    return GetPlayerOwner()->WeaponControlNetComp;
}
//--------------------------------------------------------------------------------------



/* ---   Direction Fire   --- */

void UWeaponLocalController::ReAttachWeaponForAiming()
{
    if (GetCurrentFPWeaponFrame()->GetRootComponent()->GetAttachParent() != GetPlayerOwner()->FPCamera)
    {
        GetCurrentFPWeaponFrame()->AttachToComponent(
            GetPlayerOwner()->FPCamera,
            FAttachmentTransformRules::KeepWorldTransform);

        GetCurrentFPWeaponFrame()->SmoothMovementComponent->MoveToLocation(
            GetCurrentWeaponData()->AimingLocation);

        GetCurrentFPWeaponFrame()->SmoothRotationComponent->RotateToRotator();
    }
}

void UWeaponLocalController::ReAttachWeaponForFromHip()
{
    if (GetCurrentFPWeaponFrame()->GetRootComponent()->GetAttachParent() != this)
    {
        GetCurrentFPWeaponFrame()->AttachToComponent(
            this,
            FAttachmentTransformRules::KeepWorldTransform);

        GetCurrentFPWeaponFrame()->SmoothMovementComponent->MoveToLocation(
            GetCurrentWeaponData()->HipLocation);

        GetCurrentFPWeaponFrame()->SmoothRotationComponent->RotateToRotator();
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
        FPS_Error_Component("lInputComponent is NOT");
    }


    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Inputs   --- */

    CheckInputsGroups();
    //-------------------------------------------

#endif // WITH_EDITOR
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

bool UWeaponLocalController::AddCartridgesToReserve(const FName& WeaponType, const int32 Number)
{
    int32 lIndex = WeaponSlots.Find(WeaponType);

    if (lIndex != INDEX_NONE)
    {
        // Найденный индекс в следующих массивах валиден всегда
        FWeaponSlotData& lWeaponSlot = WeaponSlots[lIndex];
        FWeaponData* lWeaponDataSlot = GetWeaponControlNetComp()->WeaponDataSlots[lIndex];

        if (lWeaponDataSlot->MaxStoredCartridges > lWeaponSlot.NumAllCartridge)
        {
            lWeaponSlot.NumAllCartridge = FMath::Min(
                lWeaponDataSlot->MaxStoredCartridges,
                lWeaponSlot.NumAllCartridge + Number);

            return true;
        }
    }
    return false;
}

void UWeaponLocalController::InitData()
{
    if (WeaponsDataTable
        && WeaponSlots.IsValidIndex(0))
    {
        CurrentSlot = &WeaponSlots[0];

        if (UWeaponNetworkController* WNComp = GetWeaponControlNetComp())
        {
            WNComp->WeaponDataSlots.Empty(WeaponSlots.Num());

            for (FWeaponSlotData& Slot : WeaponSlots)
            {
                WNComp->WeaponDataSlots.Add(
                    WeaponsDataTable->FindRow<FWeaponData>(Slot.WeaponType, "InitData"));
            }

            WNComp->InitData();
        }

        if (const FWeaponData* WData = GetCurrentWeaponData())
        {
            GetCurrentFPWeaponFrame()->UpdateWeaponOnSelectedData(WData);

            GetCurrentFPWeaponFrame()->SmoothMovementComponent->MoveToLocation(WData->HipLocation);

            GetCurrentFPWeaponFrame()->SmoothRotationComponent->RotateToRotator();
        }
        else
        {
            FPS_Error_Component("'GetCurrentWeaponData()' is NOT");
        }
    }
    else if (!WeaponsDataTable)
    {
        FPS_Error_Component("WeaponsDataTable is NOT");
    }
    else if (!WeaponSlots.IsValidIndex(0))
    {
        FPS_Error_Component("WeaponSlots[0] is NOT");
    }
}
//--------------------------------------------------------------------------------------



/* ---   Switching   --- */

void UWeaponLocalController::SetCurrentSlotByNum(uint8 iNum)
{
    SetChanging(iNum);
}

void UWeaponLocalController::ToSlot1()
{
    SetChanging(0);
}

void UWeaponLocalController::ToSlot2()
{
    SetChanging(1);
}

void UWeaponLocalController::ToNextSlot()
{
    SetChanging(
        CurrentSlot == &WeaponSlots.Last(0)
        ? 0
        : CurrentSlot - &WeaponSlots[0] + 1);
}

void UWeaponLocalController::ToPrevSlot()
{
    SetChanging(
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

        // 0b 0000 100~
        : SettingActions & (uint8)EActionVariations::Changing
        ? SettingActions & uint8(EActionVariations::Aiming | EActionVariations::Changing)

        // 0b 0000 010~
        : SettingActions & (uint8)EActionVariations::Reloading
        ? SettingActions & uint8(EActionVariations::Aiming | EActionVariations::Reloading)

        // 0b 0000 00~~
        : SettingActions;

    // PS: CurrentActions ограничен следующими значениями по порядку приоритета от наибольшего к меньшему:
    // 0b 1000 0000 - Блокировка Действий
    // 0b 0000 100~ - Действие "Смена" с отслеживанием "Прицеливания"
    // 0b 0000 010~ - Действие "Перезарядка" с отслеживанием "Прицеливания"
    // 0b 0000 00~~ - Любые свободные действия ("Стрельба" и/или "Прицеливание")

    if (OldActions != GetCurrentActions())
    {
        GetWeaponControlNetComp()->Server_SetCurrentActions(GetCurrentActions());

        // FP: Вызов действия 'Block' может отключить несколько бит за раз, что приведёт к ошибке
        // завершения других действий -- НЕ запустится вызов соответствующих методов 'Stop####()'.
        // Однако, 'Block' мы пока НЕ используем (на данный момент).

        // Отключённый бит
        EActionVariations bDisabledBit = EActionVariations(OldActions & ~GetCurrentActions());
        // Включённый бит
        EActionVariations bEnabledBit = EActionVariations(GetCurrentActions() & ~OldActions);

        // Завершение Действия по Отключенному Биту
        switch (bDisabledBit)
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
        switch (bEnabledBit)
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
            StartBlockingActions();
            break;

        default:
            break;
        }

        /** Реакции на совокупность действий: */

        /* Скорость передвижения */
        {
            ESpeedVariations lSpeedMode = ESpeedVariations::Sprint;

            if (CheckAction(EActionVariations::Aiming))
            {
                lSpeedMode = ESpeedVariations::Walk;
            }
            else if (GetCurrentActions() > uint8(EActionVariations::Aiming))
            {
                lSpeedMode = ESpeedVariations::Jog;
            }

            SetSpeedControl(lSpeedMode);
        }

        /* Положение оружия */
        if (bool(EActionVariations::Shooting ^ (bDisabledBit | bEnabledBit))) // Игнорировать, если изменился только бит Стрельбы
        {
            if (GetCurrentActions() >= uint8(EActionVariations::Reloading))
            {
                ReAttachWeaponForFromHip();
                bUseTracingToGuideShooting = false;
            }
            else if (CheckAction(EActionVariations::Aiming))
            {
                ReAttachWeaponForAiming();
            }
            else
            {
                ReAttachWeaponForFromHip();
                bUseTracingToGuideShooting = true;
            }
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Set   --- */

void UWeaponLocalController::SetReloading()
{
    if (CurrentSlot && GetCurrentWeaponData())
    {
        bool bChecker = false;

        if (!IsWeaponLoaded())
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

void UWeaponLocalController::SetChanging(uint8 iNum)
{
    if (WeaponSlots.IsValidIndex(iNum)
        && &WeaponSlots[iNum] != CurrentSlot)
    {
        SetBlockingActionBit(EActionVariations::Changing);
        NewSlotNum = iNum;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Reset   --- */
// Упрощено в '.h'
//--------------------------------------------------------------------------------------



/* ---   Actions | Stopped   --- */

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

    if (IsWeaponNotLoaded()
        && CurrentSlot->NumPreparedCartridges > 0)
    {
        CurrentSlot->bIsWeaponLoaded = true;
        --(CurrentSlot->NumPreparedCartridges);
    }

    FTransform lTransform = GetCurrentFPWeaponFrame()->ShootGuidance->GetComponentTransform();

    GetWeaponControlNetComp()->DropStorage(
        lTransform.GetLocation(),
        lTransform.Rotator());
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Started   --- */

void UWeaponLocalController::StartShooting()
{
    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::ShootingWeapons,
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
    GetWeaponControlNetComp()->Broadcast_OnStartChangingWeapon();

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::ChangeWeaponSlot,
        GetCurrentWeaponData()->RemoveWeapon_Time,
        false);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Reaction   --- */

void UWeaponLocalController::ShootingWeapons()
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
    else if (IsWeaponLoaded())
    {
        // "Использование" заряженного Патрона
        CurrentSlot->bIsWeaponLoaded = false;
        bChecker = true;
    }

    // Результат
    if (bChecker)
    {
        // Указатель на трансформацию какой-либо Направляющей
        const FTransform* lGuidanceTransform = &GetCurrentFPWeaponFrame()->ShootGuidance->GetComponentTransform();

        const FVector lStartLocation = lGuidanceTransform->GetLocation();

        const FVector lEndTraceLocation =
            GetPlayerOwner()->FPCamera->GetComponentTransform()
            .TransformPosition(FVector(10000.f, 0.f, 0.f));

        if (!GetCurrentWeaponData()->bUseHitscanMethod
            && GetCurrentWeaponData()->ProjectileType)
        {
            FRotator lRotator;

            if (bUseTracingToGuideShooting)
            {
                FHitResult lHitResult;

                bool bCheckTraceResult =
                    GetWorld()->LineTraceSingleByChannel(
                        lHitResult,
                        GetPlayerOwner()->FPCamera->GetComponentLocation(),
                        lEndTraceLocation,
                        ECC_Projectiles,
                        GetPlayerOwner()->GetCollisionParamsForTrace());

                lRotator = UKismetMathLibrary::FindLookAtRotation(
                    lStartLocation,
                    bCheckTraceResult
                    ? lHitResult.Location
                    : lEndTraceLocation);
            }
            else
            {
                lRotator = lGuidanceTransform->Rotator();
            }

            GetWeaponControlNetComp()->DropProjectile(lStartLocation, lRotator);
        }
        else
        {
            GetWeaponControlNetComp()->TraceProjectile(lStartLocation, lEndTraceLocation);
        }

        lGuidanceTransform = &GetCurrentFPWeaponFrame()->CaseDropGuidance->GetComponentTransform();

        GetWeaponControlNetComp()->DropSleeve(
            lGuidanceTransform->GetLocation(),
            lGuidanceTransform->Rotator());
    }
    else
    {
        SetReloading();
    }
}

void UWeaponLocalController::ChangeWeaponSlot()
{
    CurrentSlot = &WeaponSlots[NewSlotNum];

    GetWeaponControlNetComp()->SetCurrentWeaponDataByNum(NewSlotNum);

    GetCurrentFPWeaponFrame()->UpdateWeaponOnSelectedData(GetWeaponControlNetComp()->WeaponDataSlots[NewSlotNum]);

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::EndChangeWeaponSlot,
        GetCurrentWeaponData()->TakeWeapon_Time,
        false);
}
//--------------------------------------------------------------------------------------



/* ---   Character Movement Speed   --- */

void UWeaponLocalController::SetSpeedControl(ESpeedVariations Mode)
{
    if (SpeedControl != Mode)
    {
        SpeedControl = Mode;
        GetPlayerOwner()->GetFPSCharacterMovement()->UpdateMaxSpeed();
    }
};

FORCEINLINE void UWeaponLocalController::InitSpeedControl()
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    GetPlayerOwner()->GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Base: Debugs   --- */

void UWeaponLocalController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property)
    {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();

        if (PropertyName == GET_MEMBER_NAME_CHECKED(UWeaponLocalController, FPWeaponFrameType))
        {
            if (FPWeaponFrameType)
            {
                SetChildActorClass(FPWeaponFrameType);
            }
            else
            {
                FPS_Error_Component("FPWeaponFrameType is NOT");
            }
        }
        else if (PropertyName == GET_MEMBER_NAME_CHECKED(UWeaponLocalController, WeaponsDataTable))
        {
            if (WeaponSlots.Num())
            {
                TArray<FName> AllNames = GetAllWeaponsNames();
                FName FirstName = AllNames.IsValidIndex(0) ? AllNames[0] : FName();

                for (auto Iter = WeaponSlots.CreateIterator(); Iter; ++Iter)
                {
                    if (!WeaponsDataTable->FindRow<FWeaponData>(Iter->WeaponType, "PostEditChangeProperty"))
                    {
                        if (FirstName.IsNone())
                        {
                            Iter->WeaponType = NAME_None;

                            FPS_LOG_Component(Warning, "Weapon Type from Slot Number '%d' is NOT",
                                Iter.GetIndex());
                        }
                        else
                        {
                            WeaponsDataTable->FindRow<FWeaponData>(FirstName, "PostEditChangeProperty");
                        }
                    }
                }
            }
        }
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}

//void UWeaponLocalController::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
//{
//    Super::PostEditChangeChainProperty(PropertyChangedEvent);
//}
//--------------------------------------------------------------------------------------



/* ---   Local   --- */

TArray<FName> UWeaponLocalController::GetSocketNamesInFPMesh() const
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

TArray<FName> UWeaponLocalController::GetActionGroupsNames()const
{
    TArray<FName> ActionNames;

    UInputSettings::GetInputSettings()->GetActionNames(ActionNames);

    return ActionNames;
}


void UWeaponLocalController::CheckInputsGroups() const
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
            FPS_LOG_Component(Warning, "Not used at least one of the Actions ('%s')",
                *Data.ToString());
        }
        else if (lArray_ActionNames.Find(Data) == INDEX_NONE)
        {
            FPS_Error_Component("'%s' is NOT an Action",
                *Data.ToString());
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

void UWeaponLocalController::CheckNumOfSlots() const
{
    // Проверка выбранных слотов оружия
    if (WeaponSlots.Num())
    {
        for (auto Iter = WeaponSlots.CreateConstIterator(); Iter; ++Iter)
        {
            if (Iter->WeaponType == NAME_None)
            {
                FPS_LOG_Component(Warning, "Weapon Type from Slot Number '%d' is NOT",
                    Iter.GetIndex());
            }
            else if (!WeaponsDataTable)
            {
                FPS_Error_Component("WeaponsDataTable is NOT");
            }
            else if (!WeaponsDataTable->FindRow<FWeaponData>(Iter->WeaponType, "CheckNumOfSlots"))
            {
                FPS_Error_Component("Weapon Type '%s' from Slot Number '%d' does NOT exist",
                    *Iter->WeaponType.ToString(),
                    Iter.GetIndex());
            }
        }
    }
    else
    {
        FPS_LOG_Component(Warning, "WeaponSlots.Num() == 0");
    }
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
