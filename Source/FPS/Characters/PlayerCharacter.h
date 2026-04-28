//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/Character.h"

// Interfaces:
#include "AbilitySystemInterface.h"
#include "FPS/Tools/Interfaces/Movement/SpeedControllerInterface.h"

// Structs:
#include "FPS/Tools/Structs/GameData/MatchStateData.h"
#include "FPS/Tools/Structs/Movement/SpeedControlData.h"

// Interaction:
#include "FPS/GAS/FPS_AbilitySystemComponent.h"

// Generated:
#include "PlayerCharacter.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UCameraComponent;

// Interaction:
class UFPS_CharacterMovementComponent;
class UWeaponLocalController;
class UWeaponNetworkController;

// Interaction | GAS:
class UFPS_AttributeSet;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface, public ISpeedControllerInterface
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Конструктор, используемый для подмены стандартного "UCharacterMovementComponent"
    APlayerCharacter(const FObjectInitializer& ObjectInitializer);
    //-------------------------------------------



    /* ---   Components   --- */

    /* Камера от Первого лица */
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FPCamera = nullptr;

    // Меш визуализации от Первого лица
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* FPMesh = nullptr;
    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    // Компонент сетевого контроллера Оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UWeaponNetworkController* WeaponControlNetComp = nullptr;

    // Компонент локального (не сетевого) контроллера Оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UWeaponLocalController* WeaponControlLocComp = nullptr;

    // Компонент Системы Способностей (GAS)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UFPS_AbilitySystemComponent* AbilitySystemComp = nullptr;

    // Скрытый Набор Атрибутов (для GAS)
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttributeSet,
        Category = "Components")
    UFPS_AttributeSet* AttributeSet = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    /** Вызывается при Запуске игры или при Спавне в уже запущенной игре */
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Вызывается, когда этот субъект явно уничтожается во время игрового процесса или в редакторе,
    * но не вызывается во время трансляции уровней или завершения игрового процесса */
    virtual void Destroyed() override;

    /** Вызывается, когда истекает срок службы актера (если он у него есть). */
    virtual void LifeSpanExpired() override;

    /** Реакция на Выпадение из Мира (ниже KillZ и тому подобного) */
    virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

    /** Функция, вызываемая каждый кадр в этом Акторе, если не назначена другая частота */
    //virtual void Tick(float DeltaSeconds) override;

    /** Вызывается перед инициализацией всех компонентов */
    virtual void PreInitializeComponents() override;

    /** Вызывается после инициализации всех компонентов только во время игрового процесса */
    //virtual void PostInitializeComponents() override;

    /** Вызывается при подключения Контроллера
    @note   Вызывается только на сервере (или в автономном режиме)
    @param  NewController - Контроллер, захвативший владение данным Игроком (Пешкой) */
    virtual void PossessedBy(AController* NewController) override;

    /** Получить параметры Коллизии для Трассировки */
    FORCEINLINE const FCollisionQueryParams& GetCollisionParamsForTrace() const
    {
        return CollisionParamsForTrace;
    };
    //-------------------------------------------



    /* ---   Visualization   --- */

    /* Скрываемая Кость в FPMesh */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Player Character|Visualization",
        meta = (GetOptions = "GetBoneNamesInFPMesh",
            DisplayName = "Hidden Bone In FPMesh"))
    FName HiddenBoneInFPMesh = NAME_None;
    //-------------------------------------------



    /* ---   Net   --- */

    /** Метод, позволяющий субъекту реплицировать подобъекты в своем Актор-канале */
    //virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



    /* ---   Net | OnRep   --- */

    /** При Репликации: PlayerState */
    virtual void OnRep_PlayerState() override;

    /** При Репликации: AttributeSet */
    UFUNCTION()
    virtual void OnRep_AttributeSet();
    //-------------------------------------------



    /* ---   Inputs   --- */

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    //-------------------------------------------



    /* ---   Inputs | Actions   --- */

    // Группа Действий для "Прыжка"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Player Character|Inputs|Actions",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Jump"))
    FName ActionGroups_Jump = NAME_None;

    // Группа Действий для "Спринта"
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Player Character|Inputs|Actions",
        meta = (GetOptions = "GetActionGroupsNames",
            DisplayName = "Sprint"))
    FName ActionGroups_Sprint = NAME_None;
    //-------------------------------------------



    /* ---   Inputs | Axis   --- */

    // Группа Осей для "Движения Вперёд" (вперёд-назад)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Player Character|Inputs|Axis",
        meta = (GetOptions = "GetAxisGroupsNames",
            DisplayName = "Move Forward"))
    FName AxisGroups_MoveForward = NAME_None;

    // Группа Осей для "Движения Вправо" (вправо-влево)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Player Character|Inputs|Axis",
        meta = (GetOptions = "GetAxisGroupsNames",
            DisplayName = "Move Right"))
    FName AxisGroups_MoveRight = NAME_None;

    // Группа Осей для "Обзора Поворотом" (вправо-влево)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Player Character|Inputs|Axis",
        meta = (GetOptions = "GetAxisGroupsNames",
            DisplayName = "Turn"))
    FName AxisGroups_Turn = NAME_None;

    // Группа Осей для "Обзора по Вертикали" (вверх-вниз)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Player Character|Inputs|Axis",
        meta = (GetOptions = "GetAxisGroupsNames",
            DisplayName = "Look Up"))
    FName AxisGroups_LookUp = NAME_None;
    //-------------------------------------------



    /* ---   Movement Speed   --- */

    /* Скорость Ходьбы (лёгкого бега) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Player Character|Movement Speed")
    float WalkSpeed = 600;

    // Скорость Спринта
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Player Character|Movement Speed")
    float SprintSpeed = 800;

    //

    /** Возвращает подобъект 'CharacterMovement', приведённый к типу "UFPS_CharacterMovementComponent" */
    UFUNCTION(BlueprintPure,
        Category = "Player Character|Movement Speed",
        meta = (DisplayName = "Get FPS Character Movement"))
    virtual UFPS_CharacterMovementComponent* GetFPSCharacterMovement() const
    {
        // Текущий 'CharacterMovement' гарантированно является компонентом
        // типа "UFPS_CharacterMovementComponent" (см. конструктор класса)
        return (UFPS_CharacterMovementComponent*)GetCharacterMovement();
    };

    /** Задать значение скорости */
    UFUNCTION(BlueprintCallable,
        Category = "Player Character|Movement Speed")
    void SetSpeedControl(ESpeedVariations Mode) override;
    //-------------------------------------------



    /* ---   GAS   --- */

    /** Возвращает Компонент Системы Способностей данного Игрока */
    FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
    {
        return AbilitySystemComp;
    };
    //-------------------------------------------



    /* ---   Events   --- */

    /** Событие BP: При Инициализации Локального Контроллера */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Player Character|Visualization",
        meta = (DisplayName = "On Local Controller Initialization"))
    void Event_OnLocalControllerInitialization();
    //-------------------------------------------



    /* ---   GAS Events   --- */

    /** Событие BP: Изменение Здоровья */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Health"))
    void Event_ChangingHealth(float Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(Health);

    /** Событие BP: Изменение максимального Здоровья */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Max Health"))
    void Event_ChangingMaxHealth(float Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(MaxHealth);

    /** Событие BP: Изменение Брони */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Armor"))
    void Event_ChangingArmor(float Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(Armor);

    /** Событие BP: Изменение максимальной Брони */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Max Armor"))
    void Event_ChangingMaxArmor(float Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(MaxArmor);

    /** Событие BP: При Нулевом Здоровье */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "On Zero Health"))
    void Event_OnZeroHealth();

    /** Событие BP: При Нулевой Броне */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "On Zero Armor"))
    void Event_OnZeroArmor();
    //-------------------------------------------



