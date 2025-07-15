//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "AbilitySystemComponent.h"

// Generated:
#include "FPS_AbilitySystemComponent.generated.h"
//--------------------------------------------------------------------------------------



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
    //-------------------------------------------



    /* ---   GAS   --- */

    // Стартовые значения атрибутов
    UPROPERTY(EditDefaultsOnly,
        Category = "FPS_AbilitySystemComponent",
        meta = (ForceInlineRow))
    TMap<FGameplayAttribute, float> StartingAttributes;

    // Способности, которые будут предоставлены Актору в начале игры
    UPROPERTY(EditDefaultsOnly,
        Category = "FPS_AbilitySystemComponent")
    TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;

    // Эффекты, которые будут наложены на Актор в начале игры
    UPROPERTY(EditDefaultsOnly,
        Category = "FPS_AbilitySystemComponent")
    TArray<TSubclassOf<UGameplayEffect>> StartingEffects;
    //-------------------------------------------



private:

    /* ---   GAS   --- */

    /** Инициализация стартовых Данных (атрибутов, способностей, эффектов) */
    void InitStartingData();
    //-------------------------------------------
};
