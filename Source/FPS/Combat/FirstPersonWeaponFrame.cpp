//

// Base:
#include "FirstPersonWeaponFrame.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

// Actor Components:
#include "FPS/ActorComponents/Control/SmoothMovementComponent.h"
#include "FPS/ActorComponents/Control/SmoothRotationComponent.h"

// Interaction:
#include "FPS/ActorComponents/Data/WeaponLocalController.h"
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Core/Online/FPS_PlayerController.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** DELEGATE: Метод вызова делегата с Ретрансляцией по Сети */
#define COPYING_ComponentValues(ComponentName, Param) \
ComponentName->Set##Param(lTemplate->ComponentName->Get##Param()); \
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFirstPersonWeaponFrame::AFirstPersonWeaponFrame()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------


    /* ---   Components   --- */

    // Компонент скелетного Меша Оружия
    WeaponSkeletalMesh->CastShadow = false;

    // Компонент статического Меша Оружия
    WeaponStaticMesh->CastShadow = false;

    // Направляющая Выстрела (красный)
    ShootGuidance = CreateDefaultSubobject<UArrowComponent>(TEXT("Shoot Guidance"));
    ShootGuidance->SetupAttachment(RootComponent);
    ShootGuidance->SetArrowColor(FLinearColor(1.f, 0.f, 0.f, 1.f));
    ShootGuidance->ArrowSize = 3.f;
    ShootGuidance->ArrowLength = 40.f;
    ShootGuidance->bIsScreenSizeScaled = true;

    // Направляющая вылета Гильзы (синий)
    CaseDropGuidance = CreateDefaultSubobject<UArrowComponent>(TEXT("Case Drop Guidance"));
    CaseDropGuidance->SetupAttachment(RootComponent);
    CaseDropGuidance->SetArrowColor(FLinearColor(0.f, 0.f, 1.f, 1.f));
    CaseDropGuidance->ArrowSize = 3.f;
    CaseDropGuidance->ArrowLength = 40.f;
    CaseDropGuidance->bIsScreenSizeScaled = true;
    CaseDropGuidance->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

    // Направляющая выпадения Накопителя (зелёный): Магазина, Обоймы и т.п.
    StorageDropGuidance = CreateDefaultSubobject<UArrowComponent>(TEXT("Storage Drop Guidance"));
    StorageDropGuidance->SetupAttachment(RootComponent);
    StorageDropGuidance->SetArrowColor(FLinearColor(0.f, 1.f, 0.f, 1.f));
    StorageDropGuidance->ArrowSize = 3.f;
    StorageDropGuidance->ArrowLength = 40.f;
    StorageDropGuidance->bIsScreenSizeScaled = true;
    StorageDropGuidance->SetRelativeRotation(FRotator(-75.f, 00.f, 0.f));

    // Точка Схвата оружия Правой рукой
    GripPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Grip Point"));
    GripPoint->SetupAttachment(RootComponent);

    // Точка Удержания оружия Левой рукой
    HoldingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Holding Point"));
    HoldingPoint->SetupAttachment(RootComponent);
    //-------------------------------------------


    /* ---   Non-scene Components   --- */

    // Компонент плавного Перемещения
    SmoothMovementComponent = CreateDefaultSubobject<USmoothMovementComponent>(TEXT("Smooth Movement Component"));
    SmoothMovementComponent->bUseRelativeLocation = true;

    // Компонент плавного Вращения
    SmoothRotationComponent = CreateDefaultSubobject<USmoothRotationComponent>(TEXT("Smooth Rotation Component"));
    SmoothRotationComponent->bUseRelativeRotation = true;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AFirstPersonWeaponFrame::BeginPlay()
{
    Super::BeginPlay();

    InitDirectionFireData();
}

void AFirstPersonWeaponFrame::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

void AFirstPersonWeaponFrame::UpdateWeaponOnSelectedData(const FWeaponData* iData)
{
    //Super::UpdateWeaponOnSelectedData(iData);

    if (iData && iData->WeaponTemplate)
    {
        const AFirstPersonWeaponFrame* lTemplate = iData->WeaponTemplate.GetDefaultObject();

        // Skeletal Mesh:
        WeaponSkeletalMesh->SetSkeletalMesh(lTemplate->WeaponSkeletalMesh->SkeletalMesh);
        COPYING_ComponentValues(WeaponSkeletalMesh, RelativeTransform);

        // Static Mesh:
        WeaponStaticMesh->SetStaticMesh(lTemplate->WeaponStaticMesh->GetStaticMesh());
        COPYING_ComponentValues(WeaponStaticMesh, RelativeTransform);

        // Направляющие:
        COPYING_ComponentValues(ShootGuidance, RelativeTransform);
        COPYING_ComponentValues(CaseDropGuidance, RelativeTransform);
        COPYING_ComponentValues(StorageDropGuidance, RelativeTransform);

        // Обновить Точки крепления к Skeletal Mesh:
        if (lTemplate->WeaponSkeletalMesh->SkeletalMesh)
        {
            GripPoint->AttachToComponent(
                WeaponSkeletalMesh,
                FAttachmentTransformRules::KeepRelativeTransform,
                lTemplate->GripSocketName);

            HoldingPoint->AttachToComponent(
                WeaponSkeletalMesh,
                FAttachmentTransformRules::KeepRelativeTransform,
                lTemplate->HoldingSocketName);
        }
        else if (lTemplate->WeaponStaticMesh->GetStaticMesh())
        {
            GripPoint->AttachToComponent(
                RootComponent,
                FAttachmentTransformRules::KeepRelativeTransform);

            HoldingPoint->AttachToComponent(
                RootComponent,
                FAttachmentTransformRules::KeepRelativeTransform);
        }

        // Точки:
        COPYING_ComponentValues(GripPoint, RelativeTransform);
        COPYING_ComponentValues(HoldingPoint, RelativeTransform);
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



/* ---   Direction Fire   --- */

void AFirstPersonWeaponFrame::InitDirectionFireData()
{
    //IsLocallyControlled();

    if (GetAttachParentActor()
        && GetAttachParentActor()->GetInstigatorController())
    {
        ParentPlayerCharacter = Cast<APlayerCharacter>(GetAttachParentActor());
        if (ParentPlayerCharacter)
        {
            ParentPlayerController = ParentPlayerCharacter->GetController<AFPS_PlayerController>();
            if (!ParentPlayerController)
            {
                FPS_LOG(Error, TEXT("ParentPlayerController is NOT"));
            }
        }
        else
        {
            FPS_LOG(Error, TEXT("ParentPlayerCharacter is NOT"));
        }
    }
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Base   --- */

TArray<FName> AFirstPersonWeaponFrame::GetSocketNamesInSkeletalMesh() const
{
    if (WeaponSkeletalMesh)
    {
        return WeaponSkeletalMesh->GetAllSocketNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
