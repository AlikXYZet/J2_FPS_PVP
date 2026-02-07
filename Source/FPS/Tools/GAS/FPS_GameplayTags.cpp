// 

// Base:
#include "FPS_GameplayTags.h"
//--------------------------------------------------------------------------------------



/* ---   Tags   --- */

namespace FPS_GameplayTags
{
    /* ---   Ability Data | Explosion   --- */

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityData_Explosion_Power, "AbilityData.Explosion.Power",
        "Explosion parameter: Maximum exposure Force (max Power)");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityData_Explosion_DamageRadius, "AbilityData.Explosion.DamageRadius",
        "Explosion parameter: Maximum impact Radius of explosion");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityData_Explosion_EffectiveRadius, "AbilityData.Explosion.EffectiveRadius",
        "Explosion parameter: Effective explosion radius with Max Power");
    //-------------------------------------------


    /* ---   Gameplay State   --- */

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayState_OnDestroyed, "GameplayState.OnDestroyed",
        "Gameplay State: On Destroyed");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayState_WithoutArmor, "GameplayState.WithoutArmor",
        "Gameplay State: Without Armor");
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------
