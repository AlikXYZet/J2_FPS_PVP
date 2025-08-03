//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Components/ChildActorComponent.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"

// Generated:
#include "WeaponNetworkController.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** DELEGATE: Метод вызова делегата с Ретрансляцией по Сети */
#define DELEGATE_METHOD_Broadcast_h(PropertyName) \
    void Broadcast_##PropertyName() \
    { \
        ##PropertyName.Broadcast(); \
        Server_##PropertyName(); \
    }
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
// ----------------------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

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
class FPS_API UWeaponNetworkController : public UChildActorComponent
{
    GENERATED_BODY()

    friend class UWeaponLocalController;

public:

    /* ---   Delegates   --- */

    // Делегат: При Стрельбе Оружия
    UPROPERTY(BlueprintAssignable)
    FOnShootingWeapon OnShootingWeapon;
    //DELEGATE_METHOD_Broadcast_h(OnShootingWeapon); // Заменяется на RPC 'DropProjectile' с вызовом '.Broadcast()'

    // Делегат: При Перезарядке Оружия
    UPROPERTY(BlueprintAssignable)
    FOnReloadingWeapon OnReloadingWeapon;
    //DELEGATE_METHOD_Broadcast_h(OnReloadingWeapon); // Заменяется на RPC 'DropSleeve' с вызовом '.Broadcast()'

    // Делегат: При Старте смены Оружия
    UPROPERTY(BlueprintAssignable)
    FOnStartChangingWeapon OnStartChangingWeapon;
    DELEGATE_METHOD_Broadcast_h(OnStartChangingWeapon);

    // Делегат: При Смене Оружия
    UPROPERTY(BlueprintAssignable)
    FOnChangingWeapon OnChangingWeapon;
    //DELEGATE_METHOD_Broadcast_h(OnChangingWeapon); // Заменяется на RPC 'SetCurrentWeaponDataByNum' с вызовом '.Broadcast()'
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UWeaponNetworkController();
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

    //

    /** Используется для регистрации реплицируемых Переменных */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Использовать ли Каркас Оружия от Первого Лица */
    void InitializeFirstPersonWeaponFrame();
    //-------------------------------------------



    /* ---   Data   --- */

    /** Получить данные текущего Оружия */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Data",
        meta = (DisplayName = "Get Current Weapon Data"))
    const FWeaponData& BP_GetCurrentWeaponData() const;

    /** Получить данные текущего Оружия */
    FORCEINLINE const FWeaponData* GetCurrentWeaponData() const { return CurrentWeaponData; };
    //-------------------------------------------



    /* ---   Actions   --- */

    // Текущие Действия Игрока
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated,
        Category = "Weapon Control|Control",
        meta = (Bitmask, BitmaskEnum = EActionVariations))
    uint8 CurrentActions = 0;

    //

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



private:

    /* ---   Delegates | Net   --- */

    /** OnStartChangingWeapon: Ретрансляцией на Сервер */
    UFUNCTION(Server, Reliable)
    void Server_OnStartChangingWeapon();

    /** OnStartChangingWeapon: Ретрансляцией для Всех */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnStartChangingWeapon();
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

    //

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* ---   Data   --- */

    // Массив указателей Данных Оружия, соответствующие его слоту
    TArray<FWeaponData*> WeaponDataSlots;

    // Указатель на Данные выбранного Оружия из Таблицы
    FWeaponData* CurrentWeaponData = nullptr;

    //

    /** Инициализация данных */
    void DataInit();

    /** Проверка количества Слотов */
    void CheckNumOfSlots();
    //-------------------------------------------



    /* ---   Actions | Data   --- */

    /** Метод изменения переменной CurrentActions через Сервер для её Репликации */
    UFUNCTION(Server, Reliable) // Принудительно Надёжный
    void Server_SetCurrentActions(const uint8& Value);

    /** Проверка действий Игрока
    @param  ActionsBits - Биты проверяемых действий */
    FORCEINLINE bool CheckActions(const uint8& ActionsBits) const
    {
        return CurrentActions & ActionsBits;
    };

    /** Создание и выброс Астора согласно его Типу, Локацией и Ротацией */
    AActor* DropActor(const TSubclassOf<AActor>& ActorType, const FVector& Location, const FRotator& Rotation);
    //-------------------------------------------



    /* ---   Actions | Switching   --- */

    /** Установить текущие данные оружия согласно Номеру */
    void SetCurrentWeaponDataByNum(const uint8& Num);

    /** Server: Установить текущие данные оружия согласно Номеру */
    UFUNCTION(Server, Reliable) // Принудительно Надёжный
    void Server_SetCurrentWeaponDataByNum(const uint8& Num);

    /** Multicast: Установить текущие данные оружия согласно Номеру */
    UFUNCTION(NetMulticast, Reliable) // Принудительно Надёжный
    void Multicast_SetCurrentWeaponDataByNum(const uint8& Num);

    /** Изменение текущие данные слота согласно номеру */
    void ChangingCurrentWeaponDataByNum(const uint8& Num);
    //-------------------------------------------



    /* ---   Actions | Shooting   --- */

    /** Выбросить Снаряд с указанными Локацией и Ротацией */
    void DropProjectile(const FVector& Location, const FRotator& Rotation);

    /** Server: Выбросить Снаряд с указанными Локацией и Ротацией */
    UFUNCTION(Server, Reliable) // Принудительно Надёжный
    void Server_DropProjectile(const FVector& Location, const FRotator& Rotation);

    /** Multicast: Выбросить Снаряд с указанными Локацией и Ротацией */
    UFUNCTION(NetMulticast, Reliable) // Принудительно Надёжный
    void Multicast_DropProjectile(const FVector& Location, const FRotator& Rotation);

    void CreateProjectile(const FVector& Location, const FRotator& Rotation);

    /** Выбросить Гильзу с указанными Локацией и Ротацией */
    void DropSleeve(const FVector& Location, const FRotator& Rotation);

    /** Server: Выбросить Гильзу с указанными Локацией и Ротацией */
    UFUNCTION(Server, Unreliable)
    void Server_DropSleeve(const FVector& Location, const FRotator& Rotation);

    /** Multicast: Выбросить Гильзу с указанными Локацией и Ротацией */
    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_DropSleeve(const FVector& Location, const FRotator& Rotation);
    //-------------------------------------------



    /* ---   Actions | Reloading   --- */

    /** Выбросить Накопителя с указанными Локацией и Ротацией */
    void DropStorage(const FVector& Location, const FRotator& Rotation);

    /** Server: Выбросить Накопителя с указанными Локацией и Ротацией */
    UFUNCTION(Server, Unreliable)
    void Server_DropStorage(const FVector& Location, const FRotator& Rotation);

    /** Multicast: Выбросить Накопителя с указанными Локацией и Ротацией */
    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_DropStorage(const FVector& Location, const FRotator& Rotation);
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Net   --- */

    /** Получение наименований Сокетов текущего Меша в Mesh */
    UFUNCTION()
    TArray<FName> GetBoneSocketsInMesh() const;
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};
