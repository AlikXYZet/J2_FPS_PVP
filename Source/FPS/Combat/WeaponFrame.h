//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/Actor.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"

// Generated:
#include "WeaponFrame.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class UWeaponNetworkController;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AWeaponFrame : public AActor
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this character's properties
    AWeaponFrame();
    //-------------------------------------------



    /* ---   Components   --- */

    // Компонент скелетного Меша Оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Meshes",
        meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponSkeletalMesh = nullptr;

    // Компонент статического Меша Оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Meshes",
        meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* WeaponStaticMesh = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Data   --- */

    /** Обновить Оружие по Данным */
    virtual void UpdateWeaponOnSelectedData(const FWeaponData* Data);
    //-------------------------------------------
};
