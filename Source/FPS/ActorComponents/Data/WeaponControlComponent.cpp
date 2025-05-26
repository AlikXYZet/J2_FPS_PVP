//

// Base:
#include "WeaponControlComponent.h"

// UE:
#include "Engine/DataTable.h"
#include "GameFramework/InputSettings.h"

// Net:
#include "Net/UnrealNetwork.h"

// Interaction:
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

    //BaseInit();
    SwitchingInit();
}

void UWeaponControlComponent::OnComponentCreated()
{
    Super::OnComponentCreated();

    CheckNumOfSlots();
}

void UWeaponControlComponent::BaseInit()
{
    PlayerOwner = Cast<APlayerCharacter>(GetOwner());

    if (!PlayerOwner)
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
            lInputComponent->BindAction(ActionGroups_Aiming, IE_Pressed, this, &UWeaponControlComponent::Aiming);
            lInputComponent->BindAction(ActionGroups_Aiming, IE_Released, this, &UWeaponControlComponent::StopAiming);
        }

        if (ActionGroups_Fire != NAME_None)
        {
            lInputComponent->BindAction(ActionGroups_Fire, IE_Pressed, this, &UWeaponControlComponent::Fire);
            lInputComponent->BindAction(ActionGroups_Fire, IE_Released, this, &UWeaponControlComponent::StopFire);
        }

        if (ActionGroups_Reload != NAME_None)
            lInputComponent->BindAction(ActionGroups_Reload, IE_Pressed, this, &UWeaponControlComponent::Reloading);
        //-------------------------------------------
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("'%s'::%s: lInputComponent NOT"),
            *GetNameSafe(this), *FString(__func__));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

FWeaponSlotData& UWeaponControlComponent::GetCurrentSlotData() const
{
    return *CurrentSlot;
}
//--------------------------------------------------------------------------------------



/* ---   Switching   --- */

void UWeaponControlComponent::SetCurrentSlotByNum(const uint8& iNum)
{
    if (WeaponSlots.IsValidIndex(iNum)
        && CurrentActions < (uint8)EActionVariations::Restrict)
    {
        //SetRestrictedAction(EActionVariations::Restrict);

        FWeaponSlotData* lNewSlot = &WeaponSlots[iNum % WeaponSlots.Num()];

        if (CurrentSlot != lNewSlot)
        {
            CurrentSlot = lNewSlot;

            CurrentWeaponFrame->UpdateWeaponByName(CurrentSlot->WeaponType);
        }
    }
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
    if (WeaponSlots.Num() > 1
        && CurrentActions < (uint8)EActionVariations::Restrict)
    {
        //SetRestrictedAction(EActionVariations::Restrict);

        if (CurrentSlot == &WeaponSlots.Last(0))
        {
            CurrentSlot = &WeaponSlots[0];
        }
        else
        {
            ++CurrentSlot;
        }

        CurrentWeaponFrame->UpdateWeaponByName(CurrentSlot->WeaponType);
    }
}

void UWeaponControlComponent::ToPrevSlot()
{
    if (WeaponSlots.Num() > 1
        && CurrentActions < (uint8)EActionVariations::Restrict)
    {
        //SetRestrictedAction(EActionVariations::Restrict);

        if (CurrentSlot == &WeaponSlots[0])
        {
            CurrentSlot = &WeaponSlots.Last(0);
        }
        else
        {
            --CurrentSlot;
        }

        CurrentWeaponFrame->UpdateWeaponByName(CurrentSlot->WeaponType);
    }
}

void UWeaponControlComponent::SwitchingInit()
{
    // Авто подтягивание объекта Оружия к данному Компоненту
    if (!CurrentWeaponFrame)
    {
        CurrentWeaponFrame = Cast<AWeaponFrame>(GetChildActor());

        if (CurrentWeaponFrame)
        {
            CurrentWeaponFrame->WeaponsDataTable = WeaponsDataTable;
            CurrentWeaponFrame->UpdateWeaponByName(CurrentSlot->WeaponType);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::%s: Child Actor is NOT AWeaponFrame"),
                *GetNameSafe(this), *FString(__func__));
        }
    }
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



/* ---   Actions   --- */

void UWeaponControlComponent::UpdateCurrentActions()
{
    CurrentActions = SettingActions & (uint8)EActionVariations::Block
        ? (uint8)EActionVariations::Block
        : SettingActions;

    Server_SetCurrentActions(CurrentActions);
}

void UWeaponControlComponent::Server_SetCurrentActions_Implementation(const uint8& Value)
{
    CurrentActions = Value;
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Started   --- */

void UWeaponControlComponent::Aiming()
{
    SetAction(EActionVariations::Aiming);
}

void UWeaponControlComponent::Fire()
{
    SetRestrictedAction(EActionVariations::Shooting);
}

void UWeaponControlComponent::Reloading()
{
    SetRestrictedAction(EActionVariations::Restrict);
}
//--------------------------------------------------------------------------------------



/* ---   Actions | Stopped   --- */

void UWeaponControlComponent::StopAiming()
{
    StopAction(EActionVariations::Aiming);
}

void UWeaponControlComponent::StopFire()
{
    StopAction(EActionVariations::Shooting);
}

void UWeaponControlComponent::StopReloading()
{
    StopAction(EActionVariations::Restrict);
}

void UWeaponControlComponent::StopChanging()
{
    StopAction(EActionVariations::Restrict);
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
