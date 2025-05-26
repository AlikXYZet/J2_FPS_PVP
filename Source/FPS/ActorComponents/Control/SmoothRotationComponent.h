//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated:
#include "SmoothRotationComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: Завершение вращения
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompletedRotate);
// ----------------------------------------------------------------------------------------------------



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API USmoothRotationComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    /* Делегат: Завершение вращения */
    UPROPERTY(BlueprintAssignable)
    FOnCompletedRotate OnCompletedRotate;
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    USmoothRotationComponent();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Вызывается при создании компонента */
    virtual void OnComponentCreated() override;

    /** Вызывается при уничтожении компонента
    @param bDestroyingHierarchy - значение True, если разрушается вся иерархия компонентов.
    Позволяет избежать дорогостоящих операций
    */
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
    //-------------------------------------------



    /* ---   Actor Rotate   --- */

    // Максимальная скорость вращения
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation|Speed")
    float MaxSpeed = 100.f;

    // Минимальный шаг вращения.
    // Если до конечного значения вращения осталось меньше MinStep, то Шахматной Фигуре присваивается необходимая ротация
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation|Speed")
    float MinStep = 0.5f;

    // Флаг контроля плавности скорости на старте вращения
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation|Acceleration")
    bool bControlSpeedAtStart = false;

    // Коэффициент ускорения скорости вращения в начале пути
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation|Acceleration")
    float AccelerationCoefficient = 10.f;

    // Коэффициент замедления скорости вращения в конце пути
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation|Acceleration")
    float DecelerationCoefficient = 10.f;

    //

    /** Повернуться к указанной Точке */
    void RotateToLocation(const FVector& Point);

    /** Остановить контроль Вращения */
    FORCEINLINE void StopRotate()
    {
        bIsRotatedToNewRotation = false;
    };
    //-------------------------------------------



private:

    /* ---   Init   --- */

    // Текущий Актор-владелец
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly,
        Category = "Settings|Check",
        meta = (AllowPrivateAccess = true))
    AActor* CurrentActor = nullptr;

    //

    /** Инициализация данных текущего компонента*/
    void CurrentComponentInit();
    //-------------------------------------------



    /* ---   Actor Rotate   --- */

    // Флаг контроля вращения
    bool bIsRotatedToNewRotation = false;

    // Стартовая ротация (начало) вращения
    FRotator StartRotation;

    // Конечная ротация вращения
    FRotator EndRotation;

    //

    /** Расчёт вращения на каждый кадр */
    void RotationForTick(const float& DeltaTime);
    //-------------------------------------------
};
