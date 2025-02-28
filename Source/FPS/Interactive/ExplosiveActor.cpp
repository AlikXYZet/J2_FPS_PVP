//

// Base:
#include "ExplosiveActor.h"

// Interaction:
#include "FPS/ActorComponents/ExplosiveComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values
AExplosiveActor::AExplosiveActor()
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

    // Компонент Взрыва данного Актора
    ExplosiveComponent = CreateDefaultSubobject<UExplosiveComponent>(TEXT("Explosive Component"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts or when spawned
void AExplosiveActor::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------
