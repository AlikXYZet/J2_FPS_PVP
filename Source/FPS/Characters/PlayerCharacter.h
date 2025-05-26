//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// Generated:
#include "PlayerCharacter.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UCameraComponent;

// Interaction:
class UFPS_CharacterMovementComponent;
class UWeaponControlComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APlayerCharacter : public ACharacter
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

    // Компонент Слотов Оружия и взаимодействия с самим Оружием
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UWeaponControlComponent* WeaponControl = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Components   --- */

    /** Возвращает подобъект CharacterMovement, приведённый к типу UFPS_CharacterMovementComponent */
    UFUNCTION(BlueprintPure,
        Category = "Player Character")
    virtual UFPS_CharacterMovementComponent* GetFPSCharacterMovement() const;
    //-------------------------------------------



    /* ---   Net   --- */

    /** Метод, позволяющий субъекту реплицировать подобъекты в своем Актор-канале */
    virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



    /* ---   Inputs   --- */

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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

    /** Изменить значение максимальной скорости */
    void SetMaxWalkSpeed(const float& Value);

    /** Обновить значение максимальной скорости через Сервер */
    UFUNCTION(Server, Reliable)
    void Server_SetMaxWalkSpeed(const float& Value);

    /** Обновить значение максимальной скорости у Всех, кроме владельца */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetMaxWalkSpeed(const float& Value);
    //-------------------------------------------



private:

    /* ---   Base   --- */

    /** Очистка от неиспользуемых компонентов */
    void Cleaning();
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

    /* Биты команд и состояний Спринта
    @note   b0 - Команда: Включить спринт
    @note   b1 - Состояние: Включён спринт  */
    uint8 IsSprinting = 0;
    //-------------------------------------------
};