private:

    /* ---   Base   --- */

    // Параметры Коллизии для Трассировки
    FCollisionQueryParams CollisionParamsForTrace;

    //

    /** Инициализация при Локальном контроллере */
    void InitForLocally();

    /** Инициализация при Сетевом контроллере */
    void InitForNetwork();
    //-------------------------------------------



    /* ---   Inputs   --- */

    /** Ввод передвижения: вперёд-назад */
    void MoveForward(float Value);

    /** Ввод передвижения: вправо-влево */
    void MoveRight(float Value);

    /** Скорость передвижения: Спринт */
    void Sprint();

    /** Скорость передвижения: Ходьба (лёгкий бег) */
    void StopSprint();

    /** Управление входными сигналами (вводом) основываясь на состояние матча */
    UFUNCTION()
    void ControlInputsBasedOnMatchStatus(EMatchState NewMatchState);
    //-------------------------------------------



    /* ---   Movement Speed   --- */

    // Переменная контроля максимальной Скорости Персонажа
    ESpeedVariations SpeedControl = ESpeedVariations::Sprint;

    /* Биты команд и состояний Спринта
    @note   b0 - Команда: Включить спринт
    @note   b1 - Состояние: Включён спринт  */
    uint8 IsSprinting = 0;

    //

    /** Инициализация контроля Скорости */
    void InitSpeedControl() override;
    //-------------------------------------------



    /* ---   GAS   --- */

    /** Инициализация данных AbilitySystemComp */
    void InitAbilitySystemComp();
    //-------------------------------------------



    /* ---   GAS Events   --- */

    /** Событие: При Нулевом Здоровье */
    UFUNCTION()
    void OnZeroHealth();

    /** При смерти Игрока */
    void AsPlayerDies();
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Base   --- */

    /** Получение наименований Костей текущего Меша в FPMesh */
    UFUNCTION()
    TArray<FName> GetBoneNamesInFPMesh() const;
    //-------------------------------------------



    /* ---   Inputs   --- */

    /* Получить имена зарегистрированных Групп Действий */
    UFUNCTION()
    TArray<FName> GetActionGroupsNames();

    /* Получить имена зарегистрированных Групп Осей */
    UFUNCTION()
    TArray<FName> GetAxisGroupsNames();

    /* Проверить группы входных данных */
    void CheckInputsGroups();
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};
