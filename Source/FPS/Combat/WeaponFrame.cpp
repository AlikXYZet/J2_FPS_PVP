//

// Base:
#include "WeaponFrame.h"

// UE:
#include "Components/ArrowComponent.h"

// Interaction:
#include "Projectile.h"
#include "FPS/ActorComponents/WeaponSlotsComponent.h"
#include "FPS/Characters/PlayerCharacter.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AWeaponFrame::AWeaponFrame()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------


    /* ---   Components   --- */

    // Корневой компонент
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Компонент скелетного Меша Оружия
    WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh"));
    WeaponSkeletalMesh->SetupAttachment(RootComponent);
    WeaponSkeletalMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // Компонент статического Меша Оружия
    WeaponStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Static Mesh"));
    WeaponStaticMesh->SetupAttachment(RootComponent);
    WeaponStaticMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

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

}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AWeaponFrame::BeginPlay()
{
    Super::BeginPlay();

    WeaponFrameInit();
    SetupPlayerInputs();

    if (ParentPlayerCharacter)
    {
        StopAiming();
    }
}

void AWeaponFrame::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RemovePlayerInputs();

    Super::EndPlay(EndPlayReason);
}
//--------------------------------------------------------------------------------------



/* ---   Weapon | Editor   --- */

void AWeaponFrame::LoadDataFromWeaponsDataTable()
{
    if (SelectedWeapon != "NONE")
    {
        // Получение
        SelectedWeaponData = *WeaponsDataTable->FindRow<FWeaponData>(SelectedWeapon, "LoadDataFromWeaponsDataTable()");

        UpdateWeaponOnSelectedData();
    }
}

