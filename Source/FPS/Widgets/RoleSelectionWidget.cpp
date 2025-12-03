// 

// Base:
#include "RoleSelectionWidget.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void URoleSelectionWidget::NativeConstruct()
{
    if (!AFPS_GameState::IsValidStaticPointer())
    {
        FPS_LOG(Error, TEXT("Current 'Game State' is NOT 'AFPS_GameState'"));
    }

    Super::NativeConstruct();

    InitRoleSelection();
}

void URoleSelectionWidget::SetVisibility(ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);

    /** ПереИнициализация данных Статистики при изменении Видимости
    "Is NOT Visible?"
    @note   Метод 'IsVisible()' более тяжёлый */
    if (InVisibility != ESlateVisibility::Collapsed
        && InVisibility != ESlateVisibility::Hidden)
    {
        //GetFPSGameState()->OnEndSortingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnEndSortingOfSpectators);
        //GetFPSGameState()->OnRemovingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnRemovingSpectatorsItems);
        //GetFPSGameState()->OnAddingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnAddingSpectatorsItems);
        Event_OnAddingSpectatorsItems(GetFPSGameState()->GetSortedSpectators().Num());
        GetFPSGameState()->ReSortSpectatorsData();
    }
    else
    {
        //GetFPSGameState()->OnEndSortingSpectators.RemoveAll(this);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Spectators Data   --- */

void URoleSelectionWidget::SetDataSortingTypeForSortedSpectators(EPlayerStatisticsSortingType InType)
{
    GetFPSGameState()->SetDataSortingTypeForSortedSpectators(InType);
}

const FPlayerData& URoleSelectionWidget::GetDataByIndexFromSortedSpectators(int32 Index) const
{
    return GetFPSGameState()->GetDataByIndexFromSortedSpectators(Index);
}

const TArray<FPlayerData>& URoleSelectionWidget::GetSortedSpectators() const
{
    return GetFPSGameState()->GetSortedSpectators();
}
//--------------------------------------------------------------------------------------



/* ---   Role Selection   --- */

void URoleSelectionWidget::SetOwnerReadiness(bool bReadiness)
{
    GetFPSGameState()->SetClientReadiness(bReadiness);
}

void URoleSelectionWidget::InitRoleSelection()
{
    GetFPSGameState()->OnClientReadinessChange.AddDynamic(this, &URoleSelectionWidget::Event_OnOwnerReadinessChange);
    GetFPSGameState()->OnEndSortingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnEndSortingOfSpectators);
    GetFPSGameState()->OnRemovingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnRemovingSpectatorsItems);
    GetFPSGameState()->OnAddingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnAddingSpectatorsItems);
}
//--------------------------------------------------------------------------------------
