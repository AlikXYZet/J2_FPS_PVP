//

// Base:
#include "WeaponFrame.h"

// Interaction:
#include <FPS/ActorComponents/Data/WeaponControlComponent.h>
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AWeaponFrame::AWeaponFrame()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true; // Принудительно

    SetActorTickInterval(0.1f); // 10 Hz
    //-------------------------------------------


    /* ---   Components   --- */

    // Корневой компонент
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Компонент скелетного Меша Оружия
    WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh"));
    WeaponSkeletalMesh->SetupAttachment(RootComponent);
    WeaponSkeletalMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    WeaponSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));
    WeaponSkeletalMesh->SetGenerateOverlapEvents(false);

    // Компонент статического Меша Оружия
    WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Static Mesh"));
    WeaponStaticMesh->SetupAttachment(RootComponent);
    WeaponStaticMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    WeaponStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
    WeaponStaticMesh->SetGenerateOverlapEvents(false);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AWeaponFrame::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponFrame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AWeaponFrame::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

void AWeaponFrame::UpdateWeaponOnSelectedData(const FWeaponData* lData)
{
    if (lData)
    {
        // Меш:
        if (lData->SkeletalMesh)
        {
            WeaponSkeletalMesh->SetSkeletalMesh(lData->SkeletalMesh);
            WeaponSkeletalMesh->SetRelativeTransform(lData->MeshTransform);
            WeaponStaticMesh->SetStaticMesh(nullptr);
        }
        else if (lData->StaticMesh)
        {
            WeaponStaticMesh->SetStaticMesh(lData->StaticMesh);
            WeaponStaticMesh->SetRelativeTransform(lData->MeshTransform);
            WeaponSkeletalMesh->SetSkeletalMesh(nullptr);
        }
    }
}
//--------------------------------------------------------------------------------------
