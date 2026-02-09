//

// Base:
#include "StaticMeshAssetActionUtility.h"
//--------------------------------------------------------------------------------------



/* ---   Static Mesh   --- */

bool UStaticMeshAssetActionUtility::SetSimpleCollisionPhysicalMaterial(UStaticMesh* StaticMesh, UPhysicalMaterial* PhysMaterial)
{
    if (StaticMesh
        && StaticMesh->GetBodySetup())
    {
        StaticMesh->GetBodySetup()->PhysMaterial = PhysMaterial;
        StaticMesh->MarkPackageDirty();
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------
