//

// Base:
#include "WeaponSlotsComponent.h"

// Interaction:
#include "FPS/Combat/WeaponFrame.h"
#include "FPS/Combat/Projectile.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UWeaponSlotsComponent::UWeaponSlotsComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UWeaponSlotsComponent::BeginPlay()
{
    Super::BeginPlay();

    WeaponSlotsComponentInit();
    SetupPlayerInputs();
}

void UWeaponSlotsComponent::OnComponentCreated()
{
    Super::OnComponentCreated();

    CheckNumOfSlots();
}

void UWeaponSlotsComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);

    RemovePlayerInputs();
}
//--------------------------------------------------------------------------------------



/* ---   Weapon Slots | Data   --- */

TArray<FName> UWeaponSlotsComponent::GetDataFromWeaponsDataTable() const
{
    if (WeaponsDataTable)
    {
        return WeaponsDataTable->GetRowNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------



/* ---   Weapon Slots | Control   --- */

FWeaponSlotData& UWeaponSlotsComponent::GetCurrentSlotData() const
{
    return *CurentSlot;
}
//--------------------------------------------------------------------------------------



/* ---   Weapon Slots | Switching   --- */

void UWeaponSlotsComponent::SetCurrentSlotByNum(const uint8& iNum)
{
    if (WeaponSlots.Num() > 1)
    {
        FWeaponSlotData* lNewSlot = &WeaponSlots[iNum % WeaponSlots.Num()];

        if (CurentSlot != lNewSlot)
        {
            CurentSlot = lNewSlot;

            CurrentWeaponFrame->UpdateWeaponByName(CurentSlot->WeaponType);
        }
    }
}

void UWeaponSlotsComponent::ToSlot1()
{
    SetCurrentSlotByNum(1);
}

void UWeaponSlotsComponent::ToSlot2()
{
    SetCurrentSlotByNum(2);
}

void UWeaponSlotsComponent::ToNextSlot()
{
    if (WeaponSlots.Num() > 1)
    {
        if (CurentSlot == &WeaponSlots.Last(0))
        {
            CurentSlot = &WeaponSlots[0];
        }
        else
        {
            ++CurentSlot;
        }

        CurrentWeaponFrame->UpdateWeaponByName(CurentSlot->WeaponType);
    }
}

void UWeaponSlotsComponent::ToPrevSlot()
{
    if (WeaponSlots.Num() > 1)
    {
        if (CurentSlot == &WeaponSlots[0])
        {
            CurentSlot = &WeaponSlots.Last(0);
        }
        else
        {
            --CurentSlot;
        }

        CurrentWeaponFrame->UpdateWeaponByName(CurentSlot->WeaponType);
    }
}

void UWeaponSlotsComponent::CheckNumOfSlots()
{
    // Проверка количества слотов
    if (WeaponSlots.Num())
    {
        CurentSlot = &WeaponSlots[0];
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("'%s'::WeaponSlotsComponentInit: WeaponSlots.Num() == 0"),
            *GetNameSafe(this));
    }
}

void UWeaponSlotsComponent::WeaponSlotsComponentInit()
{
    // Авто подтягивание объекта Оружия к данному Компоненту
    if (!CurrentWeaponFrame)
    {
        TArray<AActor*> lAllChildActors;
        AWeaponFrame* lCheck;
        GetOwner()->GetAllChildActors(lAllChildActors);

        for (auto lChild : lAllChildActors)
        {
            lCheck = Cast<AWeaponFrame>(lChild);

            if (lCheck)
            {
                // Подтягивание первого доступного объекта Оружия
                CurrentWeaponFrame = lCheck;
                break;
            }
        }

        if (CurrentWeaponFrame)
        {
            CurrentWeaponFrame->SetCurrentWeaponSlotsComponent(this);
            CurrentWeaponFrame->UpdateWeaponByName(CurentSlot->WeaponType);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("'%s'::WeaponSlotsComponentInit: CurrentWeaponFrame is NOT"),
                *GetNameSafe(this));
        }
    }
}

void UWeaponSlotsComponent::SetupPlayerInputs()
{
    UInputComponent* lInputComponent = Cast<APawn>(GetOwner())->InputComponent;

    if (lInputComponent)
    {
        lInputComponent->BindAction("NextSlot", IE_Pressed, this, &UWeaponSlotsComponent::ToNextSlot);
        lInputComponent->BindAction("PrevSlot", IE_Pressed, this, &UWeaponSlotsComponent::ToPrevSlot);
        lInputComponent->BindAction("SlotNum1", IE_Pressed, this, &UWeaponSlotsComponent::ToSlot1);
        lInputComponent->BindAction("SlotNum2", IE_Pressed, this, &UWeaponSlotsComponent::ToSlot2);
    }
}

void UWeaponSlotsComponent::RemovePlayerInputs()
{
    UInputComponent* lInputComponent = Cast<APawn>(GetOwner())->InputComponent;

    if (lInputComponent)
    {
        lInputComponent->RemoveActionBinding("NextSlot", IE_Pressed);
        lInputComponent->RemoveActionBinding("PrevSlot", IE_Pressed);
        lInputComponent->RemoveActionBinding("SlotNum1", IE_Pressed);
        lInputComponent->RemoveActionBinding("SlotNum2", IE_Pressed);
    }
}
//--------------------------------------------------------------------------------------
