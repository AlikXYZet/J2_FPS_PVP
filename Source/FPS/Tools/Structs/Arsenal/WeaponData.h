#pragma once

// Base:
#include "CoreMinimal.h"

// UE:
#include "Engine/DataTable.h"

// Generated:
#include "WeaponData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class AProjectile;
//--------------------------------------------------------------------------------------



/* Структура данных Оружия:
визуализация, анимации и характеристики
*/
USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_BODY()


    /* ---   Visualization   --- */

    /* Скелетный Меш Оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization",
        meta = (EditCondition = "StaticMesh==nullptr"))
    USkeletalMesh* SkeletalMesh = nullptr;

    // Статический Меш Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization",
        meta = (EditCondition = "SkeletalMesh==nullptr"))
    UStaticMesh* StaticMesh = nullptr;

    // Тип используемого Снаряда
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    TSubclassOf<AProjectile> ProjectileType;

    // Трансформация Меша Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform MeshTransform;

    // Трансформация Направляющей Выстрела
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform ShootGuidanceTransform;

    // Трансформация Направляющей вылета Гильзы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform CaseDropGuidanceTransform;

    // Трансформация Направляющей выпадения Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform StorageDropGuidanceTransform;

    // Локация Оружия в положении от Бедра
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FVector HipLocation = FVector::ZeroVector;

    // Локация Оружия при Прицеливании
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FVector AimingLocation = FVector::ZeroVector;
    //-------------------------------------------


    /* ---   Animations   --- */

    /** От Бедра */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* FromHip = nullptr;

    // Прицеливание
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* Aiming = nullptr;

    // Перезарядка от Бедра
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* Reload = nullptr;

    // Перезарядка при Прицеливании
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* ReloadWhenAiming = nullptr;
    //-------------------------------------------


    /* ---   Specifications   --- */

    /* Оружие заряжаемое? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    bool bIsLoadable = true;

    // Максимальное Количество подготовленных Патронов в чём-либо (в магазине, обойме и т.п.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    int32 MaxPreparedCartridges = 0;
    //-------------------------------------------


    /* ---   Constructors   --- */

    FWeaponData() {};
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
