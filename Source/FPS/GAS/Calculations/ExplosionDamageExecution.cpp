//

// Base:
#include "ExplosionDamageExecution.h"

// GAS:
#include "AbilitySystemComponent.h"
#include "FPS/GAS/FPS_AttributeSet.h"
#include "FPS/Tools/GAS/FPS_GameplayTags.h"
//--------------------------------------------------------------------------------------



/* ---   Static Struct   --- */

// Объявляем атрибуты для захвата и определяем, как мы хотим захватить их из источника и цели.
struct FDamageStatics
{
    /* ---   Source   --- */
    DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
    //-------------------------------------------

    /* ---   Target   --- */
    //DECLARE_ATTRIBUTE_CAPTUREDEF(Type);
    //-------------------------------------------

    FDamageStatics()
    {
        /* ---   Source   --- */
        DEFINE_ATTRIBUTE_CAPTUREDEF(UFPS_AttributeSet, Health, Source, true);
        DEFINE_ATTRIBUTE_CAPTUREDEF(UFPS_AttributeSet, Armor, Source, true);
        //-------------------------------------------

        /* ---   Target   --- */
        //DEFINE_ATTRIBUTE_CAPTUREDEF(UFPS_AttributeSet, Type, Target, false);
        //-------------------------------------------
    }
};

static const FDamageStatics& DamageStatics()
{
    static FDamageStatics DStatics;
    return DStatics;
}
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

UExplosionDamageExecution::UExplosionDamageExecution()
{
    // Зафиксируем атрибуты, имеющие отношение к вычислению
    RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
}
//--------------------------------------------------------------------------------------



/* ---   Calculation   --- */

void UExplosionDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    // Получаем целевой ASC и его Актор
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    const AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

    // Получаем источник и его Актор
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

    // Данные спецификации и контекста
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    //const FGameplayEffectContextHandle Context = Spec.GetContext();

    // Параметры взрыва
    const float MaxDamage = Spec.GetSetByCallerMagnitude(
        FPS_GameplayTags::AbilityData_Explosion_Power,
        false, 100.f);

    const float MaxRadius = Spec.GetSetByCallerMagnitude(
        FPS_GameplayTags::AbilityData_Explosion_DamageRadius,
        false, 1000.f);

    const float EffectiveRadius = Spec.GetSetByCallerMagnitude(
        FPS_GameplayTags::AbilityData_Explosion_EffectiveRadius,
        false, 100.f);

    float FinalDamage = 0.f;

    if (SourceActor && TargetActor)
    {
        const FVector SourceLocation = SourceActor->GetActorLocation();
        const FVector TargetLocation = TargetActor->GetActorLocation();

        const float Distance = FVector::Dist(SourceLocation, TargetLocation);

        if (Distance <= EffectiveRadius
            || (Distance <= MaxRadius
                && EffectiveRadius >= MaxRadius))
        {
            // Верхняя граница урона (Максимальный урон == MaxDamage)
            FinalDamage = MaxDamage;
        }
        else
        {
            const float DamageScale = 1.f - ((Distance - EffectiveRadius) / (MaxRadius - EffectiveRadius));

            // Расчёт относительного урона с учётом нижней границы (Минимальный урон == 0)
            FinalDamage = MaxDamage * FMath::Max(DamageScale, 0.f);
        }
    }

    // Применяем модификатор
    FGameplayModifierEvaluatedData HealthEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FinalDamage);
    FGameplayModifierEvaluatedData ArmorEvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, -FinalDamage);

    OutExecutionOutput.AddOutputModifier(HealthEvaluatedData);
    OutExecutionOutput.AddOutputModifier(ArmorEvaluatedData);
}
//--------------------------------------------------------------------------------------
