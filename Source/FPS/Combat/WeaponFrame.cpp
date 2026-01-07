//

// Base:
#include "WeaponFrame.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Interaction:
#include "FirstPersonWeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** DELEGATE: Метод вызова делегата с Ретрансляцией по Сети */
#define COPYING_ComponentValues(ComponentName, Param) \
    ComponentName->Set##Param(lTemplate->ComponentName->Get##Param()); \
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AWeaponFrame::AWeaponFrame()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    SetActorTickInterval(0.1f); // 10 Hz

    // Настройка репликации
    bReplicates = true;
    SetReplicateMovement(false);
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

//--------------------------------------------------------------------------------------



/* ---   Data   --- */

void AWeaponFrame::UpdateWeaponOnSelectedData(const FWeaponData* iData)
{
    if (iData && iData->WeaponTemplate)
    {
        /* Производим копирование и применение необходимых данных, а не пересоздание данного Актора */

        const AFirstPersonWeaponFrame* lTemplate = iData->WeaponTemplate.GetDefaultObject();

        // Skeletal Mesh:
        WeaponSkeletalMesh->SetSkeletalMesh(lTemplate->WeaponSkeletalMesh->SkeletalMesh);
        COPYING_ComponentValues(WeaponSkeletalMesh, RelativeTransform);

        // Static Mesh:
        WeaponStaticMesh->SetStaticMesh(lTemplate->WeaponStaticMesh->GetStaticMesh());
        COPYING_ComponentValues(WeaponStaticMesh, RelativeTransform);
    }
    else if (!iData)
    {
        FPS_Error("iData is NOT");
    }
    else if (!iData->WeaponTemplate)
    {
        FPS_Error("WeaponTemplate is NOT");
    }
}
//--------------------------------------------------------------------------------------
