//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "AbilitySystemComponent.h"

// Generated:
#include "FPS_AbilitySystemComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** Макрос: Создание функции делегата для передачи значения атрибутов GAS через Событие BP */
#define GAMEPLAYATTRIBUTE_VALUE_HandleChanged(PropertyName) \
	FORCEINLINE void Handle##PropertyName##Changed(const FOnAttributeChangeData& Data) \
	{ \
		Event_Changing##PropertyName(Data.NewValue); \
	}

/** Макрос: Подписка функции к делегату для передачи значения атрибутов GAS через Событие BP */
#define GAMEPLAYATTRIBUTE_VALUE_Delegating(Class, PropertyName) \
    AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AttributeSet->Get##PropertyName##Attribute()) \
        .AddUObject(this, &##Class##::Handle##PropertyName##Changed);
//--------------------------------------------------------------------------------------



/* Структура данных параметров для Эффектов */
USTRUCT()
struct FEffectsParamData
{
    GENERATED_BODY()

    // Таблица передаваемых параметров по Тэгу для Эффектов
    UPROPERTY(EditAnywhere,
        Category = "FPS_AbilitySystemComponent|Effects",
        meta = (ForceInlineRow, NoElementDuplicate, Categories = "AbilityData"))
    TMap<FGameplayTag, float> SetByCallerValues;
};



UCLASS()
class FPS_API UFPS_AbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UFPS_AbilitySystemComponent();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Вызывается при создании компонента */
    virtual void OnComponentCreated() override;

    /** Вызывается при уничтожении компонента
    * @param    bDestroyingHierarchy - true, если разрушается вся иерархия компонентов, позволяет избежать дорогостоящих операций
    */
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
    //-------------------------------------------



    /* ---   GAS | Attributes   --- */

    // Стартовые значения Атрибутов
    UPROPERTY(EditAnywhere,
        Category = "FPS_AbilitySystemComponent|Attributes",
        meta = (ForceInlineRow))
    TMap<FGameplayAttribute, float> StartingAttributes;
    //-------------------------------------------



    /* ---   GAS | Abilities   --- */

    // Доступные Способности, которые будут предоставлены Актору в начале игры
    UPROPERTY(EditAnywhere,
        Category = "FPS_AbilitySystemComponent|Abilities")
    TArray<TSubclassOf<UGameplayAbility>> AvailableAbilities;

    // Таблица передаваемых по Тэгу параметрами Эффектов, вызванных Способностями
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "FPS_AbilitySystemComponent|Abilities",
        meta = (ForceInlineRow, NoElementDuplicate, Categories = "AbilityData"))
    TMap<FGameplayTag, float> AbilitiesEffectsParams;

    //

    /** Создать спецификацию Эффекта с указанными в AbilitiesEffectsParams параметрами
    * @param    EffectClass - Класс накладываемого Эффекта
    * @param    Level - Уровень применяемого Эффекта
    * @return   Дескриптор спецификации Эффекта
    */
    UFUNCTION(BlueprintPure,
        Category = "FPS_AbilitySystemComponent|Abilities")
    FGameplayEffectSpecHandle MakeEffectSpecWithParams(
        const TSubclassOf<UGameplayEffect> EffectClass,
        const float Level = 1.f);


    /** Применить Эффект на цели с указанными в AbilitiesEffectsParams параметрами
    * @param    TargetASC - Целевой AbilitySystemComponent, на который накладывается Эффект
    * @param    EffectClass - Класс накладываемого Эффекта
    * @param    Level - Уровень применяемого Эффекта
    * @return   Дескриптор активного Эффекта
    */
    UFUNCTION(BlueprintCallable,
        Category = "FPS_AbilitySystemComponent|Abilities")
    FActiveGameplayEffectHandle ApplyEffectSpecToTargetWithParams(
        UAbilitySystemComponent* TargetASC,
        const TSubclassOf<UGameplayEffect> EffectClass,
        const float Level = 1.f);
    //-------------------------------------------



    /* ---   GAS | Effects   --- */

    // Стартовые Эффекты с индивидуальными параметрами, которые будут наложены на Актор в начале игры
    UPROPERTY(EditAnywhere,
        Category = "FPS_AbilitySystemComponent|Effects",
        meta = (ForceInlineRow))
    TMap<TSubclassOf<UGameplayEffect>, FEffectsParamData> StartingEffectsWithParameters;

    //

    UFUNCTION(BlueprintCallable,
        Category = "FPS_AbilitySystemComponent|Effects")
    void RemoveAllGameplayEffects();
    //-------------------------------------------



private:

    /* ---   GAS   --- */

    /** Инициализация стартовых Данных (атрибутов, способностей, эффектов) */
    void InitStartingData();

    /** Установите параметры в указанный GameplayEffectSpec
    * @param    EffectClass - Класс накладываемого Эффекта
    * @param    Params - Таблица Тегов с наименованием параметров и их значений
    */
    void SetParamsInGameplayEffectSpec(FGameplayEffectSpec& Spec, const TMap<FGameplayTag, float>& Params);

    /** Предварительная проверка Тегов в указанном контейнере параметров */
    void CheckTagsInParams(TMap<FGameplayTag, float>& Params);
    //-------------------------------------------



    /* ---   GAS | Attributes   --- */

    /** Инициализация стартовых значений Атрибутов */
    void InitStartingAttributes();
    //-------------------------------------------



    /* ---   GAS | Abilities   --- */

    /** Инициализация доступных Способностей */
    void InitAvailableAbilities();

    /** Предварительная проверка Тегов в AbilitiesEffectsParams */
    FORCEINLINE void CheckTagsInAbilitiesEffectsParams()
    {
        CheckTagsInParams(AbilitiesEffectsParams);
    };
    //-------------------------------------------



    /* ---   GAS | Effects   --- */

    /** Инициализация стартовых Эффектов с индивидуальными параметрами */
    void InitStartingEffectsWithParameters();

    /** Предварительная проверка Тегов в StartingEffectsWithParameters */
    void CheckTagsInStartingEffectsWithParameters();
    //-------------------------------------------
};
