//

// Base:
#include "PlayerCharacter.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/InputSettings.h"

// Net:
#include "Net/UnrealNetwork.h"

// GAS:
#include "FPS/GAS/FPS_AttributeSet.h"

// Interaction:
#include "FPS/ActorComponents/Control/FPS_CharacterMovementComponent.h"
#include "FPS/ActorComponents/Data/WeaponLocalController.h"
#include "FPS/ActorComponents/Data/WeaponNetworkController.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** Макрос: Подписка функции к делегату для передачи значения атрибутов GAS через Событие BP */
#define GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_Delegating(APlayerCharacter, PropertyName)
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Конструктор с подменной стандартного "UCharacterMovementComponent"
// на его дочерний тип "UFPS_CharacterMovementComponent"
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
    FPMesh->SetupAttachment(GetCapsuleComponent());
    FPMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    FPMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    FPMesh->CastShadow = false;
    //-------------------------------------------


    /* ---   Non-scene Components   --- */

    // Компонент сетевого контроллера Оружия
    WeaponControlNetComp = CreateDefaultSubobject<UWeaponNetworkController>(TEXT("Weapon Control Network Component"));
    WeaponControlNetComp->SetupAttachment(GetMesh(), "WeaponSocket_HandR");
    WeaponControlNetComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    // Компонент локального (не сетевого) контроллера Оружия
    WeaponControlLocComp = CreateDefaultSubobject<UWeaponLocalController>(TEXT("Weapon Control Local Component"));
    WeaponControlLocComp->SetupAttachment(FPMesh, "WeaponSocket_HandR");
    WeaponControlLocComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    // Компонент Системы Способностей (GAS)
    AbilitySystemComp = CreateDefaultSubobject<UFPS_AbilitySystemComponent>(TEXT("Ability System Comp"));
    AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // Скрытый Набор Атрибутов (для GAS)
    AttributeSet = CreateDefaultSubobject<UFPS_AttributeSet>(TEXT("Attributes"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocallyControlled())
    {
        CleaningForLocally();

        InitAbilitySystemComp();
        InitSpeedControl();
    }
    else
    {
        CleaningForNetwork();
    }
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
}

void APlayerCharacter::CleaningForLocally()
{
    GetMesh()->SetVisibility(false);
    GetMesh()->SetCastHiddenShadow(true);

    FPMesh->HideBoneByName(HiddenBoneInFPMesh, EPhysBodyOp::PBO_None);

    WeaponControlNetComp->InitializeForFirstPersonDisplay();
}

void APlayerCharacter::CleaningForNetwork()
{
    WeaponControlLocComp->DestroyComponent();
    FPMesh->DestroyComponent();
    FPCamera->DestroyComponent();
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

bool APlayerCharacter::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

    if (WeaponControlNetComp)
        WroteSomething |= Channel->ReplicateSubobject(WeaponControlNetComp, *Bunch, *RepFlags);

    return WroteSomething;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //-------------------------------------------


    /* ---   Weapon Control   --- */

    DOREPLIFETIME(APlayerCharacter, WeaponControlNetComp);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Net | OnRep   --- */

void APlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    AbilitySystemComp->InitAbilityActorInfo(this, this);
}
//--------------------------------------------------------------------------------------



/* ---   Inputs   --- */

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);


    /* ===   Actions   === */

    /* ---   Actions | Movement   --- */
    if (ActionGroups_Jump != NAME_None)
    {
        PlayerInputComponent->BindAction(ActionGroups_Jump, IE_Pressed, this, &ACharacter::Jump);
        PlayerInputComponent->BindAction(ActionGroups_Jump, IE_Released, this, &ACharacter::StopJumping);
    }

    if (ActionGroups_Sprint != NAME_None)
    {
        PlayerInputComponent->BindAction(ActionGroups_Sprint, IE_Pressed, this, &APlayerCharacter::Sprint);
        PlayerInputComponent->BindAction(ActionGroups_Sprint, IE_Released, this, &APlayerCharacter::StopSprint);
    }
    //-------------------------------------------
    //===========================================


    /* ===   Axis   === */

    /* ---   Axis | Movement   --- */

    if (AxisGroups_MoveForward != NAME_None)
        PlayerInputComponent->BindAxis(AxisGroups_MoveForward, this, &APlayerCharacter::MoveForward);

    if (AxisGroups_MoveRight != NAME_None)
        PlayerInputComponent->BindAxis(AxisGroups_MoveRight, this, &APlayerCharacter::MoveRight);

    if (AxisGroups_Turn != NAME_None)
        PlayerInputComponent->BindAxis(AxisGroups_Turn, this, &APawn::AddControllerYawInput);

    if (AxisGroups_LookUp != NAME_None)
        PlayerInputComponent->BindAxis(AxisGroups_LookUp, this, &APawn::AddControllerPitchInput);
    //-------------------------------------------
    //===========================================


    /* ===   Components   === */

    if (WeaponControlLocComp)
        WeaponControlLocComp->SetupPlayerInputs();
    //===========================================


    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Inputs   --- */

    CheckInputsGroups();
    //-------------------------------------------

