// 

// Base:
#include "RoleSelectionWidget.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"
#include "FPS/Core/Online/FPS_PlayerController.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void URoleSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitWithGameState();
    InitWithPlayerController();
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

void URoleSelectionWidget::SetDataSortingTypeForSortedSpectators(EPlayerStatisticsSortingType InType) const
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

void URoleSelectionWidget::GoToSpectators() const
{
    ((AFPS_PlayerController*)GetOwningPlayer())->GoToSpectators();
}

void URoleSelectionWidget::GoToPlayers() const
{
    ((AFPS_PlayerController*)GetOwningPlayer())->GoToPlayers();
}

void URoleSelectionWidget::SetOwnerReadiness(bool bReadiness) const
{
    ((AFPS_PlayerController*)GetOwningPlayer())->SetMatchReadiness(bReadiness);
}

void URoleSelectionWidget::InitWithGameState()
{
    if (AFPS_GameState::IsValidStaticPointer())
    {
        GetFPSGameState()->OnEndSortingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnEndSortingOfSpectators);
        GetFPSGameState()->OnRemovingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnRemovingSpectatorsItems);
        GetFPSGameState()->OnAddingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnAddingSpectatorsItems);
    }
    else
    {
        FPS_Error("Current 'Game State' is NOT 'AFPS_GameState'");
    }
}

void URoleSelectionWidget::InitWithPlayerController()
{
    if (AFPS_PlayerController* PC = GetOwningPlayer<AFPS_PlayerController>())
    {
        PC->OnSelectedRoleChange.AddDynamic(this, &URoleSelectionWidget::Event_OnSelectedRoleChange);
        PC->OnPlayerReadinessChange.AddDynamic(this, &URoleSelectionWidget::Event_OnOwnerReadinessChange);
    }
    else
    {
        FPS_Error("Current 'Owning Player' is NOT 'AFPS_PlayerController'");
    }
}
//--------------------------------------------------------------------------------------
