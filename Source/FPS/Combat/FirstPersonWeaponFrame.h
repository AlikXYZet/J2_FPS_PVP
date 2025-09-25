//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "WeaponFrame.h"

// Generated:
#include "FirstPersonWeaponFrame.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UArrowComponent;
class UCameraComponent;

// Actor Components:
class USmoothMovementComponent;
class USmoothRotationComponent;

// Interaction:
class UWeaponLocalController;
class AProjectile;
class APlayerCharacter;
class AFPS_PlayerController;
//--------------------------------------------------------------------------------------



UCLASS(Abstract)
class FPS_API AFirstPersonWeaponFrame : public AWeaponFrame
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this character's properties
    AFirstPersonWeaponFrame();
    //-------------------------------------------



    /* ---   Components   --- */

    // Направляющая Выстрела (красный)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Arrows",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* ShootGuidance = nullptr;

    // Направляющая вылета Гильзы (синий)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Arrows",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* CaseDropGuidance = nullptr;

    // Направляющая выпадения Накопителя (зелёный): Магазина, Обоймы и т.п.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Arrows",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* StorageDropGuidance = nullptr;

    // Точка Схвата оружия Правой рукой
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Points",
        meta = (AllowPrivateAccess = "true"))
    USceneComponent* GripPoint;

    // Точка Удержания оружия Левой рукой
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Points",
        meta = (AllowPrivateAccess = "true"))
    USceneComponent* HoldingPoint;
    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    // Компонент плавного Перемещения
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Controls",
        meta = (AllowPrivateAccess = "true"))
    USmoothMovementComponent* SmoothMovementComponent = nullptr;

    // Компонент плавного Вращения
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Controls",
        meta = (AllowPrivateAccess = "true"))
    USmoothRotationComponent* SmoothRotationComponent = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    // Function called every frame on this Actor
    virtual void Tick(float DeltaSeconds) override;
    //-------------------------------------------



    /* ---   Control   --- */

    // Имя Сокета для Точка Схвата оружия Правой рукой
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "Components|Points",
        meta = (GetOptions = "GetSocketNamesInSkeletalMesh"))
    FName GripSocketName = NAME_None;

    // Имя Сокета для Точка Удержания оружия Левой рукой
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "Components|Points",
        meta = (GetOptions = "GetSocketNamesInSkeletalMesh"))
    FName HoldingSocketName = NAME_None;
    //-------------------------------------------



    /* ---   Data   --- */

    /** Обновить Оружие по Данным */
    virtual void UpdateWeaponOnSelectedData(const FWeaponData* Data) override;
    //-------------------------------------------



private:

    /* ---   Control   --- */

    // Игрок-Владелец данного Оружия
    APlayerCharacter* ParentPlayerCharacter = nullptr;
    //-------------------------------------------



    /* ---   Direction Fire   --- */

    // Контроллер Игрока-Владельца данного Оружия
    AFPS_PlayerController* ParentPlayerController = nullptr;

    // Параметры Коллизии для Трассировки
    FCollisionQueryParams CollisionParamsForTrace;

    //

    /* Инициализация данных Направления Огня данного оружия */
    void InitDirectionFireData();
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

private:

    /* ---   Control   --- */

    /** Получение наименований Костей текущего Меша в FPMesh */
    UFUNCTION()
    TArray<FName> GetSocketNamesInSkeletalMesh() const;
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};
