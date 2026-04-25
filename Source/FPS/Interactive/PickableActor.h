//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/Actor.h"

// Interface:
#include "FPS/Tools/Interfaces/Properties/InteractiveInterface.h"

// Generated:
#include "PickableActor.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class UInteractiveComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APickableActor : public AActor, public IInteractiveInterface
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this actor's properties
    APickableActor();
    //-------------------------------------------



    /* ---   Components   --- */

    /** Меш визуализации */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = Components,
        meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* StaticMesh = nullptr;
    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    /** Компонент Подбора данного Актора */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = Components,
        meta = (AllowPrivateAccess = "true"))
    UInteractiveComponent* InteractiveComponent = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Interactive | Highlighting   --- */

    /** Получить компоненты, которые требуется подсветить */
    virtual TArray<FComponentRendering> GetUsedComponents_Implementation() override;
    //-------------------------------------------



    /* ---   Activity   --- */

    // Длительность времени в Активном состоянии
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PickableActor|Activity",
        meta = (ForceUnits = Seconds, ClampMin = "0", UIMin = "0"))
    float InactiveState_Time = 2.f;

    //

    /** Событие BP: При перезапуске Состояния Активности */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "PickableActor|Activity",
        meta = (DisplayName = "On Restarting Activity State"))
    void Event_OnRestartingActivityState();

    /** Запустить неактивное Состояние */
    UFUNCTION(BlueprintCallable,
        Category = "PickableActor|Activity")
    void StartInactiveState();

    UFUNCTION(BlueprintCallable, BlueprintPure,
        Category = "PickableActor|Activity")
    bool IsActiveState() const
    {
        return !Timer_ActivityControl.IsValid();
    }
    //-------------------------------------------



private:

    /* ---   Activity   --- */

    // Таймер для контроля Активности
    FTimerHandle Timer_ActivityControl;
    //-------------------------------------------
};
