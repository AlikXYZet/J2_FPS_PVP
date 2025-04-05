//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// UE:
#include "Engine/DataTable.h"

// Generated:
#include "WeaponSlotsComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class AWeaponFrame;
class AProjectile;
//--------------------------------------------------------------------------------------



/* ---   Structs   --- */

/* Структура данных слота:
Оружие и его расходники
*/
USTRUCT(BlueprintType)
struct FWeaponSlotData
{
    GENERATED_BODY()

    /* ---   Slot Data   --- */

    // Тип Оружия, занимаемый данный Слот
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data")
    FName WeaponType = "NONE";

    // Количество всех Патронов без учёта подготовленных
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data")
    int32 NumAllCartridge = 0;

    // Количество подготовленных Патронов в чём-либо (в магазине, обойме и т.п.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data")
    int32 NumPreparedCartridges = 0;

    // Заряжено ли оружие?
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data")
    bool bIsWeaponLoaded = false;
    //-------------------------------------------
};
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

    // Таблица данных местоположения фигур
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Slots | Data",
        meta = (RequiredAssetDataTags = "RowStructure=WeaponData"))
    UDataTable* WeaponsDataTable;
    //-------------------------------------------



    /* ---   Weapon Slots | Control   --- */

    // Массив со слотами (данными) оружия
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

    // Массив со слотами (данными) оружия
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
