//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Components/ActorComponent.h"

// Generated:
#include "SmoothRotationComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: Завершение вращения
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompletedRotate);
// ----------------------------------------------------------------------------------------------------



/* ---   Enums   --- */

/** Вариации Ротации Актора */
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class ERotationVariations : uint8
{
    NotLock = 0,

    Roll = 0b001, // Roll
    Pitch = 0b010, // Pitch
    Yaw = 0b100, // Yaw

    XY = Roll | Pitch, // Roll and Pitch
    XZ = Roll | Yaw, // Roll and Yaw
    YZ = Pitch | Yaw, // Pitch and Yaw

    XYZ = 0b111    UMETA(DisplayName = "XYZ: Component is Disabled")// All
};
ENUM_CLASS_FLAGS(ERotationVariations)
//--------------------------------------------------------------------------------------



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
    //virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    /** Вызывается при создании компонента */
    //virtual void OnComponentCreated() override;

    /** Вызывается при уничтожении компонента
    @param bDestroyingHierarchy - значение True, если разрушается вся иерархия компонентов.
    Позволяет избежать дорогостоящих операций
    */
    //virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
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
    void RotateToLocation(const FVector& Point = FVector::ZeroVector);

    /** Повернуться к указанной Точке */
    void RotateToRotator(const FRotator& Rotator = FRotator::ZeroRotator);

    /** Остановить контроль Вращения */
    FORCEINLINE void StopRotate()
    {
        bIsRotatedToNewRotation = false;
    };

    FORCEINLINE bool CheckLock(ERotationVariations Value)
    {
        return bool(LockRotation & Value);
    };
    //-------------------------------------------



private:

    /* ---   Actor Rotate   --- */

    // Стартовая ротация (начало) вращения
    FRotator StartRotation;

    // Конечная ротация вращения
    FRotator EndRotation;

    //

    /** Расчёт вращения на каждый кадр */
    void RotationForTick(float DeltaTime);
    //-------------------------------------------



    /* ---   Booleanas   --- */

public:

    // Блокировка направления ротации
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation")
    ERotationVariations LockRotation = ERotationVariations::NotLock;

    /* Флаг использования Относительной Ротации, а не Мировой
    @note   НЕ корректно работает, если у компонента-владельца изменена Ротация */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation",
        meta = (DisplayPriority = -1))
    uint8 bUseRelativeRotation : 1;

    // Флаг контроля плавности скорости на старте вращения
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings|Rotation|Acceleration",
        meta = (DisplayPriority = -1))
    uint8 bControlSpeedAtStart : 1;



private:

    // Флаг контроля вращения
    uint8 bIsRotatedToNewRotation : 1;
    //-------------------------------------------
};
