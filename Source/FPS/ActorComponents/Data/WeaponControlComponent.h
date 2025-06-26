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

// Generated:
#include "WeaponControlComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При Стрельбе Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShootingWeapon);

// Делегат: При Перезарядке Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadingWeapon);

// Делегат: При Старте смены Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartChangingWeapon);

// Делегат: При Смене Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangingWeapon);

/** DELEGATE: Метод вызова делегата с Ретрансляцией по Сети */
#define DELEGATE_METHOD_Broadcast_h(PropertyName) \
    void Broadcast_##PropertyName() \
    { \
        ##PropertyName.Broadcast(); \
        Server_##PropertyName(); \
    }
// ----------------------------------------------------------------------------------------------------



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



/* ---   Enums   --- */

/** Вариации Действий Игрока */
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EActionVariations : uint8
{
    Aiming = 1 << 0,    // Прицеливание
    Shooting = 1 << 1,  // Стрельба
    Reloading = 1 << 2, // Перезарядка
    Changing = 1 << 3,  // Смена

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

    // Делегат: При Стрельбе Оружия
    UPROPERTY(BlueprintAssignable)
    FOnShootingWeapon OnShootingWeapon;
    DELEGATE_METHOD_Broadcast_h(OnShootingWeapon);

    // Делегат: При Перезарядке Оружия
    UPROPERTY(BlueprintAssignable)
    FOnReloadingWeapon OnReloadingWeapon;
    DELEGATE_METHOD_Broadcast_h(OnReloadingWeapon);

    // Делегат: При Старте смены Оружия
    UPROPERTY(BlueprintAssignable)
    FOnStartChangingWeapon OnStartChangingWeapon;
    DELEGATE_METHOD_Broadcast_h(OnStartChangingWeapon);

    // Делегат: При Смене Оружия
    UPROPERTY(BlueprintAssignable)
    FOnChangingWeapon OnChangingWeapon;
    DELEGATE_METHOD_Broadcast_h(OnChangingWeapon);
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UWeaponControlComponent();
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Вызывается при Создании компонента в Редакторе или Игровом Процессе */
    virtual void OnComponentCreated() override;

    /** Вызывается при Уничтожении компонента в Редакторе или Игровом Процессе */
    virtual void DestroyComponent(bool bPromoteChildren = false) override;

    /** Инициализирует компонент до вызова в Игровом Процессе BeginPlay() Компонента и Актора-Владельца */
    virtual void InitializeComponent() override;

    /** Begins Play для Компонента */
    virtual void BeginPlay() override;
    //-------------------------------------------



    /* ---   Net   --- */

    // Создаваемый Каркас Оружия для вида от Первого Лица
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Net",
        meta = (DisplayName = "Weapon Frame Type"))
    TSubclassOf<AWeaponFrame> WeaponFrameType;

    /* Сокет Оружия в FPMesh */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Net",
        meta = (GetOptions = "GetBoneSocketsInMesh"))
    FName WeaponSocketInMesh = NAME_None;

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

    //

    /** Используется для регистрации реплицируемых Переменных */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Использовать ли Каркас Оружия от Первого Лица */
    void InitializeFirstPersonWeaponFrame();
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

    FORCEINLINE const FWeaponData* GetCurrentWeaponData() const { return CurrentWeaponData; };

    /** Получить данные текущего Слота */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Data",
        meta = (DisplayName = "Get Current Slot Data"))
    const FWeaponSlotData& BP_GetCurrentSlotData() const;

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

    /* ---   Delegates | Net   --- */

    /** OnShootingWeapon: Ретрансляцией на Сервер */
    UFUNCTION(Server, Reliable)
    void Server_OnShootingWeapon();

    /** OnShootingWeapon: Ретрансляцией на Клиенты */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnShootingWeapon();

    /** OnReloadingWeapon: Ретрансляцией на Сервер */
    UFUNCTION(Server, Reliable)
    void Server_OnReloadingWeapon();

    /** OnReloadingWeapon: Ретрансляцией на Клиенты */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnReloadingWeapon();

    /** OnStartChangingWeapon: Ретрансляцией на Сервер */
    UFUNCTION(Server, Reliable)
    void Server_OnStartChangingWeapon();

    /** OnStartChangingWeapon: Ретрансляцией на Клиенты */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnStartChangingWeapon();

    /** OnChangingWeapon: Ретрансляцией на Сервер */
    UFUNCTION(Server, Reliable)
    void Server_OnChangingWeapon();

    /** OnChangingWeapon: Ретрансляцией на Клиенты */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnChangingWeapon();
    //-------------------------------------------



    /* ---   Base   --- */

    /* Игрок-Владелец данного UWeaponControlComponent */
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Control|Check")
    APlayerCharacter* PlayerOwner = nullptr;

    // Указатель на текущий Каркас Оружия
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Control|Check")
    AWeaponFrame* CurrentWeaponFrame = nullptr;

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

    // Указатель на Данные выбранного Оружия из Таблицы
    FWeaponData* CurrentWeaponData = nullptr;

    //

    /** Инициализация данных */
    void DataInit();

    /** Проверка количества Слотов */
    void CheckNumOfSlots();
    //-------------------------------------------



    /* ---   Actions | Data   --- */

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

    /** Управление Оружием: Прицеливаться */
    void SetAiming();

    /** Управление Оружием: Стрелять */
    void SetShooting();

    /** Управление Оружием: Перезарядить */
    void SetReloading();

    /** Управление Оружием: Сменить Оружие */
    void SetChanging(FWeaponSlotData* NewSlot);

    /** Управление Оружием: Ограничивать Действия */
    FORCEINLINE void SetBlockingActionBit(const EActionVariations& InAction)
    {
        if (CurrentActions < (uint8)EActionVariations::Reloading)
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

    /* Создание и выброс Астора согласно его Типу и Направляющей */
    void DropActor(const TSubclassOf<AActor>& ActorType, const UArrowComponent* Guidance);

    /** DropActor: Ретрансляцией на Сервер */
    UFUNCTION(Server, Reliable)
    void Server_DropActor(UClass* ActorType, const FTransform& Transform);

    /** DropActor: Ретрансляцией на Клиенты */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_DropActor(UClass* ActorType, const FTransform& Transform);
    //-------------------------------------------



    /* ---   Character Movement Speed   --- */

    // Переменная контроля максимальной Скорости Персонажа
    ESpeedVariations SpeedControl = ESpeedVariations::Sprint;

    //

    /** Инициализация контроля Скорости */
    void SpeedControlInit() override;
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Net   --- */

    /** Получение наименований Сокетов текущего Меша в Mesh */
    UFUNCTION()
    TArray<FName> GetBoneSocketsInMesh() const;

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
#endif
    //===========================================
};
