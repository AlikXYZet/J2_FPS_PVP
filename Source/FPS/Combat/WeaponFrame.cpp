//

// Base:
#include "WeaponFrame.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

// Actor Components:
#include "FPS/ActorComponents/Control/SmoothMovementComponent.h"
#include "FPS/ActorComponents/Control/SmoothRotationComponent.h"

// Interaction:
#include "FPS/Core/Online/FPS_PlayerController.h"
#include "Projectile.h"
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

    //

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

    //

    // Компонент плавного Перемещения
    SmoothMovementComponent = CreateDefaultSubobject<USmoothMovementComponent>(TEXT("Smooth Movement Component"));
    SmoothMovementComponent->bUseRelativeLocation = true;

    // Компонент плавного Вращения
    SmoothRotationComponent = CreateDefaultSubobject<USmoothRotationComponent>(TEXT("Smooth Rotation Component"));
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

    //RotateToTraceResult();
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

void AWeaponFrame::UpdateWeaponByName(const FName& iName)
{
    if (&iName
        && iName != NAME_None
        && WeaponsDataTable)
    {
        FWeaponData* lRow = WeaponsDataTable->FindRow<FWeaponData>(iName, "UpdateWeaponByName");
        
        if (lRow)
        {
            SelectedWeaponData = lRow;

            UpdateWeaponOnSelectedData();
        }
    }
}

void AWeaponFrame::UpdateWeaponOnSelectedData()
{
    // Меш:
    if (SelectedWeaponData->SkeletalMesh)
    {
        WeaponSkeletalMesh->SetSkeletalMesh(SelectedWeaponData->SkeletalMesh);
        WeaponSkeletalMesh->SetRelativeTransform(SelectedWeaponData->MeshTransform);
        WeaponStaticMesh->SetStaticMesh(nullptr);
    }
    else if (SelectedWeaponData->StaticMesh)
    {
        WeaponStaticMesh->SetStaticMesh(SelectedWeaponData->StaticMesh);
        WeaponStaticMesh->SetRelativeTransform(SelectedWeaponData->MeshTransform);
        WeaponSkeletalMesh->SetSkeletalMesh(nullptr);
    }

    // Направляющие:
    ShootGuidance->SetRelativeTransform(SelectedWeaponData->ShootGuidanceTransform);
    CaseDropGuidance->SetRelativeTransform(SelectedWeaponData->CaseDropGuidanceTransform);
    StorageDropGuidance->SetRelativeTransform(SelectedWeaponData->StorageDropGuidanceTransform);
}
//--------------------------------------------------------------------------------------



/* ---   Direction Fire   --- */
//
//void AWeaponFrame::RotateToTraceResult()
//{
//    if (!bIsAiming && ParentPlayerController)
//    {
//        FHitResult lHitResult;
//
//        if (!ParentPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, lHitResult))
//        {
//            lHitResult.Location = ParentPlayerCharacter->FPCamera->GetComponentTransform()
//                .TransformPosition(FVector(1000.f, 0.f, 0.f));
//        }
//
//        SmoothRotationComponent->RotateToLocation(lHitResult.Location);
//    }
//}
//
//void AWeaponFrame::AttachWhenStoppedAiming()
//{
//    bIsAiming = false;
//
//    AttachToComponent(
//        ParentPlayerCharacter->FPMesh,
//        FAttachmentTransformRules::KeepWorldTransform);
//
//    SmoothMovementComponent->MoveToLocation(SelectedWeaponData->HipLocation);
//}
//
//void AWeaponFrame::AttachWhenAiming()
//{
//    bIsAiming = true;
//
//    AttachToComponent(
//        ParentPlayerCharacter->FPCamera,
//        FAttachmentTransformRules::KeepWorldTransform);
//
//    SmoothRotationComponent->StopRotate();
//    SetActorRelativeRotation(FRotator::ZeroRotator);
//
//    SmoothMovementComponent->MoveToLocation(SelectedWeaponData->AimingLocation);
//}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Data   --- */

TArray<FName> AWeaponFrame::GetRowNamesFromWeaponsDataTable() const
{
    if (WeaponsDataTable)
    {
        return WeaponsDataTable->GetRowNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------



/* ---   Editor   --- */

void AWeaponFrame::LoadDataFromWeaponsDataTable()
{
    if (SelectedWeapon != NAME_None)
    {
        // Получение
        SelectedWeaponData = WeaponsDataTable->FindRow<FWeaponData>(SelectedWeapon, "LoadDataFromWeaponsDataTable()");

        UpdateWeaponOnSelectedData();
    }
}

void AWeaponFrame::SaveCurrentDataInWeaponsDataTable()
{
    if (SelectedWeapon != NAME_None)
    {
        // Меш:
        if (WeaponSkeletalMesh->SkeletalMesh)
        {
            SelectedWeaponData->SkeletalMesh = WeaponSkeletalMesh->SkeletalMesh;
            SelectedWeaponData->MeshTransform = WeaponSkeletalMesh->GetRelativeTransform();
        }
        else if (WeaponStaticMesh->GetStaticMesh())
        {
            SelectedWeaponData->StaticMesh = WeaponStaticMesh->GetStaticMesh();
            SelectedWeaponData->MeshTransform = WeaponStaticMesh->GetRelativeTransform();
        }

        // Направляющие:
        SelectedWeaponData->ShootGuidanceTransform = ShootGuidance->GetRelativeTransform();
        SelectedWeaponData->CaseDropGuidanceTransform = CaseDropGuidance->GetRelativeTransform();
        SelectedWeaponData->StorageDropGuidanceTransform = StorageDropGuidance->GetRelativeTransform();

        // Сохранение
        WeaponsDataTable->AddRow(SelectedWeapon, *SelectedWeaponData);
    }
}
//--------------------------------------------------------------------------------------
#endif
//======================================================================================
