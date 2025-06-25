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

    /** Overridable function called whenever this actor is being removed from a level */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    // Function called every frame on this Actor
    virtual void Tick(float DeltaSeconds) override;
    //-------------------------------------------



    /* ---   Data   --- */

    /** Обновить Оружие по Данным */
    virtual void UpdateWeaponOnSelectedData(const FWeaponData* lData);
    //-------------------------------------------
};
