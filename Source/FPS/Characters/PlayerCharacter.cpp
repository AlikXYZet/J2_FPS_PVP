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
#include "FPS/Core/Online/FPS_GameState.h"
#include "FPS/Core/Online/FPS_PlayerController.h"
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
    SetActorTickInterval(1.f); // 1 раз/сек.
    //-------------------------------------------


    /* ---   Components   --- */

    // Главный Меш образа
    GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    // Камера от Первого лица
    FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FP Camera"));
    FPCamera->SetupAttachment(GetMesh(), "CameraSocket_Head");
    FPCamera->SetRelativeLocation(FVector(17.25f, -1.2f, -7.4f));
    FPCamera->SetUsingAbsoluteRotation(true);
    FPCamera->bUsePawnControlRotation = true;

    // Меш визуализации от Первого лица
    FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP Mesh"));
    FPMesh->SetupAttachment(GetCapsuleComponent());
    FPMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
    FPMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    FPMesh->CastShadow = false;
    //-------------------------------------------


    /* ---   Point Components   --- */

    // Компонент сетевого контроллера Оружия
    WeaponControlNetComp = CreateDefaultSubobject<UWeaponNetworkController>(TEXT("Weapon Control Network Component"));
    WeaponControlNetComp->SetupAttachment(GetMesh(), "WeaponSocket_HandR");
    WeaponControlNetComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

    // Компонент локального (не сетевого) контроллера Оружия
    WeaponControlLocComp = CreateDefaultSubobject<UWeaponLocalController>(TEXT("Weapon Control Local Component"));
    WeaponControlLocComp->SetupAttachment(FPMesh, "WeaponSocket_HandR");
    WeaponControlLocComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
    //-------------------------------------------


    /* ---   Non-scene Components   --- */

    // Компонент Системы Способностей (GAS)
    AbilitySystemComp = CreateDefaultSubobject<UFPS_AbilitySystemComponent>(TEXT("Ability System Comp"));
    AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    // Скрытый Набор Атрибутов (для GAS)
    //AttributeSet = CreateDefaultSubobject<UFPS_AttributeSet>(TEXT("Attributes"));
    // PS: Создание перенесено в `PreInitializeComponents()`, см. в обсуждении ошибки UE-81109
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
    {
        /* Чистка на стороне Клиентов:
        @note   В данный момент, на стороне Сервера, пешка Игрока-Сервера ещё не имеет Контроллера */
        if (IsLocallyControlled())
        {
            InitForLocally();
        }
        else
        {
            InitForNetwork();
        }
    }
}

//void APlayerCharacter::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//}

void APlayerCharacter::PreInitializeComponents()
{
    Super::PreInitializeComponents();

    // Скрытый Набор Атрибутов (для GAS)
    if (!AttributeSet)
    {
        AttributeSet = NewObject<UFPS_AttributeSet>(this, TEXT("Attributes"));
        AbilitySystemComp->AddAttributeSetSubobject(AttributeSet);
    }
    // PS: Создание здесь экземпляра AttributeSet через NewObject<T>(*),
    // а не в конструкторе через CreateDefaultSubobject<T>(*),
    // является решением ошибки, описанной в UE-81109:
    // "уничтожение сборщиком AttributeSet у дубликатов актора-владельца"
}

//void APlayerCharacter::PostInitializeComponents()
//{
//    Super::PostInitializeComponents();
//}

void APlayerCharacter::PossessedBy(AController* NewController)
{
    /* @note   Вызывается только на сервере (или в автономном режиме) */

    Super::PossessedBy(NewController);

    // Чистка на стороне Сервера
    if (IsLocallyControlled())
    {
        WeaponControlNetComp->InitializeForFirstPersonDisplay();
        /* @note    Данный метод на стороне Клиента вызывается самостоятельно посредством репликации
        Однако, на стороне Сервера требуется вызов для Игрока-Сервера */

        InitForLocally();
    }
    else
    {
        InitForNetwork();
    }
}

FORCEINLINE void APlayerCharacter::InitForLocally()
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    GetMesh()->SetVisibility(false);
    GetMesh()->SetCastHiddenShadow(true);

    FPMesh->HideBoneByName(HiddenBoneInFPMesh, EPhysBodyOp::PBO_None);

    CollisionParamsForTrace.AddIgnoredActor(this);

    InitAbilitySystemComp();
    InitSpeedControl();

    DisableInput(GetController<APlayerController>());

    GetController<AFPS_PlayerController>()->SetMouseControlToCenter(true);

    GetFPSGameState()->OnMatchStateChange.AddDynamic(this, &APlayerCharacter::ControlInputsBasedOnMatchStatus);

    Event_OnLocalControllerInitialization();
}

FORCEINLINE void APlayerCharacter::InitForNetwork()
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    WeaponControlLocComp->DestroyComponent();
    FPMesh->DestroyComponent();
    //FPCamera->DestroyComponent(); // Debug: Пропадает камера у Игрока Сервера
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

// Излишне
//bool APlayerCharacter::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
//{
//    return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
//}

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

void APlayerCharacter::ControlInputsBasedOnMatchStatus(EMatchState NewMatchState)
{
    switch (NewMatchState)
    {
        //case EMatchState::WaitingToStart:
        //    break;

        //case EMatchState::PreProgress:
        //    break;

    case EMatchState::InProgress:
        EnableInput(GetController<APlayerController>());
        break;

    case EMatchState::WaitingPostMatch:
        DisableInput(GetController<APlayerController>());
        break;

        //case EMatchState::LeavingMap:
        //    break;

        //case EMatchState::Aborted:
        //    break;

        //case EMatchState::Custom:
        //    break;

    default:
        break;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Movement Speed   --- */

void APlayerCharacter::SetSpeedControl(ESpeedVariations Mode)
{
    if (SpeedControl != Mode)
    {
        SpeedControl = Mode;
        GetFPSCharacterMovement()->UpdateMaxSpeed();
    }
}

FORCEINLINE void APlayerCharacter::InitSpeedControl()
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    GetFPSCharacterMovement()->AddSpeedControl(SpeedControl);
    WeaponControlLocComp->InitSpeedControl();
}
//--------------------------------------------------------------------------------------



/* ---   GAS   --- */

FORCEINLINE void APlayerCharacter::InitAbilitySystemComp()
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    if (AbilitySystemComp)
    {
        if (AttributeSet)
        {
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(Health);
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(MaxHealth);
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(Armor);
            GAMEPLAYATTRIBUTE_VALUE_Delegating_APlayerCharacter(MaxArmor);

            AttributeSet->OnZeroHealth.AddDynamic(this, &APlayerCharacter::Event_OnZeroHealth);
            AttributeSet->OnZeroArmor.AddDynamic(this, &APlayerCharacter::Event_OnZeroArmor);
        }
        else
        {
            FPS_Error("AttributeSet is NOT");
        }
    }
    else
    {
        FPS_Error("AbilitySystemComp is NOT");
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
            FPS_LOG(Warning, "Not used at least one of the Actions ('%s')",
                *Data.ToString());
        }
        else if (lArray_ActionNames.Find(Data) == INDEX_NONE)
        {
            FPS_Error("'%s' is NOT an Action",
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
            FPS_LOG(Warning, "Not used at least one of the Axes ('%s')",
                *Data.ToString());
        }
        else if (lArray_AxisNames.Find(Data) == INDEX_NONE)
        {
            FPS_Error("'%s' is NOT an Axis",
                *Data.ToString());
        }
    }
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
