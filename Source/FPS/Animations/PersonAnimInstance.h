//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Animation/AnimInstance.h"

// Structs:
#include "FPS/Tools/Structs/Animations/PersonAnimData.h"
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"

// Generated:
#include "PersonAnimInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class APlayerCharacter;
class UWeaponNetworkController;
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
    virtual void UpdateBasicData(const FWeaponData& CurrentNewWeaponData);
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
    UWeaponNetworkController* WeaponControlNetComp = nullptr;

    //

    /** Проверка действий Игрока
    * @note   Быстрая потокобезопасная проверка */
    UFUNCTION(BlueprintPure,
        Category = "PersonAnim|Actions",
        meta = (CompactNodeTitle = "Check Action", AutoCreateRefTerm = "InAction",
            BlueprintThreadSafe, DefaultToSelf, HideSelfPin = "true"))
    bool CheckAction(const EActionVariations& InAction) const;

    /* Проверить на Все действия Игрока
    * @note   Быстрая потокобезопасная проверка */
    UFUNCTION(BlueprintPure,
        Category = "PersonAnim|Actions",
        meta = (CompactNodeTitle = "Check All Actions ( & )", Keywords = "& and",
            CommutativeAssociativeBinaryOperator, BlueprintThreadSafe, DefaultToSelf, HideSelfPin = "true"))
    bool CheckAllActions(const EActionVariations A, const EActionVariations B) const;

    /* Проверить на Одно Из действий Игрока
    * @note   Быстрая потокобезопасная проверка */
    UFUNCTION(BlueprintPure,
        Category = "PersonAnim|Actions",
        meta = (CompactNodeTitle = "Check One Of Actions ( | )", Keywords = "| or",
            CommutativeAssociativeBinaryOperator, BlueprintThreadSafe, DefaultToSelf, HideSelfPin = "true"))
    bool CheckOneOfActions(const EActionVariations A, const EActionVariations B) const;

    /* Проверить на все, Кроме следующих действий Игрока
    * @note   Быстрая потокобезопасная проверка */
    UFUNCTION(BlueprintPure,
        Category = "PersonAnim|Actions",
        meta = (CompactNodeTitle = "Check Except Actions ( ^ )", Keywords = "^ xor",
            CommutativeAssociativeBinaryOperator, BlueprintThreadSafe, DefaultToSelf, HideSelfPin = "true"))
    bool CheckExceptActions(const EActionVariations A, const EActionVariations B) const;
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
};
