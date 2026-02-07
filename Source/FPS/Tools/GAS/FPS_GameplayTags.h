//

#pragma once

// GAS:
#include "NativeGameplayTags.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** Defines a native gameplay tag with a comment that is externally declared
* in a header to allow other modules or code to use the created tag variable */
#define UE_DEFINE_GAMEPLAY_TAG_COMMENT(TagName, Tag, Comment) \
    FNativeGameplayTag TagName(UE_PLUGIN_NAME, UE_MODULE_NAME, Tag, TEXT(Comment), ENativeGameplayTagToken::PRIVATE_USE_MACRO_INSTEAD);
// PS: Макрос из UE5
//--------------------------------------------------------------------------------------



/* ---   Tags   --- */

namespace FPS_GameplayTags
{
    /* ---   Ability Data | Explosion   --- */

    // Параметр Взрыва: Максимальная Сила воздействия (макс. Мощность)
    FPS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityData_Explosion_Power);
    // Параметр Взрыва: Максимальный Радиус воздействия взрыва
    FPS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityData_Explosion_DamageRadius);
    // Параметр Взрыва: Эффективный Радиус взрыва с Макс. Мощностью
    FPS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityData_Explosion_EffectiveRadius);
    //-------------------------------------------


    /* ---   Gameplay State   --- */

    // Геймплейное состояние: Уничтожен
    FPS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayState_OnDestroyed);

    // Геймплейное состояние: Без Брони (защиты)
    FPS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayState_WithoutArmor);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   undef   --- */

//#undef UE_DEFINE_GAMEPLAY_TAG_COMMENT // Используется вне данного файла
//--------------------------------------------------------------------------------------
