//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Blutility/Classes/AssetActionUtility.h"

// Generated:
#include "StaticMeshAssetActionUtility.generated.h"
//--------------------------------------------------------------------------------------



UCLASS(Abstract, HideCategories = Object, Blueprintable)
class EDITORSCRIPTINGUTILITIESEXPANSION_API UStaticMeshAssetActionUtility : public UAssetActionUtility
{
    GENERATED_BODY()

public:

    /* ---   Static Mesh   --- */

    /** Возвращает класс, который поддерживает данное действие с активом
    (если оно не реализовано, оно будет отображаться для всех типов активов) */
    UClass* GetSupportedClass_Implementable() const { return UStaticMesh::StaticClass(); };

    /** Установить Физику Материала для указанного Ассета типа "UStaticMesh"
    @param  StaticMesh -- Ассет типа "UStaticMesh", для которого требуется сменить Физику Материала
    @param  PhysMaterial -- Физика Материала для данного Ассета */
    UFUNCTION(BlueprintCallable, CallInEditor,
        Category = "Asset Action Utility|Static Mesh",
        meta = (DefaultToSelf, HideSelfPin = true))
    static bool SetSimpleCollisionPhysicalMaterial(UStaticMesh* StaticMesh, UPhysicalMaterial* PhysMaterial);
    //-------------------------------------------
};
