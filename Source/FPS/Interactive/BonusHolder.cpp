//

// Base:
#include "BonusHolder.h"

// Interaction:
#include "PickableActor.h"
#include "FPS/ActorComponents/PickupComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values
ABonusHolder::ABonusHolder()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------



    /* ---   Components   --- */

    // Корневой компонент
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts or when spawned
void ABonusHolder::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------
