//

// Base:
#include "PickableAmmo.h"

// UE:
#include "Engine/DataTable.h"

// Structs:
#include "FPS/Tools/Structs/Properties/ComponentRendering.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void APickableAmmo::BeginPlay()
{
    Super::BeginPlay();
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Interactive   --- */

TArray<FName> APickableAmmo::GetAllWeaponsNames() const
{
    if (WeaponSelectionDataTable)
    {
        return WeaponSelectionDataTable->GetRowNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
