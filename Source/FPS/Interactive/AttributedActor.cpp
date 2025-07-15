// 

// Base:
#include "AttributedActor.h"

// Interaction | GAS:
#include "FPS/GAS/FPS_AttributeSet.h"
//--------------------------------------------------------------------------------------



/* ---   Macros   --- */

/** Макрос: Подписка функции к делегату для передачи значения атрибутов GAS через Событие BP */
#define GAMEPLAYATTRIBUTE_VALUE_Delegating(PropertyName) \
    AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AttributeSet->Get##PropertyName##Attribute()) \
        .AddUObject(this, &AAttributedActor::Handle##PropertyName##Changed);
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AAttributedActor::AAttributedActor()
{
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false; // Предварительно

    // Настройка репликации
    bReplicates = true;
    SetReplicateMovement(true);
    //-------------------------------------------


    /* ---   Components   --- */

    // Меш визуализации
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FP Camera"));
    RootComponent = Mesh;
    //-------------------------------------------


    /* ---   Non-scene Components   --- */

    // Компонент Системы Способностей (GAS)
    AbilitySystemComp = CreateDefaultSubobject<UFPS_AbilitySystemComponent>(TEXT("Ability System Comp"));
    AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    // Скрытый компонент Набора Атрибутов (для GAS)
    AttributeSet = CreateDefaultSubobject<UFPS_AttributeSet>(TEXT("Attributes"));
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AAttributedActor::BeginPlay()
{
    Super::BeginPlay();

    InitAbilitySystemComp();
}
//--------------------------------------------------------------------------------------



/* ---   GAS   --- */

void AAttributedActor::InitAbilitySystemComp()
{
    AbilitySystemComp->InitAbilityActorInfo(this, this);

    GAMEPLAYATTRIBUTE_VALUE_Delegating(Health);
    GAMEPLAYATTRIBUTE_VALUE_Delegating(MaxHealth);
    GAMEPLAYATTRIBUTE_VALUE_Delegating(Armor);
    GAMEPLAYATTRIBUTE_VALUE_Delegating(MaxArmor);

    AttributeSet->OnZeroHealth.AddDynamic(this, &AAttributedActor::Event_OnZeroHealth);
}
//--------------------------------------------------------------------------------------