void AWeaponFrame::SaveCurrentDataInWeaponsDataTable()
{
    if (SelectedWeapon != "NONE")
    {
        // Меш:
        if (WeaponSkeletalMesh->SkeletalMesh)
        {
            SelectedWeaponData.SkeletalMesh = WeaponSkeletalMesh->SkeletalMesh;
            SelectedWeaponData.MeshTransform = WeaponSkeletalMesh->GetRelativeTransform();
        }
        else if (WeaponStaticMesh->GetStaticMesh())
        {
            SelectedWeaponData.StaticMesh = WeaponStaticMesh->GetStaticMesh();
            SelectedWeaponData.MeshTransform = WeaponStaticMesh->GetRelativeTransform();
        }

        // Направляющие:
        SelectedWeaponData.ShootGuidanceTransform = ShootGuidance->GetRelativeTransform();
        SelectedWeaponData.CaseDropGuidanceTransform = CaseDropGuidance->GetRelativeTransform();
        SelectedWeaponData.StorageDropGuidanceTransform = StorageDropGuidance->GetRelativeTransform();

        // Сохранение
        WeaponsDataTable->AddRow(SelectedWeapon, SelectedWeaponData);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Weapon | Data   --- */

void AWeaponFrame::SetCurrentWeaponSlotsComponent(UWeaponSlotsComponent* iNewComponent)
{
    CurrentWeaponSlotsComponent = iNewComponent;
}

void AWeaponFrame::UpdateWeaponByName(const FName& iName)
{
    if (&iName
        && iName != "NONE"
        && WeaponsDataTable)
    {
        FWeaponData* lRow = WeaponsDataTable->FindRow<FWeaponData>(iName, "UpdateWeaponByName");

        if (lRow)
        {
            SelectedWeaponData = *lRow;

            UpdateWeaponOnSelectedData();
        }
    }
}

TArray<FName> AWeaponFrame::GetRowNamesFromWeaponsDataTable() const
{
    if (WeaponsDataTable)
    {
        return WeaponsDataTable->GetRowNames();
    }

    return TArray<FName>();
}

void AWeaponFrame::UpdateWeaponOnSelectedData()
{
    // Меш:
    if (SelectedWeaponData.SkeletalMesh)
    {
        WeaponSkeletalMesh->SetSkeletalMesh(SelectedWeaponData.SkeletalMesh);
        WeaponSkeletalMesh->SetRelativeTransform(SelectedWeaponData.MeshTransform);
        WeaponStaticMesh->SetStaticMesh(nullptr);
    }
    else if (SelectedWeaponData.StaticMesh)
    {
        WeaponStaticMesh->SetStaticMesh(SelectedWeaponData.StaticMesh);
        WeaponStaticMesh->SetRelativeTransform(SelectedWeaponData.MeshTransform);
        WeaponSkeletalMesh->SetSkeletalMesh(nullptr);
    }

    // Направляющие:
    ShootGuidance->SetRelativeTransform(SelectedWeaponData.ShootGuidanceTransform);
    CaseDropGuidance->SetRelativeTransform(SelectedWeaponData.CaseDropGuidanceTransform);
    StorageDropGuidance->SetRelativeTransform(SelectedWeaponData.StorageDropGuidanceTransform);
}
//--------------------------------------------------------------------------------------



/* ---   Weapon | Control   --- */

void AWeaponFrame::Fire()
{
    if (CurrentWeaponSlotsComponent)
    {
        FWeaponSlotData* lSlotData = &CurrentWeaponSlotsComponent->GetCurrentSlotData();

        if (lSlotData)
        {
            if (lSlotData->bIsWeaponLoaded
                && lSlotData->NumPreparedCartridges)
            {

            }
            else if (lSlotData->NumAllCartridge)
            {
                Reload();
            }
        }
    }
}

void AWeaponFrame::Reload()
{
    if (CurrentWeaponSlotsComponent)
    {
        FWeaponSlotData* lSlotData = &CurrentWeaponSlotsComponent->GetCurrentSlotData();

        if (lSlotData)
        {
            if (lSlotData->NumAllCartridge)
            {
                // Обойма не заполнена?
                if (lSlotData->NumPreparedCartridges < SelectedWeaponData.MaxPreparedCartridges)
                {

                }
                // Оружие не заряжено?
                else if (lSlotData->bIsWeaponLoaded && SelectedWeaponData.bIsLoadable)
                {

                }
            }
        }
    }
}

void AWeaponFrame::Aiming()
{
    PlayCharacterAnim(SelectedWeaponData.Aiming);
}

void AWeaponFrame::StopAiming()
{
    PlayCharacterAnim(SelectedWeaponData.FromHip);
}

void AWeaponFrame::WeaponFrameInit()
{
    ParentPlayerCharacter = Cast<APlayerCharacter>(GetParentActor());

    if (!ParentPlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("'%s'::WeaponFrameInit: ParentPlayerCharacter is NOT"),
            *GetNameSafe(this));
    }
}

void AWeaponFrame::SetupPlayerInputs()
{
    if (ParentPlayerCharacter)
    {
        UInputComponent* lInputComponent = ParentPlayerCharacter->InputComponent;

        if (lInputComponent)
        {
            lInputComponent->BindAction("Fire", IE_Pressed, this, &AWeaponFrame::Fire);

            lInputComponent->BindAction("Aiming", IE_Pressed, this, &AWeaponFrame::Aiming);
            lInputComponent->BindAction("Aiming", IE_Released, this, &AWeaponFrame::StopAiming);
        }
    }
}

void AWeaponFrame::RemovePlayerInputs()
{
    if (ParentPlayerCharacter)
    {
        UInputComponent* lInputComponent = Cast<APawn>(GetParentActor())->InputComponent;

        if (lInputComponent)
        {
            lInputComponent->RemoveActionBinding("Fire", IE_Pressed);

            lInputComponent->RemoveActionBinding("Aiming", IE_Pressed);
            lInputComponent->RemoveActionBinding("Aiming", IE_Released);
        }
    }
}

void AWeaponFrame::PlayCharacterAnim(UAnimMontage* iAnimMontage)
{
    if (iAnimMontage)
    {
        ParentPlayerCharacter->PlayAnimMontage(iAnimMontage);
    }
    else
    {
        ParentPlayerCharacter->StopAnimMontage();
    }
}
//--------------------------------------------------------------------------------------
