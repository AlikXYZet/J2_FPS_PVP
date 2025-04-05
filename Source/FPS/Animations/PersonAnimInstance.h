//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

// UE:
#include "Engine/DataTable.h"

// Generated:
#include "PersonAnimInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;

// Interaction:
class APlayerCharacter;
//--------------------------------------------------------------------------------------



/* ---   Structs   --- */

/* Структура Таблиц Данных:
Звуки в зависимости от Типа Поверхности
*/
USTRUCT(BlueprintType)
struct FSoundBySurfaceType : public FTableRowBase
{
    GENERATED_BODY()

    // Тип Поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EPhysicalSurface> SurfaceType = EPhysicalSurface::SurfaceType_Default;

    // Звук, соответствующий поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* Sound = nullptr;
};
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UPersonAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Native initialization override point */
    virtual void NativeInitializeAnimation() override;

    /** Executed when begin play is called on the owning component */
    virtual void NativeBeginPlay() override;
    //-------------------------------------------



    /* ---   Customized Base   --- */

    /** Игрок-Владелец данного AnimInstance */
    UPROPERTY(BlueprintReadOnly,
        Category = "PersonAnimInstance|Base")
    APlayerCharacter* PlayerOwner = nullptr;

    // Время циклического срабатывания CustomUpdateAnimation()
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnimInstance|Base",
        meta = (ClampMin = "0", UIMin = "0"))
    float TimeForCustomUpdateAnimation = 0.1f;

    //

    /** Пользовательский аналог UpdateAnimation() */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "PersonAnimInstance|Base",
        meta = (DisplayName = "Custom Update Animation"))
    void EventCustomUpdateAnimation();

    /** Установить Время срабатывания CustomUpdateAnimation() */
    UFUNCTION(BlueprintCallable,
        Category = "PersonAnimInstance|Base")
    void SetTimeForUpdateAnimation(const float Time);
    //-------------------------------------------



    /* ---   Movements   --- */

    /** Текущий Режим перемещения */
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnimInstance|Movements")
    TEnumAsByte<EMovementMode> CurrentMovementMode = EMovementMode::MOVE_None;

    // Текущая Скорость перемещения
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnimInstance|Movements")
    float CurrentMovementSpeed;

    // Текущее относительное Направление перемещения
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnimInstance|Movements")
    float CurrentMovementDirection;

    //

    /** Вызывается при обновлении Режима Перемещения */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "PersonAnimInstance|Movements",
        meta = (DisplayName = "Update Movement Mode"))
    void EventUpdateMovementMode(EMovementMode MovementMode, uint8 CustomMovementMode);
    //-------------------------------------------



    /* ---   Sounds   --- */

    // Таблица Данных для Звуков Поверхностей при Шаге
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnimInstance|Sounds|Step",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfaceType"))
    UDataTable* SurfaceSoundsWhenStep = nullptr;

    // TMap Данных для Звуков Поверхностей при Шаге
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnimInstance|Sounds|Step")
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> MapOfSurfaceSoundsWhenStep;

    // Таблица Данных для Звуков Поверхностей при Падении
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnimInstance|Sounds|Landing",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfaceType"))
    UDataTable* SurfaceSoundsWhenLanding = nullptr;

    // TMap Данных для Звуков Поверхностей при Падении
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnimInstance|Sounds|Landing")
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> MapOfSurfaceSoundsWhenLanding;

    //

    /** Воспроизвести Звук согласно поверхности, на которой находиться игрок */
    UFUNCTION(BlueprintCallable,
        Category = "PersonAnimInstance|Sounds")
    void PlaySurfaceSound(const TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*>& SoundsMap);

    /** Получить контейнер типа TMap из Таблицы Данных  */
    UFUNCTION(BlueprintPure,
        Category = "PersonAnimInstance|Sounds")
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> GetMapForSound(const UDataTable* SoundTable);
    //-------------------------------------------



private:

    /* ---   Customized Base   --- */

    // Таймер для EventCustomUpdateAnimation()
    FTimerHandle Timer_CustomUpdateAnimation;

    //

    /** Инициализация базовых Данных */
    void BaseInit();

    /** Пользовательский аналог UpdateAnimation() */
    void CustomUpdateAnimation();
    //-------------------------------------------



    /* ---   Movement   --- */

    /** Инициализация Данных о Перемещении */
    void MovementInit();

    /** Вызывается при обновлении Режима Перемещения */
    UFUNCTION()
    void UpdateMovementMode(EMovementMode MovementMode, uint8 CustomMovementMode);
    //-------------------------------------------



    /* ---   Sounds   --- */

    /** Инициализация Данных о Звуках */
    void SoundsInit();
    //-------------------------------------------

};
