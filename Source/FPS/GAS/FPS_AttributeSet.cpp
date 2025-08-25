// 

// Base:
#include "FPS_AttributeSet.h"

// Net:
#include "Net/UnrealNetwork.h"

// GAS:
#include "FPS/Tools/GAS/FPS_GameplayTags.h"
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
    Super::PreAttributeChange(Attribute, NewValue);
}

void UFPS_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());

        // Отслеживание со стороны сервера
        if (NewValue < 0.5f
            && GetHealth() >= 0.5f)
        {
            GetOwningAbilitySystemComponent()->AddLooseGameplayTag(
                FPS_GameplayTags::GameplayState_OnDestroyed);

            OnZeroHealth.Broadcast();
        }
    }
    else if (Attribute == GetArmorAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxArmor());
    }

    Super::PreAttributeBaseChange(Attribute, NewValue);
}
//--------------------------------------------------------------------------------------



/* ---   Net | OnRep   --- */

void UFPS_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    // Отслеживание со стороны клиента
    if (OnZeroHealth.IsBound()
        && Health.GetBaseValue() < 0.5f
        && OldHealth.GetBaseValue() >= 0.5f)
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
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFPS_AttributeSet, Armor, OldArmor);
}

void UFPS_AttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UFPS_AttributeSet, MaxArmor, OldMaxArmor);
}
//--------------------------------------------------------------------------------------
