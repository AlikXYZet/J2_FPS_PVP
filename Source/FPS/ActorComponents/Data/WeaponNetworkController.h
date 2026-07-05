//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Components/ChildActorComponent.h"

// Global:
#include "FPS/Tools/GlobalFunctions.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"

// UE:
#include "Kismet/GameplayStatics.h"

// Generated:
#include "WeaponNetworkController.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** DELEGATE: Метод вызова делегата с Ретрансляцией по Сети */
#define DELEGATE_METHOD_Broadcast_h(PropertyName) \
    void Broadcast_##PropertyName() \
    { \
        Server_##PropertyName(); \
    }
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При Стрельбе Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShootingWeapon, const FWeaponData&, CurrentWeaponData);

// Делегат: При Перезарядке Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReloadingWeapon);

// Делегат: При Старте смены Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartChangingWeapon, const FWeaponData&, CurrentOldWeaponData);

// Делегат: При Смене Оружия
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangingWeapon, const FWeaponData&, CurrentNewWeaponData);
//--------------------------------------------------------------------------------------



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

    virtual void CreateChildActor() override;

    /** Инициализирует компонент до вызова в Игровом Процессе BeginPlay() Компонента и Актора-Владельца */
    virtual void InitializeComponent() override;

    /** Begins Play для Компонента */
    virtual void BeginPlay() override;
    //-------------------------------------------



    /* ---   Net   --- */

    // Создаваемый Каркас Оружия для вида со стороны
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Weapon Control|Net")
    TSubclassOf<AWeaponFrame> WeaponFrameType;

    //

    /** Получить Игрока-Владельца данного `UWeaponControlComponent`
    @note   Может быть НЕ Безопасно, предварительно проверяется в `BaseInit()` с вызовом соответствующей ошибки */
    FORCEINLINE APlayerCharacter* GetPlayerOwner() const
    {
        return (APlayerCharacter*)GetOwner();
    };

    /** Получить текущий Каркас Оружия
    @note   Может быть НЕ Безопасно, предварительно проверяется в `InitData()` с вызовом соответствующей ошибки */
    FORCEINLINE AWeaponFrame* GetCurrentWeaponFrame() const
    {
        return ChildWeaponFrame;
        /* @note    Не используем '(AWeaponFrame*)GetChildActor()', так как требуется
        отслеживание готовности Актора посредством репликации данного указателя */
    }

    /** Используется для регистрации реплицируемых Переменных */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Инициализировать под отображение от Первого Лица (скрыть Визуал, но оставить Тень) */
    void InitializeForFirstPersonDisplay();
    //-------------------------------------------



    /* ---   Data   --- */

    /** Получить данные текущего Оружия */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Control|Data",
        meta = (DisplayName = "Get Current Weapon Data"))
    const FWeaponData& BP_GetCurrentWeaponData() const { return *GetCurrentWeaponData(); };

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
    FORCEINLINE bool CheckAction(EActionVariations Action) const
    {
        return CheckActions((uint8)Action);
    };

    /** Проверка одного из действий Игрока
    @param  Action - Проверяемое действие */
    bool CheckActions(EActionVariations Action, ...) const;
    //-------------------------------------------



