//

// Base:
#include "InteractiveComponent.h"

// Win:
#if PLATFORM_WINDOWS
#include "excpt.h"
#endif // PLATFORM_WINDOWS

// UE:
#include "GameFramework/InputSettings.h"
//--------------------------------------------------------------------------------------



/* ---   define   --- */

//#define CHECKER_UE_LOG(Text) UE_LOG(LogTemp, Error, TEXT("%s::%s: %s"), *GetNameSafe(this), *FString(__func__), *FString(Text))
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

// Sets default values for this component's properties
UInteractiveComponent::UInteractiveComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.
    // You can turn these features off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false; // Предварительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

// Called when the game starts
void UInteractiveComponent::BeginPlay()
{
    Super::BeginPlay();


    /* ---   Highlighting   --- */

    InitUsedComponents();
    InitActionGroup();
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Global   --- */

/* Проверка безопасности функции через попытку её вызова
@return   Безопасна ли функция
*/
static bool SafeProcessEvent(UObject* Owner, UFunction* Function)
{
    if (!IsValid(Owner) || !Function)
    {
        UE_LOG(LogTemp, Error, TEXT("SafeProcessEvent: Invalid object or function!"));
        return false;
    }

    __try
    {
        bool bResult = false;
        // Вызов опасной функции с уже валидными Owner и Function
        Owner->ProcessEvent(Function, &bResult);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Highlighting   --- */

void UInteractiveComponent::CursorWasBeginOverOwner(AActor* TouchedActor)
{
    bool bChecker = true;
    AActor* lOwner = GetOwner();
    UFunction* Function = nullptr;

    for (FName& Name : PredicateFunctionNames)
    {
        Function = lOwner->FindFunction(Name);

        if (Function)
        {
            lOwner->ProcessEvent(Function, &bChecker);

            if (!bChecker)
                break;
        }
    }

    if (bChecker)
    {
        for (FComponentRendering& Data : UsedComponents)
        {
            if (Data.Component)
            {
                Data.Component->SetRenderCustomDepth(true);
            }
        }
    }
}

void UInteractiveComponent::CursorWasEndFromOwner(AActor* TouchedActor)
{
    for (FComponentRendering& Data : UsedComponents)
    {
        if (Data.Component)
        {
            Data.Component->SetRenderCustomDepth(false);
        }
    }
}

bool UInteractiveComponent::AddNamePredicate(const FName& NameFunction)
{
    UFunction* lFunction = GetOwner()->FindFunction(NameFunction);

    uint8 CheckError = CheckFunction(GetOwner(), lFunction);

    if (lFunction)
    {
        if (!CheckError)
        {
            PredicateFunctionNames.AddUnique(lFunction->GetFName());
            return true;
        }
#if WITH_EDITOR
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%s::%s: %s is Unsuitable Function:"),
                *GetNameSafe(this), *FString(__func__), *NameFunction.ToString());

            if (CheckError & 0b0001)
                UE_LOG(LogTemp, Warning, TEXT("* Input parameters more than 1"));
            if (CheckError & 0b0010)
                UE_LOG(LogTemp, Warning, TEXT("* Is NOT constant (not const or pure)"));
            if (CheckError & 0b0100)
                UE_LOG(LogTemp, Warning, TEXT("* Return value is NOT bool or Byte (uint8)"));
            if (CheckError & 0b1000)
                UE_LOG(LogTemp, Warning, TEXT("* SafeProcessEvent() failed"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s::%s: %s() function NOT found:"),
            *GetNameSafe(this), *FString(__func__), *NameFunction.ToString());
#endif // WITH_EDITOR
    }

    return false;
}

void UInteractiveComponent::InitUsedComponents()
{
    AActor* lOwner = GetOwner();

    if (UsedComponents.Num() > 1
        || (UsedComponents.Num() == 1
            && UsedComponents[0].Component)
        && lOwner)
    {
        for (FComponentRendering& Data : UsedComponents)
        {
            if (Data.Component)
            {
                Data.Component->SetCustomDepthStencilValue(Data.DepthStencilValue);
            }
        }

        lOwner->OnBeginCursorOver.AddDynamic(this, &UInteractiveComponent::CursorWasBeginOverOwner);
        lOwner->OnEndCursorOver.AddDynamic(this, &UInteractiveComponent::CursorWasEndFromOwner);
    }
}

uint8 UInteractiveComponent::CheckFunction(UObject* Owner, UFunction* Function)
{
    uint8 lBitsResult = 0b1000;

    // Проверяем, что функция нет входных параметров
    lBitsResult |= (Function->NumParms != 1) << 0;
    // Проверка, что функция константная (const или pure)
    lBitsResult |= (!Function->HasAnyFunctionFlags(FUNC_Const | FUNC_BlueprintPure)) << 1;
    // Проверка, что возвращаемое значение bool или Byte (uint8 и его вариации ENUM-список)
    lBitsResult |= (Function->GetPropertiesSize() != 1) << 2;

    if (lBitsResult == 0b1000)
    {
        // Проверка безопасности функции через попытку её вызова
        lBitsResult ^= (SafeProcessEvent(Owner, Function)) << 3;
    }

    return lBitsResult;
}
//--------------------------------------------------------------------------------------



/* ---   Actions   --- */

void UInteractiveComponent::InitActionGroup()
{
    UInputSettings* InputSettings = UInputSettings::GetInputSettings();
    TArray<FInputActionKeyMapping> lArray;

    InputSettings->GetActionMappingByName(SelectedActionGroups, lArray);
    ActionKeys.Empty();

    for (FInputActionKeyMapping& Data : lArray)
    {
        ActionKeys.Add(Data.Key);
    }

    if (lArray.Num())
    {
        GetOwner()->OnClicked.AddDynamic(this, &UInteractiveComponent::OwnerWasClicked);
    }
}

void UInteractiveComponent::OwnerWasClicked(AActor* TouchedActor, FKey ButtonReleased)
{
    if (OnOwnerWasClicked.IsBound()
        && ActionKeys.Find(ButtonReleased))
    {
        for (FComponentRendering& Data : UsedComponents)
        {
            if (Data.Component
                && Data.Component->bRenderCustomDepth)
            {
                OnOwnerWasClicked.Broadcast(ButtonReleased);
                break;
            }
        }
    }
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Base: Debugs   --- */

void UInteractiveComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Здесь можно написать логику проверки изменённого свойства.
    FName PropertyName = (PropertyChangedEvent.Property != nullptr)
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UInteractiveComponent, SelectedActionGroups))
    {
        ReinitActionGroup();
    }
};
//--------------------------------------------------------------------------------------



/* ---   Highlighting   --- */

TArray<FName> UInteractiveComponent::GetNamesOfHighlightedComponents()
{
    if (AActor* lOwner = GetOwner())
    {
        TArray<UActorComponent*> lPC;
        lOwner->GetComponents(UPrimitiveComponent::StaticClass(), lPC);

        if (lPC.Num())
        {
            TArray<FName> lResult;

            for (UActorComponent* lComp : lPC)
            {
                lResult.Add(lComp->GetFName());
            }

            return lResult;
        }
    }

    return TArray<FName>{NAME_None};
}

TArray<FName> UInteractiveComponent::GetNamesOfPredicateFunctions()
{
    if (AActor* lOwner = GetOwner())
    {
        UFunction* lFunction = nullptr;
        TArray<FName> lResult;

        for (TFieldIterator<UFunction> It(lOwner->GetClass(), EFieldIteratorFlags::IncludeSuper); It; ++It)
        {
            lFunction = *It;

            if (!CheckFunction(lOwner, lFunction))
            {
                lResult.Add(lFunction->GetFName());
            }
        }

        if (lResult.Num())
        {
            return lResult;
        }
    }

    return TArray<FName>{NAME_None};
}
//--------------------------------------------------------------------------------------



/* ---   Actions   --- */

TArray<FName> UInteractiveComponent::GetActionGroupsNames()
{
    TArray<FName> ActionNames;

    UInputSettings::GetInputSettings()->GetActionNames(ActionNames);

    return ActionNames;
}

void UInteractiveComponent::ReinitActionGroup()
{
    UInputSettings* InputSettings = UInputSettings::GetInputSettings();
    TArray<FInputActionKeyMapping> lArray;

    InputSettings->GetActionMappingByName(SelectedActionGroups, lArray);
    ActionKeys.Empty();

    for (FInputActionKeyMapping& Data : lArray)
    {
        ActionKeys.Add(Data.Key);
    }
}
//--------------------------------------------------------------------------------------

#endif // WITH_EDITOR
//======================================================================================
