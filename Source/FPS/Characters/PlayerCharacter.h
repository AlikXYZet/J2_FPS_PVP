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
#include "FPS/Tools/Structs/Movement/SpeedControlData.h"

// Interaction:
#include "FPS/GAS/FPS_AbilitySystemComponent.h"

// Generated:
#include "PlayerCharacter.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** Макрос: Создание функции делегата для передачи значения атрибутов GAS через Событие BP */
#define GAMEPLAYATTRIBUTE_VALUE_HandleChanged(PropertyName) \
	FORCEINLINE void Handle##PropertyName##Changed(const FOnAttributeChangeData& Data) \
	{ \
		Event_Changing##PropertyName(Data.NewValue); \
	}
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
    UPROPERTY(BlueprintReadOnly,
        Category = "Components")
    UFPS_AttributeSet* AttributeSet = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Вызывается при подключения Контроллера
    @note   Вызывается только на сервере (или в автономном режиме)
    @param  NewController - Контроллер, захвативший владение данным Игроком (Пешкой)
    */
    virtual void PossessedBy(AController* NewController) override;
    //-------------------------------------------



    /* ---   Components   --- */

    /** Возвращает подобъект CharacterMovement, приведённый к типу UFPS_CharacterMovementComponent */
    UFUNCTION(BlueprintPure,
        Category = "Player Character")
    virtual UFPS_CharacterMovementComponent* GetFPSCharacterMovement() const;
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
    virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



    /* ---   Net | OnRep   --- */

    /** Функция обратного вызова: PlayerState */
    virtual void OnRep_PlayerState() override;
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

    /** Задать значение скорости */
    UFUNCTION(BlueprintCallable,
        Category = "Player Character|Movement Speed",
        meta = (AutoCreateRefTerm = "Mode"))
    void SetSpeedControl(const ESpeedVariations& Mode) override;
    //-------------------------------------------



    /* ---   GAS   --- */

    /** Возвращает Компонент Системы Способностей данного Игрока */
    FORCEINLINE virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
    {
        return AbilitySystemComp;
    };
    //-------------------------------------------



    /* ---   GAS Events   --- */

        /** Событие BP: Изменение Здоровья */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Health"))
    void Event_ChangingHealth(const float& Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(Health);

    /** Событие BP: Изменение максимального Здоровья */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Max Health"))
    void Event_ChangingMaxHealth(const float& Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(MaxHealth);

    /** Событие BP: Изменение Брони */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Armor"))
    void Event_ChangingArmor(const float& Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(Armor);

    /** Событие BP: Изменение максимальной Брони */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "Changing Max Armor"))
    void Event_ChangingMaxArmor(const float& Data);
    GAMEPLAYATTRIBUTE_VALUE_HandleChanged(MaxArmor);

    /** Событие BP: При Нулевом Здоровье */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Gameplay Ability System|Events",
        meta = (DisplayName = "On Zero Health"))
    void Event_OnZeroHealth();
    //-------------------------------------------



private:

    /* ---   Base   --- */

    /** Очистка от неиспользуемых компонентов */
    void Cleaning();

    /** Исправление Привязки дочернего актора
    @note   На стороне клиента слетает привязанность Дочернего Актора не смотря на то,
            что данный компонент уже привязан к нужному сокету. Поэтому исправляем данным кодом */
    void CorrectingAttachmentChildActor();
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
