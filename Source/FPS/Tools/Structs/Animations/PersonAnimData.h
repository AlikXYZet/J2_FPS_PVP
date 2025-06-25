//

#pragma once

// Core:
#include "CoreMinimal.h"

// UE:
#include "Animation/BlendSpaceBase.h"

// Generated:
#include "PersonAnimData.generated.h"
//--------------------------------------------------------------------------------------



/* Структура данных изменяемых Элементов Анимации Игрока */
USTRUCT(BlueprintType)
struct FPersonAnimData
{
    GENERATED_BODY()

    /* ---   Person Animations | Idle   --- */

    /** Blend Space: Праздный (Смешивания взора Вверх-Вниз и состояния Прицеливания) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Idle")
    UBlendSpaceBase* BS_Idle = nullptr;
    //-------------------------------------------



    /* ---   Person Animations | Shooting   --- */

    /** Anim Montage: Стрельба от бедра */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Shooting")
    UAnimMontage* AM_HipShooting = nullptr;

    /** Anim Montage: Прицельная Стрельба */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Shooting")
    UAnimMontage* AM_AimShooting = nullptr;
    //-------------------------------------------



    /* ---   Person Animations | Reloading   --- */

    /** Blend Space: Перезарядка */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Reloading")
    UBlendSpaceBase* BS_Reloading = nullptr;
    //-------------------------------------------



    /* ---   Person Animations | Changing   --- */

    /** Anim Montage: Убрать Оружие */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Changing")
    UAnimMontage* AM_RemoveWeapon = nullptr;

    /** Anim Montage: Взять Оружие */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Changing")
    UAnimMontage* AM_TakeWeapon = nullptr;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
