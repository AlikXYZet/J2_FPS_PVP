//

// Base:
#include "ExplosiveComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values for this component's properties
UExplosiveComponent::UExplosiveComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts
void UExplosiveComponent::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------
