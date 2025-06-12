//

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

    /* ---   Specifications   --- */

    /* Максимальное Количество подготовленных Патронов в чём-либо (в магазине, обойме и т.п.) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    int32 MaxPreparedCartridges = 0;

    // Количество заряжаемых Патронов
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    uint8 LoadedCartridgesNum = 1;
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
        meta = (DisplayAfter = "ShootGuidanceTransform"))
    TSubclassOf<AProjectile> ProjectileType;

    // Тип выпадающей Гильзы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (DisplayAfter = "CaseDropGuidanceTransform"))
    TSubclassOf<UObject> CaseDropType;

    // Тип выпадающего Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (DisplayAfter = "StorageDropGuidanceTransform"))
    TSubclassOf<UObject> StorageDropType;

    // Трансформация Направляющей Выстрела
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping")
    FTransform ShootGuidanceTransform;

    // Трансформация Направляющей вылета Гильзы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping")
    FTransform CaseDropGuidanceTransform;

    // Трансформация Направляющей выпадения Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping")
    FTransform StorageDropGuidanceTransform;
    //-------------------------------------------


    /* ---   Animations   --- */

    /** От Бедра */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Simple")
    UAnimMontage* FromHip = nullptr;

    // Прицеливание
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Simple")
    UAnimMontage* Aiming = nullptr;

    // Перезарядка от Бедра
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Reloading")
    UAnimMontage* Reload = nullptr;

    // Перезарядка при Прицеливании
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Reloading")
    UAnimMontage* ReloadWhenAiming = nullptr;

    // Убрать Оружие
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Changing")
    UAnimMontage* RemoveWeapon = nullptr;

    // Убрать Оружие
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Changing")
    UAnimMontage* TakeWeapon = nullptr;
    //-------------------------------------------


    /* ---   Animations | Timer   --- */

    // Время таймера в секундах
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Reloading",
        meta = (DisplayAfter = "ReloadWhenAiming", Units = "s"))
    float ReloadTime = 2.f;

    // Время таймера в секундах
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Changing",
        meta = (DisplayAfter = "RemoveWeapon", Units = "s"))
    float RemoveWeaponTime = 1.f;

    // Время таймера в секундах
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations: Changing",
        meta = (DisplayAfter = "TakeWeapon", Units = "s"))
    float TakeWeaponTime = 1.f;
    //-------------------------------------------


    /* ---   Constructors   --- */

    FWeaponData() {};
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
