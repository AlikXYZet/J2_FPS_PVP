//

#pragma once

// Core:
#include "CoreMinimal.h"

// Generated:
#include "WeaponAnimData.generated.h"
//--------------------------------------------------------------------------------------



/* Структура данных изменяемых Элементов Анимации Игрока */
USTRUCT(BlueprintType)
struct FWeaponAnimData
{
    GENERATED_BODY()

    /* ---   Person Animations   --- */

    // Убрать Оружие
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Animations")
    UAnimMontage* A_WeaponReloading = nullptr;

    // Убрать Оружие
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Animations")
    UAnimMontage* A_WeaponFiring = nullptr;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
