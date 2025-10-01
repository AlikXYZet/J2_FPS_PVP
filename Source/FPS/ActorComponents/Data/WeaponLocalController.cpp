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
#endif // WITH_EDITOR
}

void UWeaponLocalController::InitializeComponent()
{
    Super::InitializeComponent();

    InitData();
}

void UWeaponLocalController::BeginPlay()
{
    Super::BeginPlay();

    InitSpeedControl();
}
//--------------------------------------------------------------------------------------



/* ---   Local   --- */

void UWeaponLocalController::BaseInit()
{
    PlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (PlayerOwner)
    {
        WeaponControlNetComp = PlayerOwner->WeaponControlNetComp;

        if (!WeaponControlNetComp)
        {
            FPS_LOG(Error, TEXT("WeaponControlNetComp is NOT"));
        }
    }
    else
    {
        FPS_LOG(Error, TEXT("PlayerOwner is NOT"));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Direction Fire   --- */

void UWeaponLocalController::ReAttachWeaponForAiming()
{
    if (CurrentFPWeaponFrame->GetRootComponent()->GetAttachParent() != PlayerOwner->FPCamera)
    {
        CurrentFPWeaponFrame->AttachToComponent(
            PlayerOwner->FPCamera,
            FAttachmentTransformRules::KeepWorldTransform);

        CurrentFPWeaponFrame->SmoothMovementComponent->MoveToLocation(
            GetCurrentWeaponData()->AimingLocation);

        CurrentFPWeaponFrame->SmoothRotationComponent->RotateToRotator();
    }
}

void UWeaponLocalController::ReAttachWeaponForFromHip()
{
    if (CurrentFPWeaponFrame->GetRootComponent()->GetAttachParent() != this)
    {
        CurrentFPWeaponFrame->AttachToComponent(
            this,
            FAttachmentTransformRules::KeepWorldTransform);

        CurrentFPWeaponFrame->SmoothMovementComponent->MoveToLocation(
            GetCurrentWeaponData()->HipLocation);

        CurrentFPWeaponFrame->SmoothRotationComponent->RotateToRotator();
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
        FPS_LOG(Error, TEXT("lInputComponent is NOT"));
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

void UWeaponLocalController::InitData()
{
    if (WeaponsDataTable
        && WeaponSlots.IsValidIndex(0))
    {
        CurrentSlot = &WeaponSlots[0];

        if (WeaponControlNetComp)
        {
            WeaponControlNetComp->WeaponDataSlots.Empty(WeaponSlots.Num());

            for (FWeaponSlotData& Slot : WeaponSlots)
            {
                WeaponControlNetComp->WeaponDataSlots.Add(
                    WeaponsDataTable->FindRow<FWeaponData>(Slot.WeaponType, "InitData"));
            }

            WeaponControlNetComp->CurrentWeaponData = WeaponControlNetComp->WeaponDataSlots[0];
            WeaponControlNetComp->InitData();
        }

        if (GetCurrentWeaponData())
        {
            CurrentFPWeaponFrame = Cast<AFirstPersonWeaponFrame>(GetChildActor());

            if (CurrentFPWeaponFrame)
            {
                CurrentFPWeaponFrame->UpdateWeaponOnSelectedData(GetCurrentWeaponData());

                CurrentFPWeaponFrame->SmoothMovementComponent->MoveToLocation(
                    GetCurrentWeaponData()->HipLocation);

                CurrentFPWeaponFrame->SmoothRotationComponent->RotateToRotator();
            }
            else
            {
                FPS_LOG(Error, TEXT("'%s' is NOT AFirstPersonWeaponFrame (Check CurrentWeaponFrame)"),
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
    else if (!WeaponsDataTable)
    {
        FPS_LOG(Error, TEXT("WeaponsDataTable is NOT"));
    }
    else if (!WeaponSlots.IsValidIndex(0))
    {
        FPS_LOG(Error, TEXT("WeaponSlots[0] is NOT"));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Switching   --- */

void UWeaponLocalController::SetCurrentSlotByNum(const uint8& iNum)
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
        WeaponControlNetComp->Server_SetCurrentActions(GetCurrentActions());

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
    else if (CurrentSlot->bIsWeaponLoaded)
    {
        // "Использование" заряженного Патрона
        CurrentSlot->bIsWeaponLoaded = false;
        bChecker = true;
    }

    // Результат
    if (bChecker)
    {
        // Указатель на трансформацию какой-либо Направляющей
        const FTransform* lGuidanceTransform = &CurrentFPWeaponFrame->ShootGuidance->GetComponentTransform();

        const FVector lStartLocation = lGuidanceTransform->GetLocation();
        
        const FVector lEndTraceLocation =
            PlayerOwner->FPCamera->GetComponentTransform()
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
                        PlayerOwner->FPCamera->GetComponentLocation(),
                        lEndTraceLocation,
                        ECC_Projectiles,
                        PlayerOwner->CollisionParamsForTrace);

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

            WeaponControlNetComp->DropProjectile(lStartLocation, lRotator);
        }
        else
        {
            WeaponControlNetComp->TraceProjectile(lStartLocation, lEndTraceLocation);
        }

        lGuidanceTransform = &CurrentFPWeaponFrame->CaseDropGuidance->GetComponentTransform();

        WeaponControlNetComp->DropSleeve(
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

    WeaponControlNetComp->SetCurrentWeaponDataByNum(NewSlotNum);

    CurrentFPWeaponFrame->UpdateWeaponOnSelectedData(WeaponControlNetComp->WeaponDataSlots[NewSlotNum]);

    GetWorld()->GetTimerManager().SetTimer(
        Timer_ActionControl,
        this,
        &UWeaponLocalController::EndChangeWeaponSlot,
        GetCurrentWeaponData()->TakeWeapon_Time,
        false);
}
//--------------------------------------------------------------------------------------



/* ---   Character Movement Speed   --- */

void UWeaponLocalController::BP_SetSpeedControl(const ESpeedVariations& Mode)
{
    SetSpeedControl(Mode);
};

void UWeaponLocalController::SetSpeedControl(const ESpeedVariations& Mode)
{
    if (SpeedControl != Mode)
    {
        SpeedControl = Mode;

        PlayerOwner->GetFPSCharacterMovement()->UpdateMaxSpeed();
    }
};

void UWeaponLocalController::InitSpeedControl()
{
    if (PlayerOwner && PlayerOwner->IsLocallyControlled())
    {
        PlayerOwner->GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
    }
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Base: Debugs   --- */

void UWeaponLocalController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.Property
        && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UWeaponLocalController, FPWeaponFrameType))
    {
        if (FPWeaponFrameType)
        {
            SetChildActorClass(FPWeaponFrameType);
        }
        else
        {
            FPS_LOG(Error, TEXT("FPWeaponFrameType is NOT"));
        }
    }

    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UWeaponLocalController::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
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
            FPS_LOG(Warning, TEXT("Not used at least one of the Actions ('%s')"),
                *Data.ToString());
        }
        else if (lArray_ActionNames.Find(Data) == INDEX_NONE)
        {
            FPS_LOG(Error, TEXT("'%s' is NOT an Action"),
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

void UWeaponLocalController::CheckNumOfSlots()
{
    // Проверка наличия слотов
    if (!WeaponSlots.Num())
    {
        FPS_LOG(Warning, TEXT("WeaponSlots.Num() == 0"));
    }
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
