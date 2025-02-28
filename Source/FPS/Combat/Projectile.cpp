//

// Base:
#include "Projectile.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values
AProjectile::AProjectile()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------
