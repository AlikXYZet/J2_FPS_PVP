//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated:
#include "ExplosiveActor.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class UExplosiveComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AExplosiveActor : public AActor
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this actor's properties
    AExplosiveActor();
    //-------------------------------------------



    /* ---   Components   --- */

    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    // Компонент Взрыва данного Актора
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = Components,
        meta = (AllowPrivateAccess = "true"))
    UExplosiveComponent* ExplosiveComponent;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:
};
