// 

// Base:
#include "FPS_AbilitySystemComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UFPS_AbilitySystemComponent::UFPS_AbilitySystemComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно

    // Компонент реплицируем по умолчанию
    SetIsReplicatedByDefault(true);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UFPS_AbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();

    InitStartingData();
}

void UFPS_AbilitySystemComponent::OnComponentCreated()
{
    Super::OnComponentCreated();
}
//--------------------------------------------------------------------------------------



/* ---   GAS   --- */

void UFPS_AbilitySystemComponent::InitStartingData()
{
    if (GetOwnerActor() && GetOwnerActor()->HasAuthority())
    {
        for (TPair<FGameplayAttribute, float>& Data : StartingAttributes)
        {
            // Фильтрация от незаполненного значения с проверкой свойства на тип "FGameplayAttributeData"
            // PS: Является защитой, от ошибки при попытке изменить данные-"не атрибуты" в "AttributeSet",
            // например при ключе == "UFPS_AttributeSet.OnZeroHealth"
            if (Data.Key.IsGameplayAttributeDataProperty(Data.Key.GetUProperty()))
            {
                if (GetAttributeSubobject(Data.Key.GetAttributeSetClass()))
                {
                    SetNumericAttributeBase(Data.Key, Data.Value);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("'%s'::'%s'::%s: '%s' is NOT used for this Actor. See Attribute '%s' "),
                        *GetNameSafe(GetOwnerActor()), *GetNameSafe(this), *FString(__func__),
                        *Data.Key.GetAttributeSetClass()->GetName(), *Data.Key.GetName());
                }
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("'%s'::'%s'::%s: Incorrect Attribute Name: '%s'"),
                    *GetNameSafe(GetOwnerActor()), *GetNameSafe(this), *FString(__func__), *Data.Key.GetName());
            }
        }

        for (TSubclassOf<UGameplayAbility>& GAClass : StartingAbilities)
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

        for (TSubclassOf<UGameplayEffect>& GEClass : StartingEffects)
        {
            // Фильтрация от незаполненного значения
            if (GEClass.Get())
            {
                ApplyGameplayEffectSpecToSelf(
                    FGameplayEffectSpec(
                        GEClass->GetDefaultObject<UGameplayEffect>(),
                        MakeEffectContext()));
            }
        }
    }
}
//--------------------------------------------------------------------------------------
