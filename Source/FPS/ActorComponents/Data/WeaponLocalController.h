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



/* ---   Macros   --- */

/** Управление Оружием: Задать действие */
#define SET_ACTION(PropertyName) \
    FORCEINLINE void Set##PropertyName() \
    { \
        SetActionBit(EActionVariations::##PropertyName); \
    }

/** Управление Оружием: Сброс действия */
#define RESET_ACTION(PropertyName) \
    FORCEINLINE void Reset##PropertyName() \
    { \
        ResetActionBit(EActionVariations::##PropertyName); \
    }

//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UArrowComponent;
class UDataTable;

// Interaction:
class APlayerCharacter;
class AFirstPersonWeaponFrame;
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


    virtual void CreateChildActor() override;

    /** Инициализирует компонент до вызова в Игровом Процессе BeginPlay() Компонента и Актора-Владельца */
    virtual void InitializeComponent() override;

    /** Begins Play для Компонента */
    virtual void BeginPlay() override;
    //-------------------------------------------



    /* ---   Local   --- */

    // Создаваемый Каркас Оружия для вида от Первого Лица
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Local",
        meta = (DisplayName = "FP Weapon Frame Type"))
    TSubclassOf<AFirstPersonWeaponFrame> FPWeaponFrameType;

    /* Сокет Оружия в FPMesh */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Local",
        meta = (GetOptions = "GetSocketNamesInFPMesh",
            DisplayName = "Weapon Socket In FPMesh"))
    FName WeaponSocketInFPMesh = NAME_None;

    //

    /** Получить Игрока-Владельца данного `UWeaponControlComponent`
    @note   Может быть НЕ Безопасно, предварительно проверяется в `BaseInit()` с вызовом соответствующей ошибки */
    FORCEINLINE APlayerCharacter* GetPlayerOwner() const
    {
        return (APlayerCharacter*)GetOwner();
    };

    /** Получить текущий Каркас Оружия от Первого Лица
    @note   Может быть НЕ Безопасно, предварительно проверяется в `InitData()` с вызовом соответствующей ошибки */
    FORCEINLINE AFirstPersonWeaponFrame* GetCurrentFPWeaponFrame() const
    {
        return (AFirstPersonWeaponFrame*)GetChildActor();
    }

    /** Получить компонент Сетевого контроллера Оружия */
    UWeaponNetworkController* GetWeaponControlNetComp() const;
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
    FORCEINLINE const FWeaponData* GetCurrentWeaponData() const
    {
        return GetWeaponControlNetComp()->GetCurrentWeaponData();
    };

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
    void SetCurrentSlotByNum(uint8 Num);

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
        return GetWeaponControlNetComp()->CurrentActions;
    };

    /** Управление Оружием: Задать Действие */
    FORCEINLINE void SetActionBit(EActionVariations InAction)
    {
        if (SettingActions ^ (uint8)InAction)
        {
            SettingActions |= (uint8)InAction;
            UpdateCurrentActions();
        }
    };

    /** Управление Оружием: Прекратить Действие */
    FORCEINLINE void ResetActionBit(EActionVariations InAction)
    {
        if (SettingActions & (uint8)InAction)
        {
            SettingActions ^= (uint8)InAction;
            UpdateCurrentActions();
        }
    };

    /** Проверка действий Игрока
    @param  Action - Проверяемое действие */
    FORCEINLINE bool CheckAction(EActionVariations Action) const
    {
        return GetWeaponControlNetComp()->CheckAction(Action);
    };
    //-------------------------------------------



    /* ---   Character Movement Speed   --- */

    /** Задать значение скорости */
    UFUNCTION(BlueprintCallable,
        Category = "Speed Control",
        meta = (DisplayName = "Set Speed Control"))
    void SetSpeedControl(ESpeedVariations Mode) override;

    /** Инициализация контроля Скорости */
    void InitSpeedControl() override;
    //-------------------------------------------



