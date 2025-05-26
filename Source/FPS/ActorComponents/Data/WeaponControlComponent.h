//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"
#include "FPS/Tools/Structs/Arsenal/WeaponSlotData.h"

// Generated:
#include "WeaponControlComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;

// Interaction:
class APlayerCharacter;
class AProjectile;
class AWeaponFrame;
//--------------------------------------------------------------------------------------



/* ---   Enums   --- */

/** Вариации Действий Игрока */
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EActionVariations : uint8
{
    Aiming = 1 << 0,   // Прицеливание
    Shooting = 1 << 1, // Стрельба
    Restrict = 1 << 2, // Ограничение
    Block = 1 << 3,    // Блокировка
};
ENUM_CLASS_FLAGS(EActionVariations)
//--------------------------------------------------------------------------------------



/** Компонент контроля Оружия Игрока */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UWeaponControlComponent : public UChildActorComponent
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UWeaponControlComponent();
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
    //-------------------------------------------



    /* ---   Net   --- */

    /** Используется для регистрации реплицируемых Переменных */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



    /* ---   Inputs   --- */

    /** Настроить управление Оружием и его Данными */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Inputs")
    void SetupPlayerInputs();
    //-------------------------------------------



    /* ---   Inputs | Switching   --- */

    // Группа Действий для "Следующий слот"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Inputs: ActionGroups|Switching",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Next Slot"))
    FName ActionGroups_NextSlot = NAME_None;

    // Группа Действий для "Предыдущий слот"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Inputs: ActionGroups|Switching",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Prev Slot"))
    FName ActionGroups_PrevSlot = NAME_None;

    // Группа Действий для "Слот #1"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Inputs: ActionGroups|Switching",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Slot Num 1"))
    FName ActionGroups_SlotNum1 = NAME_None;

    // Группа Действий для "Слот #2"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Inputs: ActionGroups|Switching",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Slot Num 2"))
    FName ActionGroups_SlotNum2 = NAME_None;
    //-------------------------------------------



    /* ---   Inputs | Actions   --- */

    // Группа Действий для "Прицеливание"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Inputs: ActionGroups|Actions",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Aiming"))
    FName ActionGroups_Aiming = NAME_None;

    // Группа Действий для "Стрельба"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Inputs: ActionGroups|Actions",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Fire"))
    FName ActionGroups_Fire = NAME_None;

    // Группа Действий для "Перезарядка"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Inputs: ActionGroups|Actions",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Reload"))
    FName ActionGroups_Reload = NAME_None;
    //-------------------------------------------



    /* ---   Data   --- */

    /* Таблица данных Оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Control|Data",
        meta = (RequiredAssetDataTags = "RowStructure=WeaponData"))
    UDataTable* WeaponsDataTable;

    // Массив со слотами (данными) Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Control|Control")
    TArray<FWeaponSlotData> WeaponSlots = { FWeaponSlotData() };

    //

    /** Получить данные текущего Слота */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Control")
    FWeaponSlotData& GetCurrentSlotData() const;
    //-------------------------------------------



    /* ---   Switching   --- */

    /** Установить текущий Слот по номеру */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Switching")
    void SetCurrentSlotByNum(const uint8& Num);

    /** Выбрать Слот под номером 1 */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Switching")
    void ToSlot1();

    /** Выбрать Слот под номером 2 */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Switching")
    void ToSlot2();

    /** Перейти к следующему Слоту */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Switching")
    void ToNextSlot();

    /** Перейти к предыдущему Слоту */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Switching")
    void ToPrevSlot();
    //-------------------------------------------



    /* ---   Actions   --- */

    // Текущие Действия Игрока
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated,
        Category = "Weapon Control|Control",
        meta = (Bitmask, BitmaskEnum = EActionVariations))
    uint8 CurrentActions = 0;

    //

    /** Управление Оружием: Задать Действие */
    FORCEINLINE void SetAction(const EActionVariations& InAction)
    {
        SettingActions |= uint8(InAction);

        UpdateCurrentActions();
    };

    /** Управление Оружием: Прекратить Действие */
    FORCEINLINE void StopAction(const EActionVariations& InAction)
    {
        SettingActions ^= uint8(InAction);

        UpdateCurrentActions();
    };
    //-------------------------------------------



private:

    /* ---   Base   --- */

    /* Игрок-Владелец данного UWeaponControlComponent */
    APlayerCharacter* PlayerOwner = nullptr;

    //

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* ---   Data   --- */

    // Указатель на текущий Слот (элемент массива) оружия
    FWeaponSlotData* CurrentSlot = nullptr;
    // PS: Итератор излишен...
    //-------------------------------------------



    /* ---   Switching   --- */

    // Указатель на текущий Каркас Оружия
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Control|Check")
    AWeaponFrame* CurrentWeaponFrame = nullptr;

    //

    /** Инициализация Данных */
    void SwitchingInit();

    /** Проверка количества Слотов */
    void CheckNumOfSlots();
    //-------------------------------------------



    /* ---   Actions   --- */

    // Отслеживание задания Действий Игрока
    uint8 SettingActions = 0;

    //

    /** Обновить данные о Текущих действиях с предварительной проверкой */
    void UpdateCurrentActions();

    /** Метод изменения переменной CurrentActions через Сервер для её Репликации */
    UFUNCTION(Server, Reliable)
    void Server_SetCurrentActions(const uint8& Value);
    //-------------------------------------------



    /* ---   Actions | Started   --- */

    /** Управление Оружием: Прицеливаться */
    void Aiming();

    /** Управление Оружием: Стрелять */
    void Fire();

    /** Управление Оружием: Перезарядить */
    void Reloading();

    /** Управление Оружием: Задать Ограниченное Действие */
    FORCEINLINE void SetRestrictedAction(const EActionVariations& InAction)
    {
        if (CurrentActions < (uint8)EActionVariations::Restrict)
            SetAction(InAction);
    };
    //-------------------------------------------



    /* ---   Actions | Stopped   --- */

    /** Управление Оружием: Прекратить Прицеливаться */
    void StopAiming();

    /** Управление Оружием: Прекратить Стрелять */
    void StopFire();

    /** Управление Оружием: Прекратить Перезарядку */
    void StopReloading();

    /** Управление Оружием: Прекратить смену Оружия */
    void StopChanging();
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Inputs   --- */

    /* Получить имена всех Функций-Предикатов Актора-Владельца */
    UFUNCTION()
    TArray<FName> GetActionGroupsNames();
    //-------------------------------------------



    /* ---   Data   --- */

    /** Получение Названия строк из таблицы WeaponsDataTable */
    UFUNCTION()
    TArray<FName> GetAllWeaponsNames() const;
    //-------------------------------------------
#endif
    //===========================================
};
