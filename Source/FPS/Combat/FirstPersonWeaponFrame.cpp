//

// Base:
#include "FirstPersonWeaponFrame.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

// Actor Components:
#include "FPS/ActorComponents/Control/SmoothMovementComponent.h"
#include "FPS/ActorComponents/Control/SmoothRotationComponent.h"

// Interaction:
#include "FPS/ActorComponents/Data/WeaponLocalController.h"
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Core/Online/FPS_PlayerController.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFirstPersonWeaponFrame::AFirstPersonWeaponFrame()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true; // Принудительно
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
    //-------------------------------------------


    /* ---   Non-scene Components   --- */

    // Компонент плавного Перемещения
    SmoothMovementComponent = CreateDefaultSubobject<USmoothMovementComponent>(TEXT("Smooth Movement Component"));
    SmoothMovementComponent->bUseRelativeLocation = true;

    // Компонент плавного Вращения
    SmoothRotationComponent = CreateDefaultSubobject<USmoothRotationComponent>(TEXT("Smooth Rotation Component"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AFirstPersonWeaponFrame::BeginPlay()
{
    Super::BeginPlay();
}

void AFirstPersonWeaponFrame::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //RotateToTraceResult();
}
//--------------------------------------------------------------------------------------



/* ---   Data   --- */

void AFirstPersonWeaponFrame::UpdateWeaponOnSelectedData(const FWeaponData* iData)
{
    Super::UpdateWeaponOnSelectedData(iData);

    if (iData)
    {
        // Направляющие:
        ShootGuidance->SetRelativeTransform(iData->ProjectileGuidanceTransform);
        CaseDropGuidance->SetRelativeTransform(iData->SleeveGuidanceTransform);
        StorageDropGuidance->SetRelativeTransform(iData->StorageGuidanceTransform);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Direction Fire   --- */
//
//void AFirstPersonWeaponFrame::RotateToTraceResult()
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
//void AFirstPersonWeaponFrame::AttachWhenStoppedAiming()
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
//void AFirstPersonWeaponFrame::AttachWhenAiming()
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

TArray<FName> AFirstPersonWeaponFrame::GetRowNamesFromWeaponsDataTable() const
{
    if (WeaponsDataTable)
    {
        return WeaponsDataTable->GetRowNames();
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------



/* ---   Editor   --- */

void AFirstPersonWeaponFrame::LoadDataFromWeaponsDataTable()
{
    if (WeaponName != NAME_None)
    {
        // Получение
        SelectedWeaponData = WeaponsDataTable->FindRow<FWeaponData>(WeaponName, "LoadDataFromWeaponsDataTable()");

        UpdateWeaponOnSelectedData(SelectedWeaponData);
    }
}

void AFirstPersonWeaponFrame::SaveCurrentDataInWeaponsDataTable()
{
    if (WeaponName != NAME_None)
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
        SelectedWeaponData->ProjectileGuidanceTransform = ShootGuidance->GetRelativeTransform();
        SelectedWeaponData->SleeveGuidanceTransform = CaseDropGuidance->GetRelativeTransform();
        SelectedWeaponData->StorageGuidanceTransform = StorageDropGuidance->GetRelativeTransform();

        // Сохранение
        WeaponsDataTable->AddRow(WeaponName, *SelectedWeaponData);
    }
}
//--------------------------------------------------------------------------------------
#endif
//======================================================================================
