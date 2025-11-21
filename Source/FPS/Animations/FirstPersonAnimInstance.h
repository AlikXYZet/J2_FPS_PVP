//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "PersonAnimInstance.h"

// Generated:
#include "FirstPersonAnimInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class AFirstPersonWeaponFrame;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UFirstPersonAnimInstance : public UPersonAnimInstance
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



    /* ---   IK Anim   --- */

    // Имя сокета Правой руки
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,
        Category = "IK Anim")
    FName RightHandSocketName = "GripSocket";

    // Имя сокета Левой руки
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,
        Category = "IK Anim")
    FName LeftHandSocketName = "HoldingSocket";

    //

    /** Получить локацию для Правой руки */
    UFUNCTION(BlueprintPure,
        Category = "IK Anim",
        meta = (BlueprintThreadSafe))
    const FTransform& GetRightHandTransform();

    /** Получить локацию для Левой руки */
    UFUNCTION(BlueprintPure,
        Category = "IK Anim",
        meta = (BlueprintThreadSafe))
    const FTransform& GetLeftHandTransform();
    //-------------------------------------------



protected:
private:

    /* ---   IK_Anim   --- */

    // Текущий Каркас Оружия от Первого Лица
    const AFirstPersonWeaponFrame* CurrentFPWeaponFrame = nullptr;

    // Указатель на данные отслеживаемой точки Правой руки
    const FTransform* RightHandTransform = &FTransform::Identity;

    // Указатель на данные отслеживаемой точки Левой руки
    const FTransform* LeftHandTransform = &FTransform::Identity;

    //

    /** Инициализация для IK Анимации */
    void InitIKAnim();

    /** Обновить указатели на локации для рук */
    void UpdateTransformPtrForHands();
    //-------------------------------------------
};
