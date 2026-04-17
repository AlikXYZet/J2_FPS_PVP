//

#pragma once

// Core:
#include "CoreMinimal.h"

// Generated:
#include "WeaponSlotData.generated.h"
//--------------------------------------------------------------------------------------



/* Структура данных слота:
Оружие и его расходники
*/
USTRUCT(BlueprintType)
struct FWeaponSlotData
{
    GENERATED_BODY()

    /* ---   Constructors   --- */

    FWeaponSlotData() {};
    FWeaponSlotData(const FName& iWeaponType) : WeaponType(iWeaponType) {};
    //-------------------------------------------


    /* ---   Slot Data   --- */

    // Тип Оружия, занимаемый данный Слот
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data",
        meta = (GetOptions = "GetAllWeaponsNames"))
    FName WeaponType = NAME_None;

    // Количество всех Патронов без учёта подготовленных
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data")
    int32 NumAllCartridge = 60;

    // Количество подготовленных Патронов в чём-либо (в магазине, обойме и т.п.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data")
    int32 NumPreparedCartridges = 30;

    // Заряжено ли оружие?
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Slot Data")
    bool bIsWeaponLoaded = false;
    //-------------------------------------------


    /* ---   Operators | ==   --- */

    FORCEINLINE bool operator==(const FWeaponSlotData& Second) const
    {
        return WeaponType == Second.WeaponType;
    }

    FORCEINLINE bool operator==(const FName& SecondWeaponType) const
    {
        return WeaponType == SecondWeaponType;
    }
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------
