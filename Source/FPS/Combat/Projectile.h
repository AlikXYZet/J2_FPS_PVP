//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/Actor.h"

// Plugins:
#include "NiagaraComponent.h"

// Generated:
#include "Projectile.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class USphereComponent;
class UProjectileMovementComponent;

// GAS
class UAbilitySystemComponent;
class UGameplayEffect;
//--------------------------------------------------------------------------------------



UCLASS(Abstract)
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
    //-------------------------------------------



    /* ---   Non-scene Components   --- */

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

    /** Очистка от неиспользуемых компонентов */
    FORCEINLINE void Cleaning()
    {
        if (ProjectileMesh && !ProjectileMesh->GetStaticMesh())
            ProjectileMesh->DestroyComponent();

        if (FXComponent && !FXComponent->Template)
            FXComponent->DestroyComponent();

        if (NiagaraFXComponent && !NiagaraFXComponent->GetAsset())
            NiagaraFXComponent->DestroyComponent();
    };
    //-------------------------------------------
};
