//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated:
#include "ExplosiveComponent.generated.h"
//--------------------------------------------------------------------------------------



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UExplosiveComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UExplosiveComponent();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts
    virtual void BeginPlay() override;
    //-------------------------------------------



public:
};
