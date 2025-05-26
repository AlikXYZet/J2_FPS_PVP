//

// Base:
#include "PlayerCharacter.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/ActorChannel.h"

// Net:
#include "Net/UnrealNetwork.h"

// Interaction:
#include "FPS/ActorComponents/Control/FPS_CharacterMovementComponent.h"
#include "FPS/ActorComponents/Data/WeaponControlComponent.h"
#include "FPS/Combat/WeaponFrame.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Конструктор с подменной стандартного "UCharacterMovementComponent"
APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UFPS_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------


    /* ---   Components   --- */

    // Главный Мешь образа
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // Камера от Первого лица
    FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FP Camera"));
    FPCamera->SetupAttachment(GetMesh(), "neck_01");
    FPCamera->SetRelativeLocation(FVector(17.25f, -1.2f, -7.4f));
    FPCamera->SetUsingAbsoluteRotation(true);
    FPCamera->bUsePawnControlRotation = true;

    // Мешь визуализации от Первого лица
    FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP Mesh"));
    FPMesh->SetupAttachment(FPCamera);
    FPMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
    FPMesh->CastShadow = false;
    //-------------------------------------------


    /* ---   Non-scene Components   --- */

    // Компонент Слотов Оружия и взаимодействия с ним
    WeaponControl = CreateDefaultSubobject<UWeaponControlComponent>(TEXT("Weapon Control Comp"));
    WeaponControl->SetupAttachment(GetMesh(), "WeaponSocket_HandR");
    WeaponControl->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
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
        if (AActor* lWeapon = WeaponControl->GetChildActor())
        {
            lWeapon->AttachToComponent(
                FPMesh,
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                "WeaponSocket_HandR");
        }

        GetMesh()->SetVisibility(false);
        GetMesh()->bCastHiddenShadow = true;
        GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
    }
    else
    {
        FPMesh->DestroyComponent();
        FPCamera->DestroyComponent();
    }
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

bool APlayerCharacter::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    if (WeaponControl)
        WroteSomething |= Channel->ReplicateSubobject(WeaponControl, *Bunch, *RepFlags);

    return WroteSomething;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APlayerCharacter, WeaponControl);
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

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::Sprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);
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


    /* ===   Components   === */

    if (WeaponControl)
        WeaponControl->SetupPlayerInputs();
    //===========================================
}

void APlayerCharacter::MoveForward(float Value)
{
    if (Value > 0)
    {
        if (0b01 == (IsSprinting & 0b11))
        {
            // ВКЛ режим Спринта
            IsSprinting |= 0b10;
            SetMaxWalkSpeed(SprintSpeed);
            WeaponControl->SetAction(EActionVariations::Block);
        }
    }
    else
    {
        if (IsSprinting & 0b10)
        {
            // ВЫКЛ режим Спринта
            IsSprinting ^= 0b10;
            SetMaxWalkSpeed(WalkSpeed);
            WeaponControl->StopAction(EActionVariations::Block);
        }
    }

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

void APlayerCharacter::Sprint()
{
    // ВКЛ бит команды Спринта
    IsSprinting |= 0b01;
}

void APlayerCharacter::StopSprint()
{
    // ВЫКЛ биты команды и состояния Спринта
    IsSprinting ^= 0b11;
    SetMaxWalkSpeed(WalkSpeed);
    WeaponControl->StopAction(EActionVariations::Block);
}
//--------------------------------------------------------------------------------------



/* ---   Movement Speed   --- */

void APlayerCharacter::SetMaxWalkSpeed(const float& Value)
{
    // Быстрое применение значения у Клиента-Владельца
    GetCharacterMovement()->MaxWalkSpeed = Value;

    Server_SetMaxWalkSpeed(Value);
}

void APlayerCharacter::Server_SetMaxWalkSpeed_Implementation(const float& Value)
{
    Multicast_SetMaxWalkSpeed(Value);
}

void APlayerCharacter::Multicast_SetMaxWalkSpeed_Implementation(const float& Value)
{
    // Фильтрация, если вызвал Владелец
    if (IsLocallyControlled())
        return;

    if (GetCharacterMovement())
        GetCharacterMovement()->MaxWalkSpeed = Value;
}
//--------------------------------------------------------------------------------------
