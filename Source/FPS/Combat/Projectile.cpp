//

// Base:
#include "Projectile.h"

// UE:
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// GAS:
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AProjectile::AProjectile()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно

    // Репликация неуместна:
    //bReplicates = true;
    //SetReplicateMovement(true);
    //-------------------------------------------


    /* ---   Components   --- */

    // Капсула коллизии Снаряда
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
    RootComponent = SphereComponent;
    SphereComponent->SetRelativeScale3D(FVector(0.2f));
    SphereComponent->SetCollisionProfileName(TEXT("InvisibleWallDynamic"));
    SphereComponent->SetUseCCD(true);

    // Корневой компонент
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMesh->SetRelativeScale3D(FVector(0.63f));
    ProjectileMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    ProjectileMesh->SetGenerateOverlapEvents(false);
    ProjectileMesh->SetCastShadow(false);

    // FX
    FXComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FX"));
    FXComponent->SetupAttachment(RootComponent);

    // FX Niagara
    NiagaraFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara FX"));
    NiagaraFXComponent->SetupAttachment(RootComponent);

    // Компонент перемещения Снаряда
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
    ProjectileMovement->InitialSpeed = 3000;
    ProjectileMovement->MaxSpeed = 3000;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Velocity = FVector::XAxisVector;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    Cleaning();
}

void AProjectile::NotifyHit(
    UPrimitiveComponent* MyComp,
    AActor* Other,
    UPrimitiveComponent* OtherComp,
    bool bSelfMoved,
    FVector HitLocation,
    FVector HitNormal,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    if (HasAuthority()
        && Other
        && DamageEffect
        && InstigatorASC)
    {
        IAbilitySystemInterface* TargetInterface = Cast<IAbilitySystemInterface>(Other);
        if (TargetInterface)
        {
            InstigatorASC->ApplyGameplayEffectToTarget(
                DamageEffect.GetDefaultObject(),
                TargetInterface->GetAbilitySystemComponent(),
                0,
                FGameplayEffectContextHandle());
        }
    }

    Destroy();
}
//--------------------------------------------------------------------------------------
