//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Interfaces:
#include "FPS/Tools/Interfaces/Movement/SpeedControllerInterface.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"
#include "FPS/Tools/Structs/Arsenal/WeaponSlotData.h"
#include "FPS/Tools/Structs/Movement/SpeedControlData.h"

// Generated:
#include "WeaponControlComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При старте смены Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartWeaponChanging);

// Делегат: При удалении старого Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveOldWeapon);

// Делегат: При Старте Смены Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeNewWeapon);
// ----------------------------------------------------------------------------------------------------



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
    Sprinting = 1 << 0,// Спринт (Быстрый бег)
    Aiming = 1 << 1,   // Прицеливание
    Shooting = 1 << 2, // Стрельба
    //X = 1 << 3, // Резерв

    //X = 1 << 4, // Резерв
    //X = 1 << 5, // Резерв
    //X = 1 << 6, // Резерв
    Block = 1 << 7,    // Блокировка всего
};
ENUM_CLASS_FLAGS(EActionVariations)
//--------------------------------------------------------------------------------------



/** Компонент контроля Оружия Игрока */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UWeaponControlComponent : public UChildActorComponent, public ISpeedControllerInterface
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    // Делегат: При старте смены Оружия
    UPROPERTY(BlueprintAssignable)
    FOnStartWeaponChanging OnStartWeaponChanging;

    // Делегат: При удалении старого Оружия
    UPROPERTY(BlueprintAssignable)
    FOnRemoveOldWeapon OnRemoveOldWeapon;

    // Делегат: При Старте Смены Оружия
    UPROPERTY(BlueprintAssignable)
    FOnTakeNewWeapon OnTakeNewWeapon;
    //-------------------------------------------



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
        Category = "Weapon Control|Control",
        meta = (TitleProperty = "WeaponType"))
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
    FORCEINLINE void SetActionBit(const EActionVariations& InAction)
    {
        if (SettingActions ^ (uint8)InAction)
        {
            SettingActions |= (uint8)InAction;
            UpdateCurrentActions();
        }
    };

    /** Управление Оружием: Прекратить Действие */
    FORCEINLINE void ResetActionBit(const EActionVariations& InAction)
    {
        if (SettingActions & (uint8)InAction)
        {
            SettingActions ^= (uint8)InAction;
            UpdateCurrentActions();
        }
    };

    /** Проверка действий Игрока
    @param  Action - Проверяемое действие */
    FORCEINLINE bool CheckAction(const EActionVariations& Action) const
    {
        return CheckActions((uint8)Action);
    };

    /** Проверка одного из действий Игрока
    @param  Action - Проверяемое действие */
    bool CheckActions(const EActionVariations& Action, ...) const;
    //-------------------------------------------



    /* ---   Character Movement Speed   --- */

    /** Задать значение скорости */
    UFUNCTION(BlueprintCallable,
        Category = "Speed Control",
        meta = (AutoCreateRefTerm = "Mode"))
    void SetSpeedControl(const ESpeedVariations& Mode) override;
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

    // Указатель на текущий Каркас Оружия
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Control|Check")
    AWeaponFrame* CurrentWeaponFrame = nullptr;

    // Указатель на текущий Слот (элемент массива) оружия
    FWeaponSlotData* CurrentSlot = nullptr;
    // PS: Итератор излишен...

    // Указатель на Данные выбранного Оружия из Таблицы
    FWeaponData* CurrentWeaponData;

    //

    void DataInit();

    /** Проверка количества Слотов */
    void CheckNumOfSlots();

    /** Запустить Анимацию Персонажа с проверкой */
    //FORCEINLINE void PlayCharacterAnim(UAnimMontage* AnimMontage)
    //{
    //    if (AnimMontage)
    //        PlayerOwner->PlayAnimMontage(AnimMontage);
    //    else
    //        PlayerOwner->StopAnimMontage();
    //};
    //-------------------------------------------



    /* ---   Actions   --- */

    // Отслеживание задания Действий Игрока
    uint8 SettingActions = 0;

    // Таймер для контроля Действий
    FTimerHandle Timer_ActionControl;

    //

    /** Обновить данные о Текущих действиях с предварительной проверкой */
    void UpdateCurrentActions();

    /** Метод изменения переменной CurrentActions через Сервер для её Репликации */
    UFUNCTION(Server, Reliable)
    void Server_SetCurrentActions(const uint8& Value);

    /** Проверка действий Игрока
    @param  ActionsBits - Биты проверяемых действий */
    FORCEINLINE bool CheckActions(const uint8& ActionsBits) const
    {
        return CurrentActions & ActionsBits;
    };
    //-------------------------------------------



    /* ---   Actions | Set   --- */

    // Указатель на новый Слот Оружия. Используется для контроля смены оружия
    FWeaponSlotData* NewSlotForChangingWeapons = nullptr;

    //

    /** Управление Оружием: Спринт */
    void SetSprinting();

    /** Управление Оружием: Прицеливаться */
    void SetAiming();

    /** Управление Оружием: Стрелять */
    void SetShooting();

    /** Управление Оружием: Перезарядить */
    void SetReloading();

    /** Управление Оружием: Сменить Оружие */
    void SetChanging(FWeaponSlotData* NewSlot);

    /** Управление Оружием: Ограничивать Действия */
    bool SetBlocking(const EActionVariations& InAction);
    //-------------------------------------------



    /* ---   Actions | Reset   --- */

    /** Управление Оружием: Прекратить Спринт */
    void ResetSprinting();

    /** Управление Оружием: Прекратить Прицеливаться */
    void ResetAiming();

    /** Управление Оружием: Прекратить Стрелять */
    void ResetShooting();

    /** Управление Оружием: Прекратить Ограничивать Действия */
    void ResetBlocking();
    //-------------------------------------------



    /* ---   Actions | Start   --- */

    /** Запуск Действия: Спринт */
    void StartSprinting();

    /** Запуск Действия: Прицеливаться */
    void StartAiming();

    /** Запуск Действия: Стрелять */
    void StartShooting();

    /** Запуск Действия: Перезарядка */
    void StartReloading();

    /** Запуск Действия: Убрать старое Оружие */
    void RemoveOldWeapon();

    /** Запуск Действия: Сменить Слот Оружия */
    void ChangeWeaponSlot();

    /** Запуск Действия: Взять новое Оружие */
    void TakeNewWeapon();
    //-------------------------------------------



    /* ---   Actions | Stop   --- */

    /** Останов Действия: Спринт */
    void StopSprinting();

    /** Останов Действия: Прицеливаться */
    void StopAiming();

    /** Запуск Действия: Стрелять */
    void StopShooting();

    /** Останов Действия: Прекратить Ограничивать Действия */
    void StopBlockingActions();
    //-------------------------------------------



    /* ---   Character Movement Speed   --- */

    /** Инициализация контроля Скорости */
    void InitSpeedControl() override;
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Inputs   --- */

    /* Получить имена зарегистрированных Групп Действий */
    UFUNCTION()
    TArray<FName> GetActionGroupsNames();

    /* Проверить группы входных данных */
    void CheckInputsGroups();
    //-------------------------------------------



    /* ---   Data   --- */

    /** Получение Названия строк из таблицы WeaponsDataTable */
    UFUNCTION()
    TArray<FName> GetAllWeaponsNames() const;
    //-------------------------------------------
#endif
    //===========================================
};
