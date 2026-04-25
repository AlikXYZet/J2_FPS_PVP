//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "FPS/Interactive/PickableActor.h"

// Generated:
#include "PickableAmmo.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APickableAmmo : public APickableActor
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this actor's properties
    APickableAmmo() {};
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Interactive | Data   --- */

    /** Наименование оружия, которому требуется предоставить патроны */
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Interactive|Data",
        meta = (GetOptions = "GetAllWeaponsNames", DisplayAfter = "WeaponSelectionDataTable"))
    FName WeaponType = NAME_None;

    // Количество добавляемых Патронов
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "Interactive|Data")
    int32 NumberOfCartridges = 10;
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITORONLY_DATA

private:

    /* ---   Interactive | Data   --- */

    /** Таблица данных, для выбора Оружия из списка */
    UPROPERTY(EditDefaultsOnly,
        Category = "Interactive|Data",
        meta = (RequiredAssetDataTags = "RowStructure=WeaponData"))
    UDataTable* WeaponSelectionDataTable = nullptr;

    //

    /** Получение Названия строк из таблицы 'Weapon Selection Data Table' */
    UFUNCTION()
    TArray<FName> GetAllWeaponsNames() const;
    //-------------------------------------------

#endif // WITH_EDITORONLY_DATA
    //===========================================
};
