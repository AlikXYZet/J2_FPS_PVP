//

// Base:
#include "PlayerCharacter.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values
APlayerCharacter::APlayerCharacter()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}
//--------------------------------------------------------------------------------------
