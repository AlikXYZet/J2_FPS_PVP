//

// Base:
#include "Weapon.h"

// Interaction:
#include "Projectile.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values
AWeapon::AWeapon()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------