private:

    /* ---   Delegates   --- */

    /** OnStartChangingWeapon: Ретрансляцией на Сервер */
    UFUNCTION(Server, Reliable)
    void Server_OnStartChangingWeapon();

    /** OnStartChangingWeapon: Ретрансляцией для Всех */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OnStartChangingWeapon();
    //-------------------------------------------



    /* ---   Net   --- */

    // Указатель на создаваемый Актор Оружия
    UPROPERTY(ReplicatedUsing = OnRep_ChildWeaponFrame)
    AWeaponFrame* ChildWeaponFrame = nullptr;

    //

    /** При репликации: 'ChildWeaponFrame' */
    UFUNCTION()
    void OnRep_ChildWeaponFrame();
    //-------------------------------------------



    /* ---   Data   --- */

    // Массив указателей Данных Оружия, соответствующие его слоту
    TArray<FWeaponData*> WeaponDataSlots;

    // Указатель на Данные выбранного Оружия из Таблицы
    FWeaponData* CurrentWeaponData = nullptr;

    //

    /** Инициализация данных */
    void InitData();
    //-------------------------------------------



    /* ---   Actions | Data   --- */

    /** Метод изменения переменной CurrentActions через Сервер для её Репликации
    @mote   Принудительно Надёжный */
    UFUNCTION(Server, Reliable)
    void Server_SetCurrentActions(uint8 Value);

    /** Проверка действий Игрока
    @param  ActionsBits - Биты проверяемых действий */
    FORCEINLINE bool CheckActions(uint8 ActionsBits) const
    {
        return CurrentActions & ActionsBits;
    };

    /** Создание и выброс Астора согласно его Типу, Локации и Ротации
    @note   Более быстрый вызов для простого Актора */
    FORCEINLINE AActor* DropActor(const TSubclassOf<AActor>& ActorType, const FVector& Location, const FRotator& Rotation)
    {
        if (ActorType.Get())
        {
            return GetWorld()->SpawnActor(
                ActorType.Get(),
                &Location,
                &Rotation,
                SpawnParameters);
        }

        return nullptr;
    };

    /** Создание и выброс Наследника от Астора согласно его Подтипу, Локации и Ротации
    @note   Более быстрый вызов для наследника Актора */
    template<class T,
        class = std::enable_if_t<is_Actor<T>>>
    FORCEINLINE T* DropActor(const TSubclassOf<T>& ActorType, const FVector& Location, const FRotator& Rotation)
    {
        if (ActorType.Get())
        {
            return GetWorld()->SpawnActor<T>(
                ActorType.Get(),
                Location,
                Rotation,
                SpawnParameters);
        }

        return nullptr;
    };

    /** Воспроизвести Звук в указанной Локации */
    FORCEINLINE void PlaySound(USoundBase* Sound, FVector Location) const
    {
        if (Sound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Location);
        }
    };
    //-------------------------------------------



    /* ---   Actions | Switching   --- */

    /** Установить текущие данные оружия согласно Номеру */
    void SetCurrentWeaponDataByNum(uint8 Num);

    /** Server: Установить текущие данные оружия согласно Номеру
    @mote   Принудительно Надёжный */
    UFUNCTION(Server, Reliable)
    void Server_SetCurrentWeaponDataByNum(uint8 Num);

    /** Multicast: Установить текущие данные оружия согласно Номеру
    @mote   Принудительно Надёжный */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetCurrentWeaponDataByNum(uint8 Num);
    //-------------------------------------------



    /* ---   Actions | Shooting   --- */

    /** Выбросить Снаряд с указанными Локацией и Ротацией */
    void DropProjectile(const FVector& Location, const FRotator& Rotation);

    /** Server: Выбросить Снаряд с указанными Локацией и Ротацией */
    UFUNCTION(Server, Reliable)
    void Server_DropProjectile(const FVector& Location, const FRotator& Rotation);

    /** Multicast: Выбросить Снаряд с указанными Локацией и Ротацией */
    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_DropProjectile(const FVector& Location, const FRotator& Rotation);


    /** Провести Трассировку Снаряда с указанными Локациями и Визуализацией */
    void TraceProjectile(const FVector& StartLocation, const FVector& EndLocation);

    /** Провести Трассировку Снаряда с указанными Локациями и Визуализацией */
    UFUNCTION(Server, Reliable)
    void Server_TraceProjectile(const FVector& StartLocation, const FVector& EndLocation);

    /** Провести Трассировку Снаряда с указанными Локациями и Визуализацией */
    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_TraceProjectile(const FVector& StartLocation, const FVector& EndLocation);


    /** Метод Реакции на прикосновение снаряда */
    UFUNCTION()
    void OnProjectileHitForServer(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    /** Метод Реакции на прикосновение снаряда */
    UFUNCTION()
    void OnProjectileHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    /** Передача урона объекту по результатам касания (посредством коллизии или трейсов) */
    void ProjectileDamage(const FHitResult& Hit);


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



    /* ---   Statics   --- */

    // Параметр Принудительного создания выбрасываемых Акторов посредством метода DropActor()
    static const FActorSpawnParameters SpawnParameters;

    // Массив Типов Объектов, что отслеживаются Hitscan-методом
    static const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesForHitscan;
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

public:

    /* ---   Base: Debugs   --- */

    /** Вызывается, когда какое-либо свойство этого объекта было изменено
    @note   Используется для проверки изменённых переменных */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /** Альтернативная версия PostEditChange, вызываемая при изменении свойств внутри структур */
    //virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    //-------------------------------------------



private:

    /* ---   Net   --- */

    /** Получение наименований Сокетов текущего Меша в Mesh */
    UFUNCTION()
    TArray<FName> GetSocketNamesInMesh() const;
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};



/* ---   undef   --- */

#undef DELEGATE_METHOD_Broadcast_h
//--------------------------------------------------------------------------------------