#endif // WITH_EDITOR
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
            SetSpeedControl(ESpeedVariations::Sprint);
        }
    }
    else
    {
        if (IsSprinting & 0b10)
        {
            // ВЫКЛ режим Спринта
            IsSprinting ^= 0b10;
            SetSpeedControl(ESpeedVariations::Jog);
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
    SetSpeedControl(ESpeedVariations::Jog);
}
//--------------------------------------------------------------------------------------



/* ---   Movement Speed   --- */

UFPS_CharacterMovementComponent* APlayerCharacter::BP_GetFPSCharacterMovement() const
{
    return GetFPSCharacterMovement();
}

void APlayerCharacter::SetSpeedControl(const ESpeedVariations& Mode)
{
    if (SpeedControl != Mode)
    {
        SpeedControl = Mode;
        GetFPSCharacterMovement()->UpdateMaxSpeed();
    }
}

void APlayerCharacter::InitSpeedControl()
{
    GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
    SetSpeedControl(ESpeedVariations::Jog);
}
//--------------------------------------------------------------------------------------



/* ---   GAS   --- */

void APlayerCharacter::InitAbilitySystemComp()
{
    if (AbilitySystemComp)
    {
        if (AttributeSet)
        {
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(Health);
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(MaxHealth);
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(Armor);
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(MaxArmor);

            AttributeSet->OnZeroHealth.AddDynamic(this, &APlayerCharacter::Event_OnZeroHealth);
        }
        else
        {
            FPS_LOG(Error, TEXT("AttributeSet is NOT"));
        }
    }
    else
    {
        FPS_LOG(Error, TEXT("AbilitySystemComp is NOT"));
    }
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Base   --- */

TArray<FName> APlayerCharacter::GetBoneNamesInFPMesh() const
{
    if (FPMesh)
    {
        TArray<FName> lResult;
        FPMesh->GetBoneNames(lResult);
        return lResult;
    }

    return TArray<FName>();
}
//--------------------------------------------------------------------------------------



/* ---   Inputs   --- */

TArray<FName> APlayerCharacter::GetActionGroupsNames()
{
    TArray<FName> ActionNames;

    UInputSettings::GetInputSettings()->GetActionNames(ActionNames);

    return ActionNames;
}

TArray<FName> APlayerCharacter::GetAxisGroupsNames()
{
    TArray<FName> AxisNames;

    UInputSettings::GetInputSettings()->GetAxisNames(AxisNames);

    return AxisNames;
}

void APlayerCharacter::CheckInputsGroups()
{
    TArray<FName> lUsed_Actions = {
        ActionGroups_Jump,
        ActionGroups_Sprint,
    };

    TArray<FName> lArray_ActionNames;
    UInputSettings::GetInputSettings()->GetActionNames(lArray_ActionNames);

    for (FName& Data : lUsed_Actions)
    {
        if (Data == NAME_None)
        {
            FPS_LOG(Warning, TEXT("Not used at least one of the Actions ('%s')"),
                *Data.ToString());
        }
        else if (lArray_ActionNames.Find(Data) == INDEX_NONE)
        {
            FPS_LOG(Error, TEXT("'%s' is NOT an Action"),
                *Data.ToString());
        }
    }

    TArray<FName> lUsed_Axis = {
        AxisGroups_MoveForward,
        AxisGroups_MoveRight,
        AxisGroups_Turn,
        AxisGroups_LookUp,
    };

    TArray<FName> lArray_AxisNames;
    UInputSettings::GetInputSettings()->GetAxisNames(lArray_AxisNames);

    for (FName& Data : lUsed_Axis)
    {
        if (Data == NAME_None)
        {
            FPS_LOG(Warning, TEXT("Not used at least one of the Axes ('%s')"),
                *Data.ToString());
        }
        else if (lArray_AxisNames.Find(Data) == INDEX_NONE)
        {
            FPS_LOG(Error, TEXT("'%s' is NOT an Axis"),
                *Data.ToString());
        }
    }
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
