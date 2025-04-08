//

// Base:
#include "PlayerCharacter.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Interaction:
#include "FPS/ActorComponents/FPS_CharacterMovementComponent.h"
#include "FPS/ActorComponents/WeaponSlotsComponent.h"
#include "FPS/Combat/WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UFPS_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------


    /* ---   Components   --- */

    // Главный Мешь образа
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -98.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // Компонент дочернего Актора Оружия
    ChildWeaponActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("Child Weapon Actor"));
    ChildWeaponActor->SetupAttachment(GetMesh(), "WeaponSocket_HandR");

    // Камера от первого лица
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
    FirstPersonCamera->SetupAttachment(GetMesh(), "neck_01");
    FirstPersonCamera->SetRelativeLocation(FVector(17.25f, -1.2f, -7.4f));
    FirstPersonCamera->SetUsingAbsoluteRotation(true);

    FirstPersonCamera->bUsePawnControlRotation = true;

    // Компонент Слотов Оружия и взаимодействия с ним
    WeaponSlotsComponent = CreateDefaultSubobject<UWeaponSlotsComponent>(TEXT("Weapon Slots"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Components   --- */

UFPS_CharacterMovementComponent* APlayerCharacter::GetFPSCharacterMovement() const
{
    return Cast<UFPS_CharacterMovementComponent>(GetCharacterMovement());
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    Cleaning();
}

void APlayerCharacter::Cleaning()
{
    if (GetController() == UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        GetMesh()->HideBoneByName(HiddenBone, EPhysBodyOp::PBO_None);
    }
    else
    {
        FirstPersonCamera->DestroyComponent();
    }
}

TArray<FName> APlayerCharacter::GetBoneSocketsInMesh() const
{
    if (GetMesh())
    {
        return GetMesh()->GetAllSocketNames();
    }

    return TArray<FName>();
}

TArray<FName> APlayerCharacter::GetBoneNamesInMesh() const
{
    if (GetMesh())
    {
        TArray<FName> lResult;
        GetMesh()->GetBoneNames(lResult);
        return lResult;
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------



/* ---   Inputs   --- */

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);


    /* ===   Actions   === */

    /* ---   Actions | Movement   --- */

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
    //-------------------------------------------
    //===========================================


    /* ===   Axis   === */

    /* ---   Axis | Movement   --- */

    PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    //-------------------------------------------
    //===========================================
}

void APlayerCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void APlayerCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}
//--------------------------------------------------------------------------------------
