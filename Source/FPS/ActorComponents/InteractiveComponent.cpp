//

// Base:
#include "InteractiveComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values for this component's properties
UInteractiveComponent::UInteractiveComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts
void UInteractiveComponent::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------
