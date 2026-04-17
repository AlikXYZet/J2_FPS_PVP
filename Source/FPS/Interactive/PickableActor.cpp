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

    // Главный Меш
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
    StaticMesh->SetupAttachment(RootComponent);
    //-------------------------------------------



    /* ---   Non-scene Components   --- */

    // Компонент Подбора данного Актора
    InteractiveComponent = CreateDefaultSubobject<UInteractiveComponent>(TEXT("Interactive Component"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts or when spawned
void APickableActor::BeginPlay()
{
    InitInteractiveComponent();

    Super::BeginPlay();
}
//--------------------------------------------------------------------------------------



/* ---   Interaction   --- */

void APickableActor::InitInteractiveComponent()
{
    //InteractiveComponent->UsedComponents.AddUnique({ StaticMesh->GetFName(), StaticMesh, 0});
    InteractiveComponent->AddNamePredicate("CheckHighlighting");
    //InteractiveComponent->OnOwnerWasClicked.AddDynamic(this, &IInteractiveInterface::ProcessInteractiveAction);
}
//--------------------------------------------------------------------------------------
