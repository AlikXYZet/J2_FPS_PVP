//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Generated:
#include "Projectile.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class USphereComponent;
class UProjectileMovementComponent;

// Plugins:
class UNiagaraComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AProjectile : public AActor
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this character's properties
    AProjectile();
    //-------------------------------------------



    /* ---   Components   --- */

    // Сферическая коллизии Снаряда
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    USphereComponent* SphereComponent = nullptr;

    // Компонент статического Меша визуализации Снаряда
    // @note    Эффективнее будет заменить на Particle или Niagara System
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* ProjectileMesh = nullptr;

    // Компонент Партикла визуализации Снаряда
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UParticleSystemComponent* FXComponent = nullptr;

    // Компонент Партикла визуализации Снаряда
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UNiagaraComponent* NiagaraFXComponent = nullptr;

    // Компонент перемещения Снаряда
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UProjectileMovementComponent* ProjectileMovement = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:


private:

    /* ---   Base   --- */

    void Clearing();
    //-------------------------------------------
};
