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
class AFirstPersonWeaponFrame;
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

    // Образец Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear,
        Category = "Visualization",
        meta = (BlueprintBaseOnly))
    TSubclassOf<AFirstPersonWeaponFrame> WeaponTemplate;

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear,
        Category = "Dropping",
        meta = (BlueprintBaseOnly))
    TSubclassOf<AProjectile> ProjectileType;

    // Тип выпадающей Гильзы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (BlueprintBaseOnly, ShowTreeView, OnlyPlaceable))
    TSubclassOf<AActor> SleeveType;

    // Тип выпадающего Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (BlueprintBaseOnly, ShowTreeView, OnlyPlaceable))
    TSubclassOf<AActor> StorageType;
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
