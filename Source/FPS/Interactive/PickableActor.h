//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated:
#include "PickableActor.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class UPickupComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APickableActor : public AActor
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
    UPickupComponent* PickupComponent;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:
};