private:

    /* ---   Direction Fire   --- */

    /** Перекрепить оружие для положения Прицеливания */
    void ReAttachWeaponForAiming();

    /** Перекрепить оружие для положения от бедра */
    void ReAttachWeaponForFromHip();
    //-------------------------------------------



    /* ---   Data   --- */

    // Указатель на текущий Слот (элемент массива) оружия
    FWeaponSlotData* CurrentSlot = nullptr;
    // PS: Итератор излишен...

    //

    /** Инициализация данных */
    void InitData();
    //-------------------------------------------



    /* ---   Actions | Data   --- */

    // Таймер для контроля Действий
    FTimerHandle Timer_ActionControl;

    //

    /** Обновить данные о Текущих действиях с предварительной проверкой */
    void UpdateCurrentActions();
    //-------------------------------------------



    /* ---   Actions | Set   --- */

    /** Управление Оружием: Прицеливаться */
    SET_ACTION(Aiming);

    /** Управление Оружием: Стрелять */
    SET_ACTION(Shooting);

    /** Управление Оружием: Перезарядить */
    void SetReloading();

    /** Управление Оружием: Сменить Оружие */
    void SetChanging(uint8 Num);

    /** Управление Оружием: Ограничивать Действия */
    FORCEINLINE void SetBlockingActionBit(EActionVariations InAction)
    {
        if (GetCurrentActions() < (uint8)EActionVariations::Reloading)
        {
            SetActionBit(InAction);
        }
    };
    //-------------------------------------------



    /* ---   Actions | Reset   --- */

    /** Управление Оружием: Прекратить Прицеливаться */
    RESET_ACTION(Aiming);

    /** Управление Оружием: Прекратить Стрелять */
    RESET_ACTION(Shooting);

    /** Управление Оружием: Прекратить Перезарядку */
    RESET_ACTION(Reloading);

    /** Управление Оружием: Прекратить Смену Оружия */
    RESET_ACTION(Changing);

    /** Управление Оружием: Прекратить Ограничивать Действия */
    RESET_ACTION(Block);
    //-------------------------------------------



    /* ---   Actions | Stop   --- */

    /** Останов Действия: Прицеливаться */
    FORCEINLINE void StopAiming() {};

    /** Останов Действия: Стрелять */
    FORCEINLINE void StopShooting()
    {
        GetWorld()->GetTimerManager().ClearTimer(Timer_ActionControl);
    };

    /** Останов Действия: Прекратить Перезарядку */
    void StopReloading();

    /** Останов Действия: Прекратить Смену оружия */
    void StopChanging() {};

    /** Останов Действия: Прекратить Ограничивать Действия */
    FORCEINLINE void StopBlockingActions()
    {
        if (GetCurrentActions() & (uint8)EActionVariations::Block)
        {
            GetCurrentActions() ^= (uint8)EActionVariations::Block;
        }
    };
    //-------------------------------------------



    /* ---   Actions | Start   --- */

    /** Запуск Действия: Прицеливаться */
    FORCEINLINE void StartAiming() {};

    /** Запуск Действия: Стрелять */
    void StartShooting();

    /** Запуск Действия: Перезарядка */
    void StartReloading();

    /** Запуск Действия: Убрать старое Оружие */
    void StartChangeWeaponSlot();

    /** Запуск Действия: Ограничивать Действия */
    void StartBlockingActions() {};
    //-------------------------------------------



    /* ---   Actions | Reaction   --- */

    /** Запуск Действия: Стрельба */
    void ShootingWeapons();

    /** Запуск Действия: Сменить Слот Оружия */
    void ChangeWeaponSlot();

    /** Запуск Действия: Взять новое Оружие */
    FORCEINLINE void EndChangeWeaponSlot()
    {
        ResetChanging();
    };
    //-------------------------------------------



    /* ---   Character Movement Speed   --- */

    // Переменная контроля максимальной Скорости Персонажа
    ESpeedVariations SpeedControl = ESpeedVariations::Sprint;
    //-------------------------------------------



    /* ---   Booleanas   --- */

    // Отслеживание задания Действий Игрока
    uint8 SettingActions = 0;

    // Номер нового выбранного слота
    uint8 NewSlotNum = 0;

    // Флаг: Использовать трассировку для направления стрельбы
    bool bUseTracingToGuideShooting = true;
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

public:

    /* ---   Base: Debugs   --- */

    /** Вызывается, когда какое-либо свойство этого объекта было изменено
    @note   Используется для проверки изменённых переменных */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /** Эта альтернативная версия PostEditChange вызывается при изменении свойств внутри структур */
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    //-------------------------------------------



private:

    /* ---   Local   --- */

    /** Получение наименований Сокетов текущего Меша в FPMesh */
    UFUNCTION()
    TArray<FName> GetSocketNamesInFPMesh() const;
    //-------------------------------------------



    /* ---   Inputs   --- */

    /* Получить имена зарегистрированных Групп Действий */
    UFUNCTION()
    TArray<FName> GetActionGroupsNames() const;

    /* Проверить группы входных данных */
    void CheckInputsGroups() const;
    //-------------------------------------------



    /* ---   Data   --- */

    /** Получение Названия строк из таблицы WeaponsDataTable */
    UFUNCTION()
    TArray<FName> GetAllWeaponsNames() const;
    //-------------------------------------------



    /* ---   Switching   --- */

    /** Проверка количества Слотов */
    void CheckNumOfSlots() const;
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};
//--------------------------------------------------------------------------------------



/* ---   undef   --- */

#undef SET_ACTION
#undef RESET_ACTION
//--------------------------------------------------------------------------------------
