//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Components/ActorComponent.h"

// Generated:
#include "SmoothMovementComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: Начало перемещения
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMove);

// Делегат: Завершение перемещения
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompletedMove);

// Делегат: На Приближении
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApproach);

// Делегат: На Отдалении
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSeparation);
// ----------------------------------------------------------------------------------------------------



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API USmoothMovementComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    // Делегат: Начало перемещения
    UPROPERTY(BlueprintAssignable)
    FOnStartMove OnStartMove;

    // Делегат: Завершение перемещения
    UPROPERTY(BlueprintAssignable)
    FOnCompletedMove OnCompletedMove;

    // Делегат: На Приближении
    UPROPERTY(BlueprintAssignable)
    FOnApproach OnApproach;

    // Делегат: На Отдалении
    UPROPERTY(BlueprintAssignable)
    FOnSeparation OnSeparation;
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    USmoothMovementComponent();
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



    /* ---   Actor Movement   --- */

    /* Флаг использования Относительной Локации, а не Мировой
    @note   НЕ корректно работает, если у компонента-владельца изменена Ротация */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement")
    bool bUseRelativeLocation = false;

    // Максимальная скорость перемещения
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement|Speed")
    float MaxSpeed = 1000.f;

    // Минимальный шаг перемещения.
    // Если до конечной точки осталось расстояние меньше MinStep, то Актор "телепортируется" в точку назначения
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement|Speed")
    float MinStep = 0.5f;

    // Флаг контроля плавности скорости на старте движения
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement|Acceleration")
    bool bControlSpeedAtStart = false;

    // Коэффициент ускорения скорости перемещения в начале пути
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement|Acceleration",
        meta = (EditCondition = "bControlSpeedAtStart"))
    float AccelerationCoefficient = 10.f;

    // Коэффициент замедления скорости перемещения в конце пути
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement|Acceleration")
    float DecelerationCoefficient = 10.f;

    // Расстояние, на котором срабатывает Делегат OnApproach (На Приближении)
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement|Distance Triggers")
    float ApproachDistance = 100.f;

    // Расстояние, на котором срабатывает Делегат OnSeparation (На Отдалении)
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Movement|Distance Triggers")
    float SeparationDistance = 100.f;

    //

    /** Переместиться к указанной Точке */
    void MoveToLocation(const FVector& Point);
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
    void InitCurrentComponent();
    //-------------------------------------------



    /* ---   Actor Movement   --- */

    // Флаг контроля перемещения
    bool bIsMovingToNewLocation = false;

    // Стартовая локация (начало) перемещения
    FVector StartLocation;

    // Конечная локация перемещения
    FVector EndLocation;

    // Флаг срабатывания Делегата OnApproach (На Приближении)
    bool bApproachWork = false;

    // Флаг срабатывания Делегата OnSeparation (На Отдалении)
    bool bSeparationWork = false;

    //

    /** Расчёт передвижения на каждый кадр */
    void MovementForTick(const float& DeltaTime);
    //-------------------------------------------
};
