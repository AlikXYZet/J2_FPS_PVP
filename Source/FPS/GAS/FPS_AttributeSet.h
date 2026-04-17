//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "AttributeSet.h"

// GAS:
#include "FPS_AbilitySystemComponent.h"

// Generated:
#include "FPS_AttributeSet.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При Нулевом Здоровье
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnZeroHealth);

// Делегат: При Нулевой Броне
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnZeroArmor);
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UFPS_AttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    /* Делегат: При Нулевом Здоровье */
    UPROPERTY(BlueprintAssignable)
    FOnZeroHealth OnZeroHealth;

    /* Делегат: При Нулевой Броне */
    UPROPERTY(BlueprintAssignable)
    FOnZeroArmor OnZeroArmor;
    //-------------------------------------------



    /* ---   Constructors   --- */

    UFPS_AttributeSet() {};
    //-------------------------------------------



public:

    /* ---   Net   --- */

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



    /* ---   Attribute Set   --- */

    /** Вызывается непосредственно перед исполнением какого-либо 'GameplayEffect' */
    virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data);

    /** Вызывается непосредственно после выполнения какого-либо 'GameplayEffect', изменяющего базовое значение атрибута */
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

    /** Вызывается непосредственно перед изменением значения "Current Value" какого-либо атрибута.
    Например: Получение урона */
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    /** Вызывается непосредственно перед изменением значения "Base Value" какого-либо атрибута.
    Например: Установка первоначальных стартовых значений */
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    //-------------------------------------------



    /* ---   Attributes   --- */

    // Показатель Здоровья
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health,
        Category = "Attributes")
    FGameplayAttributeData Health = 100.f;
    ATTRIBUTE_ACCESSORS(UFPS_AttributeSet, Health);

    // Показатель максимального Здоровья
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth,
        Category = "Attributes")
    FGameplayAttributeData MaxHealth = 100.f;
    ATTRIBUTE_ACCESSORS(UFPS_AttributeSet, MaxHealth);

    // Показатель Брони
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor,
        Category = "Attributes")
    FGameplayAttributeData Armor = 100.f;
    ATTRIBUTE_ACCESSORS(UFPS_AttributeSet, Armor);

    // Показатель максимальной Брони
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor,
        Category = "Attributes")
    FGameplayAttributeData MaxArmor = 100.f;
    ATTRIBUTE_ACCESSORS(UFPS_AttributeSet, MaxArmor);
    //-------------------------------------------



protected:

    /* ---   Net | OnRep   --- */

    /** Функция обратного вызова: Показатель Здоровья */
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

    /** Функция обратного вызова: Показатель максимального Здоровья */
    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    /** Функция обратного вызова: Показатель Брони */
    UFUNCTION()
    virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

    /** Функция обратного вызова: Показатель максимальной Брони */
    UFUNCTION()
    virtual void OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor);
    //-------------------------------------------



private:
};



/* ---   undef   --- */

#undef ATTRIBUTE_ACCESSORS
//--------------------------------------------------------------------------------------
