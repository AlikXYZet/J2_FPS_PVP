//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "InteractiveComponent.h"

// Generated:
#include "PickupComponent.generated.h"
//--------------------------------------------------------------------------------------



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UPickupComponent : public UInteractiveComponent
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UPickupComponent();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts
    virtual void BeginPlay() override;
    //-------------------------------------------



public:
};
