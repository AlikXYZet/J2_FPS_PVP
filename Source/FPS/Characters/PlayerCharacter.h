//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

// Generated:
#include "PlayerCharacter.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this character's properties
    APlayerCharacter();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Inputs   --- */

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    //-------------------------------------------
};
