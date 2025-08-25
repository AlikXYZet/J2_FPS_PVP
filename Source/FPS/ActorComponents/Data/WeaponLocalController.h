//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Components/ChildActorComponent.h"

// Interfaces:
#include "FPS/Tools/Interfaces/Movement/SpeedControllerInterface.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"
#include "FPS/Tools/Structs/Arsenal/WeaponSlotData.h"
#include "FPS/Tools/Structs/Movement/SpeedControlData.h"

// Interaction:
#include "WeaponNetworkController.h"

// Generated:
#include "WeaponLocalController.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UArrowComponent;
class UDataTable;

// Interaction:
class APlayerCharacter;
class AFirstPersonWeaponFrame;
class AProjectile;
class AWeaponFrame;
//--------------------------------------------------------------------------------------



/** Компонент контроля Оружия Игрока */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UWeaponLocalController : public UChildActorComponent, public ISpeedControllerInterface
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UWeaponLocalController();
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Вызывается при Создании компонента в Редакторе или Игровом Процессе */
    virtual void OnComponentCreated() override;

    /** Инициализирует компонент до вызова в Игровом Процессе BeginPlay() Компонента и Актора-Владельца */
    virtual void InitializeComponent() override;

    /** Begins Play для Компонента */
    virtual void BeginPlay() override;
    //-------------------------------------------



    /* ---   Local   --- */

    // Создаваемый Каркас Оружия для вида от Первого Лица
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Net",
        meta = (DisplayName = "FP Weapon Frame Type"))
    TSubclassOf<AFirstPersonWeaponFrame> FPWeaponFrameType;

    /* Сокет Оружия в FPMesh */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Net",
        meta = (GetOptions = "GetBoneSocketsInFPMesh",
            DisplayName = "Weapon Socket In FPMesh"))
    FName WeaponSocketInFPMesh = NAME_None;
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
    UDataTable* WeaponsDataTable = nullptr;

    // Массив со слотами (данными) Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Control|Data",
        meta = (TitleProperty = "WeaponType"))
    TArray<FWeaponSlotData> WeaponSlots = { FWeaponSlotData() };

    //

    /** Получить данные текущего Оружия */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Data",
        meta = (DisplayName = "Get Current Weapon Data"))
    const FWeaponData& BP_GetCurrentWeaponData() const;

    /** Получить данные текущего Оружия */
    FORCEINLINE const FWeaponData* GetCurrentWeaponData() const { return WeaponControlNetComp->GetCurrentWeaponData(); };

    /** Получить данные текущего Слота */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Data",
        meta = (DisplayName = "Get Current Slot Data"))
    const FWeaponSlotData& BP_GetCurrentSlotData() const;

    /** Получить данные текущего Слота */
    FORCEINLINE const FWeaponSlotData* GetCurrentSlotData() const { return CurrentSlot; };
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

    /** Получить текущие Действия игрока */
    FORCEINLINE uint8& GetCurrentActions() const
    {
        return WeaponControlNetComp->CurrentActions;
    };

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
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Control|Check")
    APlayerCharacter* PlayerOwner = nullptr;

    // Компонент сетевого контроллера Оружия
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Control|Check")
    UWeaponNetworkController* WeaponControlNetComp = nullptr;

    // Указатель на текущий Каркас Оружия от Первого Лица
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Control|Check",
        meta = (DisplayName = "Current FP Weapon Frame"))
    AFirstPersonWeaponFrame* CurrentFPWeaponFrame = nullptr;

    //

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* ---   Data   --- */

    // Указатель на текущий Слот (элемент массива) оружия
    FWeaponSlotData* CurrentSlot = nullptr;
    // PS: Итератор излишен...

    //

    /** Инициализация данных */
    void DataInit();
    //-------------------------------------------



    /* ---   Actions | Data   --- */

    // Отслеживание задания Действий Игрока
    uint8 SettingActions = 0;

    // Таймер для контроля Действий
    FTimerHandle Timer_ActionControl;

    //

    /** Обновить данные о Текущих действиях с предварительной проверкой */
    void UpdateCurrentActions();

    /** Проверка действий Игрока
    @param  ActionsBits - Биты проверяемых действий */
    FORCEINLINE bool CheckActions(const uint8& ActionsBits) const
    {
        return GetCurrentActions() & ActionsBits;
    };
    //-------------------------------------------



    /* ---   Actions | Set   --- */

    // Номер нового выбранного слота
    uint8 NewSlotNum = 0;

    //

    /** Управление Оружием: Прицеливаться */
    void SetAiming();

    /** Управление Оружием: Стрелять */
    void SetShooting();

    /** Управление Оружием: Перезарядить */
    void SetReloading();

    /** Управление Оружием: Сменить Оружие */
    void SetChanging(const uint8& Num);

    /** Управление Оружием: Ограничивать Действия */
    FORCEINLINE void SetBlockingActionBit(const EActionVariations& InAction)
    {
        if (GetCurrentActions() < (uint8)EActionVariations::Reloading)
        {
            SetActionBit(InAction);
        }
    };
    //-------------------------------------------



    /* ---   Actions | Reset   --- */

    /** Управление Оружием: Прекратить Прицеливаться */
    void ResetAiming();

    /** Управление Оружием: Прекратить Стрелять */
    void ResetShooting();

    /** Управление Оружием: Прекратить Перезарядку */
    void ResetReloading();

    /** Управление Оружием: Прекратить Смену Оружия */
    void ResetChanging();

    /** Управление Оружием: Прекратить Ограничивать Действия */
    void ResetBlocking();
    //-------------------------------------------



    /* ---   Actions | Stop   --- */

    /** Останов Действия: Прицеливаться */
    void StopAiming();

    /** Останов Действия: Стрелять */
    void StopShooting();

    /** Останов Действия: Стрелять */
    void StopReloading();

    /** Останов Действия: Стрелять */
    void StopChanging();

    /** Останов Действия: Прекратить Ограничивать Действия */
    void StopBlockingActions();
    //-------------------------------------------



    /* ---   Actions | Start   --- */

    /** Запуск Действия: Прицеливаться */
    void StartAiming();

    /** Запуск Действия: Стрелять */
    void StartShooting();

    /** Запуск Действия: Перезарядка */
    void StartReloading();

    /** Запуск Действия: Убрать старое Оружие */
    void StartChangeWeaponSlot();
    //-------------------------------------------



    /* ---   Actions | Reaction   --- */

    /** Запуск Действия: Стрелять */
    void ShootingWeapon();

    /** Запуск Действия: Сменить Слот Оружия */
    void ChangeWeaponSlot();

    /** Запуск Действия: Взять новое Оружие */
    void EndChangeWeaponSlot();
    //-------------------------------------------



    /* ---   Character Movement Speed   --- */

    // Переменная контроля максимальной Скорости Персонажа
    ESpeedVariations SpeedControl = ESpeedVariations::Sprint;

    //

    /** Инициализация контроля Скорости */
    void InitSpeedControl() override;
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

public:

    /* ---   Base: Debugs   --- */

    /** Вызывается, когда какое-либо свойство этого объекта было изменено
    * @note Используется для проверки изменённых переменных
    */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /** Эта альтернативная версия PostEditChange вызывается при изменении свойств внутри структур */
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    //-------------------------------------------



private:

    /* ---   Local   --- */

    /** Получение наименований Сокетов текущего Меша в FPMesh */
    UFUNCTION()
    TArray<FName> GetBoneSocketsInFPMesh() const;
    //-------------------------------------------



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



    /* ---   Switching   --- */

    /** Проверка количества Слотов */
    void CheckNumOfSlots();
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};
