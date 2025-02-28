//

// Base:
#include "PickableActor.h"

// Interaction:
#include "FPS/ActorComponents/PickupComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values
APickableActor::APickableActor()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------



    /* ---   Components   --- */

    // Корневой компонент
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    // Компонент Подбора данного Актора
    PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("Explosive Component"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts or when spawned
void APickableActor::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------
