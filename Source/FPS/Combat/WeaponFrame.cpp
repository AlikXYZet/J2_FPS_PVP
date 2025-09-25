//

// Base:
#include "WeaponFrame.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Interaction:
#include "FirstPersonWeaponFrame.h"
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
    WeaponSkeletalMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    WeaponSkeletalMesh->SetGenerateOverlapEvents(false);

    // Компонент статического Меша Оружия
    WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Static Mesh"));
    WeaponStaticMesh->SetupAttachment(RootComponent);
    WeaponStaticMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    WeaponStaticMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
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
    if (iData && iData->WeaponTemplate)
    {
        const AFirstPersonWeaponFrame* lTemplate = iData->WeaponTemplate.GetDefaultObject();

        // Skeletal Mesh:
        WeaponSkeletalMesh->SetSkeletalMesh(lTemplate->WeaponSkeletalMesh->SkeletalMesh);
        WeaponSkeletalMesh->SetRelativeTransform(lTemplate->WeaponSkeletalMesh->GetRelativeTransform());

        // Static Mesh:
        WeaponStaticMesh->SetStaticMesh(lTemplate->WeaponStaticMesh->GetStaticMesh());
        WeaponStaticMesh->SetRelativeTransform(lTemplate->WeaponStaticMesh->GetRelativeTransform());
    }
    else if (!iData)
    {
        FPS_LOG(Error, "iData is NOT");
    }
    else if (!iData->WeaponTemplate)
    {
        FPS_LOG(Error, "WeaponTemplate is NOT");
    }
}
//--------------------------------------------------------------------------------------
