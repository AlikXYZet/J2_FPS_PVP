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

// GAS:
class UGameplayEffect;

// Interaction:
class AProjectile;
class AFirstPersonWeaponFrame;
class UParticleSystem;
class UNiagaraSystem;
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

    /* Образец Оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear,
        Category = "Visualization",
        meta = (BlueprintBaseOnly = ""))
    TSubclassOf<AFirstPersonWeaponFrame> WeaponTemplate;

    // Иконка отображения Типа Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear,
        Category = "Visualization",
        meta = (BlueprintBaseOnly = ""))
    UTexture2D* WeaponIcon = nullptr;

    // Локация Оружия в положении от Бедра
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FVector HipLocation = FVector::ZeroVector;

    // Локация Оружия при Прицеливании
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FVector AimingLocation = FVector::ZeroVector;
    //-------------------------------------------



    /* ---   Audialization   --- */

    // Звук Стрельбы Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Audialization")
    USoundBase* ShootingSound = nullptr;

    // Звук Перезарядки Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Audialization")
    USoundBase* ReloadingSound = nullptr;

    // Звук Снятия (убирания) Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Audialization")
    USoundBase* RemoveSound = nullptr;

    // Звук Поднятия Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Audialization")
    USoundBase* TakeSound = nullptr;
    //-------------------------------------------



    /* ---   Dropping   --- */

    /* Тип выпадающей Гильзы */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (BlueprintBaseOnly = "", ShowTreeView, OnlyPlaceable))
    TSubclassOf<AActor> SleeveType;

    // Тип выпадающего Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping",
        meta = (BlueprintBaseOnly = "", ShowTreeView, OnlyPlaceable))
    TSubclassOf<AActor> StorageType;
    //-------------------------------------------



    /* ---   Dropping: Projectile   --- */

    /* Эффект поражения данным снарядом */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear,
        Category = "Dropping: Projectile",
        meta = (BlueprintBaseOnly = ""))
    TArray<TSubclassOf<UGameplayEffect>> DamageEffects;

    // Тип используемого Снаряда
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NoClear,
        Category = "Dropping: Projectile",
        meta = (BlueprintBaseOnly = "", EditCondition = "!bUseHitscanMethod", DisplayAfter = "bUseHitscanMethod"))
    TSubclassOf<AProjectile> ProjectileType;

    // FX Трассировки снаряда
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping: Projectile",
        meta = (BlueprintBaseOnly = "", ShowTreeView, OnlyPlaceable, EditCondition = "bUseHitscanMethod", DisplayAfter = "bUseHitscanMethod"))
    TSubclassOf<AActor> FXTracer = nullptr;

    // Звук Попадания Снаряда или Перекрытия 'Hitscan'
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping: Projectile")
    USoundBase* HitSound = nullptr;
    //-------------------------------------------



    /* ---   Person Animations   --- */

    /* Анимации Игрока при использовании Оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations",
        meta = (ShowOnlyInnerProperties))
    FPersonAnimData PersonAnimations;

    // Время между Выстрелами Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Shooting",
        meta = (ForceUnits = Seconds, ClampMin = "0", UIMin = "0"))
    float ShootingWeapon_Time = 0.2f;

    // Время Перезарядки Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Reloading",
        meta = (ForceUnits = Seconds, ClampMin = "0", UIMin = "0"))
    float ReloadingWeapon_Time = 2.f;

    // Время Снятия (убирания) Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Changing",
        meta = (ForceUnits = Seconds, ClampMin = "0", UIMin = "0"))
    float RemoveWeapon_Time = 2.f;

    // Время Поднятия Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Changing",
        meta = (ForceUnits = Seconds, ClampMin = "0", UIMin = "0"))
    float TakeWeapon_Time = 2.f;
    //-------------------------------------------



    /* ---   Weapon Animations   --- */

    /* Анимации Оружия при использовании */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Animations",
        meta = (ShowOnlyInnerProperties))
    FWeaponAnimData WeaponAnimations;
    //-------------------------------------------



    /* ---   Booleanas   --- */

    // Флаг: Использовать ли Hitscan-метод расчёта попадания
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Dropping: Projectile")
    uint8 bUseHitscanMethod : 1;

    // Флаг: Заряжаемо ли Оружие
    // Например: возможность +1 патрон в патроннике
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    uint8 bIsWeaponChargeable : 1;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
