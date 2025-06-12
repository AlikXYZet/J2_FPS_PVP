//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Interface:
//#include "FPS/Tools/Interfaces/Properties/InteractiveInterface.h"

// Generated:
#include "PickableActor.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class UInteractiveComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APickableActor : public AActor//, public IInteractiveInterface
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this actor's properties
    APickableActor();
    //-------------------------------------------



    /* ---   Components   --- */

    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    // Компонент Подбора данного Актора
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = Components,
        meta = (AllowPrivateAccess = "true"))
    UInteractiveComponent* InteractiveComponent;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:
};
