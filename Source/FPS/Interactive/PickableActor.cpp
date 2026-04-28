//

// Base:
#include "PickableActor.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// Interaction:
#include "FPS/ActorComponents/Properties/InteractiveComponent.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values
APickableActor::APickableActor()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно

    // Настройка репликации
    bReplicates = true;
    SetReplicateMovement(true);
    //-------------------------------------------



    /* ---   Components   --- */

    // Корневой компонент
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Меш визуализации
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
    StaticMesh->SetupAttachment(RootComponent);
    StaticMesh->SetCollisionProfileName(ProfileName_Pickables);
    StaticMesh->SetCastShadow(false);
    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    // Компонент Подбора данного Актора
    InteractiveComponent = CreateDefaultSubobject<UInteractiveComponent>(TEXT("Interactive Component"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void APickableActor::BeginPlay()
{
    Super::BeginPlay();
}
//--------------------------------------------------------------------------------------



/* ---   Interactive | Highlighting   --- */

TArray<FComponentRendering> APickableActor::GetUsedComponents_Implementation()
{
    return TArray<FComponentRendering>{FComponentRendering(StaticMesh)};
}
//--------------------------------------------------------------------------------------



/* ---   Activity   --- */

void APickableActor::StartInactiveState()
{
    GetWorldTimerManager().SetTimer(
        Timer_ActivityControl,
        this,
        &APickableActor::Event_OnRestartingActivityState,
        InactiveState_Time,
        false);
}
//--------------------------------------------------------------------------------------
