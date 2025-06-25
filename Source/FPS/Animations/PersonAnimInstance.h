//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Animation/AnimInstance.h"

// UE:
#include "Engine/DataTable.h"

// Structs:
#include "FPS/Tools/Structs/Animations/PersonAnimData.h"

// Generated:
#include "PersonAnimInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;

// Interaction:
class APlayerCharacter;
class UWeaponControlComponent;
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

    /** Native Uninitialize override point */
    virtual void NativeUninitializeAnimation() override;

    /** Executed when begin play is called on the owning component */
    virtual void NativeBeginPlay() override;
    //-------------------------------------------



protected:

    /* ---   Customized Base   --- */

    /** Пользовательский аналог UpdateAnimation() */
    virtual void CustomUpdateAnimation();
    //-------------------------------------------



    /* ---   Player Actions   --- */

    /** Обновить данные  */
    UFUNCTION()
    virtual void UpdateBasicData();
    //-------------------------------------------



public:

    /* ---   Customized Base   --- */

    /* Игрок-Владелец данного UAnimInstance */
    UPROPERTY(BlueprintReadOnly,
        Category = "PersonAnim|Base")
    APlayerCharacter* PlayerOwner = nullptr;

    // Время циклического срабатывания CustomUpdateAnimation()
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Base",
        meta = (ClampMin = "0", UIMin = "0"))
    float TimeForCustomUpdateAnimation = 0.1f;

    //

    /** Пользовательский аналог UpdateAnimation() */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "PersonAnim|Base",
        meta = (DisplayName = "Custom Update Animation"))
    void Event_CustomUpdateAnimation();

    /** Установить Время срабатывания CustomUpdateAnimation() */
    UFUNCTION(BlueprintCallable,
        Category = "PersonAnim|Base")
    void SetTimeForUpdateAnimation(const float& Time);
    //-------------------------------------------



    /* ---   Movements   --- */

    /* Текущий Режим перемещения */
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Movements",
        meta = (DisplayPriority = 0))
    TEnumAsByte<EMovementMode> CurrentMovementMode = EMovementMode::MOVE_None;

    // Текущая Скорость Перемещения
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Movements",
        meta = (DisplayPriority = 0))
    float CurrentMovementSpeed = 0;

    // Текущее относительное Направление Перемещения
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Movements")
    float CurrentMovementDirection = 0;

    // Текущий угол Вертикального Взора
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Movements")
    float CurrentVerticalViewingAngle = 0;

    //

    /** Вызывается при обновлении Режима Перемещения */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "PersonAnim|Movements",
        meta = (DisplayName = "Update Movement Mode"))
    void Event_UpdateMovementMode(EMovementMode MovementMode, uint8 CustomMovementMode);
    //-------------------------------------------



    /* ---   Player Actions   --- */

    /* Компонент контроля Оружия Игрока-Владельца данного AnimInstance */
    UPROPERTY(BlueprintReadOnly,
        Category = "PersonAnim|Actions")
    UWeaponControlComponent* WeaponControlComponent = nullptr;

    //

    /** Проверка действий Игрока
    * @note (Быстрая потокобезопасная проверка) */
    UFUNCTION(BlueprintCallable, BlueprintPure,
        Category = "PersonAnim|Actions",
        meta = (CompactNodeTitle = "Check_Action", AutoCreateRefTerm = "InAction", BlueprintThreadSafe))
    bool CheckAction(const EActionVariations& InAction) const;
    //-------------------------------------------



    /* ---   Animations   --- */

    /** BS Передвижения Персонажа с зависимостью от скорости */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Lower Half")
    UBlendSpaceBase* BS_Movement = nullptr;

    // Анимация начала Прыжка
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Lower Half")
    UAnimSequence* A_StartJump = nullptr;

    // Зацикленная Анимация в Прыжке
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Lower Half")
    UAnimSequence* A_WhileJumping = nullptr;

    // Анимация завершения Прыжка
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Lower Half")
    UAnimSequence* A_EndJump = nullptr;

    // Группа изменяемых Элементов Анимации Игрока
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Person Animations: Data")
    FPersonAnimData PersonAnimations;
    //-------------------------------------------



    /* ---   Sounds   --- */

    // Таблица Данных для Звуков Поверхностей при Шаге
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnim|Sounds|Step",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfaceType"))
    UDataTable* SurfaceSoundsWhenStep = nullptr;

    // TMap Данных для Звуков Поверхностей при Шаге
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Sounds|Step",
        meta = (ForceInlineRow))
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> MapOfSurfaceSoundsWhenStep;

    // Таблица Данных для Звуков Поверхностей при Падении
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnim|Sounds|Landing",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfaceType"))
    UDataTable* SurfaceSoundsWhenLanding = nullptr;

    // TMap Данных для Звуков Поверхностей при Падении
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Sounds|Landing",
        meta = (ForceInlineRow))
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> MapOfSurfaceSoundsWhenLanding;

    //

    /** Воспроизвести Звук согласно поверхности, на которой находиться игрок */
    UFUNCTION(BlueprintCallable,
        Category = "PersonAnim|Sounds")
    void PlaySurfaceSound(const TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*>& SoundsMap);

    /** Получить контейнер типа TMap из Таблицы Данных  */
    UFUNCTION(BlueprintPure,
        Category = "PersonAnim|Sounds")
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> GetMapForSound(const UDataTable* SoundTable);
    //-------------------------------------------



private:

    /* ---   Customized Base   --- */

    // Таймер для Event_CustomUpdateAnimation()
    FTimerHandle Timer_CustomUpdateAnimation;

    //

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* ---   Movements   --- */

    /** Вызывается при обновлении Режима Перемещения */
    UFUNCTION()
    void UpdateMovementMode(ACharacter* Character, EMovementMode PrevMovementMode, uint8 PrevCustomMode);
    //-------------------------------------------



    /* ---   Sounds   --- */

    /** Инициализация Данных о Звуках */
    void SoundsInit();
    //-------------------------------------------
};
