//

// Base:
#include "WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AWeaponFrame::AWeaponFrame()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно

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
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

void AWeaponFrame::UpdateWeaponOnSelectedData(const FWeaponData* iData)
{
    if (iData)
    {
        // Меш:
        if (iData->SkeletalMesh)
        {
            WeaponSkeletalMesh->SetSkeletalMesh(iData->SkeletalMesh);
            WeaponSkeletalMesh->SetRelativeTransform(iData->MeshTransform);
            WeaponStaticMesh->SetStaticMesh(nullptr);
        }
        else if (iData->StaticMesh)
        {
            WeaponStaticMesh->SetStaticMesh(iData->StaticMesh);
            WeaponStaticMesh->SetRelativeTransform(iData->MeshTransform);
            WeaponSkeletalMesh->SetSkeletalMesh(nullptr);
        }
    }
}
//--------------------------------------------------------------------------------------
