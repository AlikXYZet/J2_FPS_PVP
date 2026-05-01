//

// Base:
#include "SoundBaseAssetActionUtility.h"
//--------------------------------------------------------------------------------------



/* ---   Static Mesh   --- */

bool USoundBaseAssetActionUtility::SetSoundClass(USoundBase* SoundBase, USoundClass* NewClass)
{
    if (SoundBase)
    {
        if(SoundBase->SoundClassObject != NewClass)
        {
            SoundBase->SoundClassObject = NewClass;
            SoundBase->MarkPackageDirty();
        }

        return true;
    }
    return false;
}

bool USoundBaseAssetActionUtility::SetAttenuationSettings(USoundBase* SoundBase, USoundAttenuation* NewAttenuation)
{
    if (SoundBase)
    {
        if(SoundBase->AttenuationSettings != NewAttenuation)
        {
            SoundBase->AttenuationSettings = NewAttenuation;
            SoundBase->MarkPackageDirty();
        }

        return true;
    }
    return false;
}
//--------------------------------------------------------------------------------------
