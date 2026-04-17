// 

// Base:
#include "FPS_AbilitySystemComponent.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UFPS_AbilitySystemComponent::UFPS_AbilitySystemComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UFPS_AbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(
        Timer_InitStartingData,
        this,
        &UFPS_AbilitySystemComponent::InitStartingData,
        0.5f,
        false);
}

void UFPS_AbilitySystemComponent::OnComponentCreated()
{
    Super::OnComponentCreated();
}

void UFPS_AbilitySystemComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}
//--------------------------------------------------------------------------------------



/* ---   GAS   --- */

void UFPS_AbilitySystemComponent::InitStartingData()
{
    /* ---   GAS : Checks   --- */

    CheckTagsInAbilitiesEffectsParams();
    CheckTagsInStartingEffectsWithParameters();
    //-------------------------------------------

    if (GetOwnerActor() && GetOwnerActor()->HasAuthority())
    {
        InitStartingAttributes();
        InitAvailableAbilities();
        InitStartingEffectsWithParameters();
    }
}

void UFPS_AbilitySystemComponent::SetParamsInGameplayEffectSpec(FGameplayEffectSpec& Spec, const TMap<FGameplayTag, float>& Params)
{
    Spec.SetByCallerTagMagnitudes.Append(Params);
}

void UFPS_AbilitySystemComponent::CheckTagsInParams(TMap<FGameplayTag, float>& Params)
{
#if WITH_EDITOR

    for (TPair<FGameplayTag, float>& Param : Params)
    {
        if (!Param.Key.IsValid())
        {
            FPS_LOG_Component(Warning, "Invalid param: '%s' with a value of '%f' ",
                *Param.Key.GetTagName().ToString(), Param.Value);
        }
    }
#endif // WITH_EDITOR

    Params.Remove(FGameplayTag::EmptyTag);
}
//--------------------------------------------------------------------------------------



/* ---   GAS | Attributes   --- */

FGameplayEffectSpecHandle UFPS_AbilitySystemComponent::MakeEffectSpecWithParams(
    const TSubclassOf<UGameplayEffect> EffectClass,
    const float Level)
{
    if (!EffectClass) return FGameplayEffectSpecHandle();

    FGameplayEffectSpecHandle lResult = MakeOutgoingSpec(EffectClass, Level, MakeEffectContext());
    SetParamsInGameplayEffectSpec(*lResult.Data, AbilitiesEffectsParams);

    return lResult;
}

FActiveGameplayEffectHandle UFPS_AbilitySystemComponent::ApplyEffectSpecToTargetWithParams(
    UAbilitySystemComponent* TargetASC,
    const TSubclassOf<UGameplayEffect> EffectClass,
    const float Level)
{
    if (!EffectClass || !TargetASC) return FActiveGameplayEffectHandle();

    return ApplyGameplayEffectSpecToTarget(
        *MakeEffectSpecWithParams(EffectClass, Level).Data,
        TargetASC);
}

void UFPS_AbilitySystemComponent::InitStartingAttributes()
{
    for (const TPair<FGameplayAttribute, float>& Pair : StartingAttributes)
    {
        // Фильтрация от незаполненного значения с проверкой свойства на тип "FGameplayAttributeData"
        // PS: Является защитой, от ошибки при попытке изменить данные-"не атрибуты" в "AttributeSet",
        // например при ключе == "UFPS_AttributeSet.OnZeroHealth"
        if (Pair.Key.IsGameplayAttributeDataProperty(Pair.Key.GetUProperty()))
        {
            if (GetAttributeSubobject(Pair.Key.GetAttributeSetClass()))
            {
                SetNumericAttributeBase(Pair.Key, Pair.Value);
            }
            else
            {
                FPS_Error_Component("'%s' is NOT used for this Actor. See Attribute '%s' ",
                    *Pair.Key.GetAttributeSetClass()->GetName(), *Pair.Key.GetName());
            }
        }
        else
        {
            FPS_LOG_Component(Warning, "Incorrect Attribute Name: '%s'",
                *Pair.Key.GetName());
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   GAS | Abilities   --- */

void UFPS_AbilitySystemComponent::InitAvailableAbilities()
{
    for (TSubclassOf<UGameplayAbility>& GAClass : AvailableAbilities)
    {
        // Фильтрация от незаполненного значения
        if (GAClass.Get())
        {
            GiveAbility(
                FGameplayAbilitySpec(
                    GAClass,
                    1,
                    INDEX_NONE,
                    GetOwnerActor()));
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   GAS | Effects   --- */

void UFPS_AbilitySystemComponent::RemoveAllGameplayEffects()
{
    for (const FActiveGameplayEffect& AGE : &ActiveGameplayEffects)
    {
        RemoveActiveGameplayEffect(AGE.Handle);
    }
}

void UFPS_AbilitySystemComponent::InitStartingEffectsWithParameters()
{
    for (TPair<TSubclassOf<UGameplayEffect>, FEffectsParamData>& Pair : StartingEffectsWithParameters)
    {
        // Фильтрация от незаполненного значения
        if (Pair.Key.Get())
        {
            FGameplayEffectSpec lSpec = FGameplayEffectSpec(
                Pair.Key->GetDefaultObject<UGameplayEffect>(), MakeEffectContext());

            SetParamsInGameplayEffectSpec(lSpec, Pair.Value.SetByCallerValues);

            ApplyGameplayEffectSpecToSelf(lSpec);
        }
    }
}

void UFPS_AbilitySystemComponent::CheckTagsInStartingEffectsWithParameters()
{
    for (TPair<TSubclassOf<UGameplayEffect>, FEffectsParamData>& Pair : StartingEffectsWithParameters)
    {
        CheckTagsInParams(Pair.Value.SetByCallerValues);
    }
}
//--------------------------------------------------------------------------------------
