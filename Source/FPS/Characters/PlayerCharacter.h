//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// Generated:
#include "PlayerCharacter.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При Новом режиме перемещения
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewAimingState, bool, Aiming);
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UCameraComponent;

// Interaction:
class UFPS_CharacterMovementComponent;
class UWeaponSlotsComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    // Делегат: При Новом режиме перемещения
    UPROPERTY(BlueprintAssignable)
    FOnNewAimingState OnNewAimingState;
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this character's properties
    APlayerCharacter(const FObjectInitializer& ObjectInitializer);
    //-------------------------------------------



    /* ---   Components   --- */

    // Компонент Слотов Оружия и взаимодействия с ним
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UWeaponSlotsComponent* WeaponSlotsComponent = nullptr;

    // Компонент дочернего Актора Оружия
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UChildActorComponent* ChildWeaponActor;
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



    /* ---   Base   --- */

    // Кость, после которой прячем отображение в Меше
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Player Character|Components",
        meta = (GetOptions = "GetBoneNamesInMesh"))
    FName HiddenBone = "NONE";
    //-------------------------------------------



    /* ---   Inputs   --- */

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Ввод передвижения: вперёд-назад */
    void MoveForward(float Value);

    /** Ввод передвижения: вправо-влево */
    void MoveRight(float Value);

    /** Ввод стрельбы */
    void OnFire();

    /** Начать Прицеливание */
    void Aiming();

    /** Прекратить Прицеливание */
    void StopAiming();
    //-------------------------------------------


private:

    /* ---   Base   --- */

    /** Очистка от неиспользуемых компонентов */
    void Cleaning();

    /** Получение наименований Сокетов текущего Меша */
    UFUNCTION()
    TArray<FName> GetBoneSocketsInMesh() const;

    /** Получение наименований Костей текущего Меша */
    UFUNCTION()
    TArray<FName> GetBoneNamesInMesh() const;
    //-------------------------------------------
};
