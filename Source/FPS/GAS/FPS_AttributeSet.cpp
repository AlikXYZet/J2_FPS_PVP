// 

// Base:
#include "FPS_AttributeSet.h"

// Net:
#include "Net/UnrealNetwork.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UFPS_AttributeSet::UFPS_AttributeSet()
{

}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void UFPS_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);


    /* ---   Attributes   --- */

    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, Health, COND_None, REPNOTIFY_OnChanged);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, MaxHealth, COND_None, REPNOTIFY_OnChanged);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, Armor, COND_None, REPNOTIFY_OnChanged);
    DOREPLIFETIME_CONDITION_NOTIFY(UFPS_AttributeSet, MaxArmor, COND_None, REPNOTIFY_OnChanged);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   AttributeSet   --- */

void UFPS_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}

void UFPS_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
    }
    else if (Attribute == GetArmorAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxArmor());
    }
}
//--------------------------------------------------------------------------------------



/* ---   Net | OnRep   --- */

void UFPS_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
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
