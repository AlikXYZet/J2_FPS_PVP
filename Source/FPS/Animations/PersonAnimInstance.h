//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

// Generated:
#include "PersonAnimInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class APlayerCharacter;
class UWeaponControlComponent;
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
    void EventCustomUpdateAnimation();

    /** Установить Время срабатывания CustomUpdateAnimation() */
    UFUNCTION(BlueprintCallable,
        Category = "PersonAnim|Base")
    void SetTimeForUpdateAnimation(const float& Time);
    //-------------------------------------------



    /* ---   Movements Mode   --- */

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

    //

    /** Вызывается при обновлении Режима Перемещения */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "PersonAnim|Movements",
        meta = (DisplayName = "Update Movement Mode"))
    void EventUpdateMovementMode(EMovementMode MovementMode, uint8 CustomMovementMode);
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
        meta = (CompactNodeTitle = "Check_Action", BlueprintThreadSafe))
    bool CheckAction(const EActionVariations InAction) const;
    //-------------------------------------------



private:

    /* ---   Customized Base   --- */

    // Таймер для EventCustomUpdateAnimation()
    FTimerHandle Timer_CustomUpdateAnimation;

    //

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* ---   Movement Mode   --- */

    /** Инициализация Данных о Перемещении */
    void MovementInit();

    /** Вызывается при обновлении Режима Перемещения */
    UFUNCTION()
    void UpdateMovementMode(EMovementMode MovementMode, uint8 CustomMovementMode);
    //-------------------------------------------
};
