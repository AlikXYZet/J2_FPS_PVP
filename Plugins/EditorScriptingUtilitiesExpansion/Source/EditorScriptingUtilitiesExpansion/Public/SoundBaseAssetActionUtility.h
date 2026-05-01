//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Blutility/Classes/AssetActionUtility.h"

// Generated:
#include "SoundBaseAssetActionUtility.generated.h"
//--------------------------------------------------------------------------------------



UCLASS(Abstract, HideCategories = Object, Blueprintable)
class EDITORSCRIPTINGUTILITIESEXPANSION_API USoundBaseAssetActionUtility : public UAssetActionUtility
{
    GENERATED_BODY()

public:

    /* ---   Sound Base   --- */

    /** Возвращает класс, который поддерживает данное действие с активом
    (если оно не реализовано, оно будет отображаться для всех типов активов) */
    UClass* GetSupportedClass_Implementable() const { return USoundBase::StaticClass(); };

    /** Установить Класс Звука для указанного Ассета типа "USoundBase"
    @param  SoundBase -- Ассет типа "USoundBase", для которого требуется сменить Класс Звука
    @param  NewClass -- Новый Класс Звука для данного Ассета */
    UFUNCTION(BlueprintCallable, CallInEditor,
        Category = "Asset Action Utility|Sound Base",
        meta = (DefaultToSelf, HideSelfPin = true))
    static bool SetSoundClass(USoundBase* SoundBase, USoundClass* NewClass);

    /** Установить Настройки Затухания для указанного Ассета типа "USoundBase"
    @param  SoundBase -- Ассет типа "USoundBase", для которого требуется сменить Настройки Затухания
    @param  NewAttenuation -- Новые Настройки Затухания для данного Ассета */
    UFUNCTION(BlueprintCallable, CallInEditor,
        Category = "Asset Action Utility|Sound Base",
        meta = (DefaultToSelf, HideSelfPin = true))
    static bool SetAttenuationSettings(USoundBase* SoundBase, USoundAttenuation* NewAttenuation);
    //-------------------------------------------
};
