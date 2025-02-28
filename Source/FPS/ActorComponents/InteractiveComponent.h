//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

// Generated:
#include "InteractiveComponent.generated.h"
//--------------------------------------------------------------------------------------



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UInteractiveComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UInteractiveComponent();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts
    virtual void BeginPlay() override;
    //-------------------------------------------



public:
};
