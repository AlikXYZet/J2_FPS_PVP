//

#pragma once

// Core:
#include "CoreMinimal.h"

// UE:
#include "Engine/DataTable.h"

// Structs:
#include "FPS/Tools/Structs/Animations/PersonAnimData.h"
#include "FPS/Tools/Structs/Animations/WeaponAnimData.h"

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

    /* ---   Specifications   --- */

    /* Максимальное Количество хранимых Патронов в запасе */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    int32 MaxStoredCartridges = 0;

    /* Максимальное Количество подготовленных Патронов в чём-либо (в магазине, обойме и т.п.) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    int32 MaxPreparedCartridges = 0;
    //-------------------------------------------



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

    // Трансформация Меша Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform MeshTransform;

    // Локация Оружия в положении от Бедра
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FVector HipLocation = FVector::ZeroVector;

    // Локация Оружия при Прицеливании
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FVector AimingLocation = FVector::ZeroVector;
    //-------------------------------------------



    /* ---   Dropping   --- */

    // Тип используемого Снаряда
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (DisplayAfter = "ProjectileGuidanceTransform"))
    TSubclassOf<AProjectile> ProjectileType;

    // Тип выпадающей Гильзы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (DisplayAfter = "SleeveGuidanceTransform"))
    TSubclassOf<AActor> SleeveType;

    // Тип выпадающего Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (DisplayAfter = "StorageGuidanceTransform"))
    TSubclassOf<AActor> StorageType;

    // Трансформация Направляющей Выстрела
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping")
    FTransform ProjectileGuidanceTransform;

    // Трансформация Направляющей вылета Гильзы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping")
    FTransform SleeveGuidanceTransform;

    // Трансформация Направляющей выпадения Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping")
    FTransform StorageGuidanceTransform;
    //-------------------------------------------



    /* ---   Person Animations   --- */

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations",
        meta = (ShowOnlyInnerProperties))
    FPersonAnimData PersonAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Shooting")
    float ShootingWeapon_Time = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Reloading")
    float ReloadingWeapon_Time = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Changing")
    float RemoveWeapon_Time = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Changing")
    float TakeWeapon_Time = 2.f;
    //-------------------------------------------



    /* ---   Weapon Animations   --- */

    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Animations",
        meta = (ShowOnlyInnerProperties))
    FWeaponAnimData WeaponAnimations;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
