//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponSlotData.h"

// Generated:
#include "WeaponSlotsComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;

// Interaction:
class AWeaponFrame;
class AProjectile;
//--------------------------------------------------------------------------------------



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UWeaponSlotsComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UWeaponSlotsComponent();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Вызывается при создании компонента */
    virtual void OnComponentCreated() override;

    /** Вызывается при уничтожении компонента
    @param bDestroyingHierarchy - значение True, если разрушается вся иерархия компонентов.
    Позволяет избежать дорогостоящих операций
    */
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
    //-------------------------------------------



    /* ---   Weapon Slots | Data   --- */

    // Таблица данных Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Slots | Data",
        meta = (RequiredAssetDataTags = "RowStructure=WeaponData"))
    UDataTable* WeaponsDataTable;
    //-------------------------------------------



    /* ---   Weapon Slots | Control   --- */

    // Массив со слотами (данными) Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Slots | Control")
    TArray<FWeaponSlotData> WeaponSlots = { FWeaponSlotData() };

    //

    /** Получить данные текущего Слота */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Slots | Control")
    FWeaponSlotData& GetCurrentSlotData() const;
    //-------------------------------------------



    /* ---   Weapon Slots | Switching   --- */

    /** Установить текущий Слот по номеру */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Slots | Switching")
    void SetCurrentSlotByNum(const uint8& Num);

    /** Выбрать Слот под номером 1 */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Slots | Switching")
    void ToSlot1();

    /** Выбрать Слот под номером 2 */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Slots | Switching")
    void ToSlot2();

    /** Перейти к следующему Слоту */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Slots | Switching")
    void ToNextSlot();

    /** Перейти к предыдущему Слоту */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Slots | Switching")
    void ToPrevSlot();
    //-------------------------------------------



private:

    /* ---   Weapon Slots | Data   --- */

    /** Получение Названий строк из таблицы WeaponsDataTable */
    UFUNCTION()
    TArray<FName> GetDataFromWeaponsDataTable() const;
    //-------------------------------------------



    /* ---   Weapon Slots | Control   --- */

    // Указатель на текущий Каркас Оружия
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Slots | Check")
    AWeaponFrame* CurrentWeaponFrame = nullptr;

    // Указатель на текущий Слот (элемент массива) оружия
    FWeaponSlotData* CurentSlot;
    // PS: Итератор излишен...

    //

    /** Проверка количества Слотов */
    void CheckNumOfSlots();

    /** Предварительная инициализация Данных */
    void WeaponSlotsComponentInit();

    /** Настроить управление выбора Слота */
    void SetupPlayerInputs();

    /** Убрать управление выбора Слота */
    void RemovePlayerInputs();
    //-------------------------------------------
};
