// 

// Base:
#include "FPS_AttributeSet.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Net:
#include "Net/UnrealNetwork.h"

// GAS:
#include "FPS/Tools/GAS/FPS_GameplayTags.h"
#include "GameplayEffectExtension.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameMode.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

// Смещение значений Атрибутов, ниже которого Атрибут воспринимается как с Нулевым значением
#define ZERO_VALUE_OFFSET 0.5f
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void UFPS_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);


    /* ---   Attributes   --- */

    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, Armor, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, MaxArmor, COND_None, REPNOTIFY_Always);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   AttributeSet   --- */

bool UFPS_AttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    return Super::PreGameplayEffectExecute(Data);
}

void UFPS_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

void UFPS_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    /** Отслеживание изменений Атрибутов со стороны сервера */

    // Проверяем в порядке от более часто изменяемого к редкому
    if (Attribute == GetArmorAttribute())
    {
        if (GetArmor() >= ZERO_VALUE_OFFSET
            && ZERO_VALUE_OFFSET > NewValue)
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(
                FPS_GameplayTags::GameplayState_WithoutArmor);

            OnZeroArmor.Broadcast();
        }
        else if (GetArmor() < ZERO_VALUE_OFFSET
            && ZERO_VALUE_OFFSET <= NewValue)
        {
            GetOwningAbilitySystemComponent()->RemoveLooseGameplayTag(
                FPS_GameplayTags::GameplayState_WithoutArmor,
                3 /* Защита от многократного тега */);
        }
    }
    else if (Attribute == GetHealthAttribute())
    {
        if (GetHealth() >= ZERO_VALUE_OFFSET
            && ZERO_VALUE_OFFSET > NewValue)
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(
                FPS_GameplayTags::GameplayState_OnDestroyed);

            OnZeroHealth.Broadcast();

            if (GetFPSGameMode())
            {
                GetFPSGameMode()->DestructionRegistration(*GetOwningAbilitySystemComponent());
            }
        }
    }

    Super::PreAttributeChange(Attribute, NewValue);
}

void UFPS_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    /** Ограничение Атрибутов в соответствующем диапазоне */

    if (NewValue < ZERO_VALUE_OFFSET)
    {
        // Всеобщее ограничение в нижнем диапазоне
        NewValue = 0.f;

        // Отслеживание ключевых тегов на старте
        if (Attribute == GetArmorAttribute())
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(
                FPS_GameplayTags::GameplayState_WithoutArmor);
        }
        else if (Attribute == GetHealthAttribute())
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(
                FPS_GameplayTags::GameplayState_OnDestroyed);
        }
    }
    else
    {
        // Ограничение по верхнему диапазону с проверкой в порядке от более часто изменяемого к редкому
        if (Attribute == GetArmorAttribute())
        {
            NewValue = FMath::Min(NewValue, GetMaxArmor());
        }
        else if (Attribute == GetHealthAttribute())
        {
            NewValue = FMath::Min(NewValue, GetMaxHealth());
        }
    }

    Super::PreAttributeBaseChange(Attribute, NewValue);
}
//--------------------------------------------------------------------------------------



/* ---   Net | OnRep   --- */

void UFPS_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    /** Отслеживание со стороны клиента */
    if (Health.GetBaseValue() < ZERO_VALUE_OFFSET
        && ZERO_VALUE_OFFSET <= OldHealth.GetBaseValue())
    {
        OnZeroHealth.Broadcast();
    }

    GAMEPLAYATTRIBUTE_REPNOTIFY(UFPS_AttributeSet, Health, OldHealth);
}

void UFPS_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFPS_AttributeSet, MaxHealth, OldMaxHealth);
}

void UFPS_AttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
    /** Отслеживание со стороны клиента */
    if (Armor.GetBaseValue() < ZERO_VALUE_OFFSET
        && ZERO_VALUE_OFFSET <= OldArmor.GetBaseValue())
    {
        OnZeroArmor.Broadcast();
    }

    GAMEPLAYATTRIBUTE_REPNOTIFY(UFPS_AttributeSet, Armor, OldArmor);
}

void UFPS_AttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFPS_AttributeSet, MaxArmor, OldMaxArmor);
}
//--------------------------------------------------------------------------------------
