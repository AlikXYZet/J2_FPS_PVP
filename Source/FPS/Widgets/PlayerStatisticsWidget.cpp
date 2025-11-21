// 

// Base:
#include "PlayerStatisticsWidget.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"
#include "FPS/Core/Online/FPS_GameMode.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UPlayerStatisticsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitStatisticsData();
}

void UPlayerStatisticsWidget::SetVisibility(ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);

    /** ПереИнициализация данных Статистики при изменении Видимости
    "Is NOT Visible?"
    @note   Метод 'IsVisible()' более тяжёлый */
    if (InVisibility != ESlateVisibility::Collapsed
        && InVisibility != ESlateVisibility::Hidden)
    {
        AFPS_GameState::CurrentGameState->OnEndSortingPlayerStatistics.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnEndSortingOfPlayerStatistics);
        AFPS_GameState::CurrentGameState->ReSortStatisticsData();
    }
    else
    {
        AFPS_GameState::CurrentGameState->OnEndSortingPlayerStatistics.RemoveDynamic(this, &UPlayerStatisticsWidget::Event_OnEndSortingOfPlayerStatistics);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Players Statistics Data   --- */

void UPlayerStatisticsWidget::SetDataSortingTypeForSortedPlayerStatistics(EPlayerStatisticsSortingType InType)
{
    AFPS_GameState::CurrentGameState->SetDataSortingTypeForSortedPlayerStatistics(InType);
}

const FPlayerStatisticsData& UPlayerStatisticsWidget::GetDataByIndexFromSortedPlayerStatistics(int32 Index) const
{
    return AFPS_GameState::CurrentGameState->GetDataByIndexFromSortedPlayerStatistics(Index);
}

void UPlayerStatisticsWidget::InitStatisticsData()
{
    if (AFPS_GameState::CurrentGameState)
    {
        AFPS_GameState::CurrentGameState->OnMatchStateChange.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnMatchStateChange);
        AFPS_GameState::CurrentGameState->PlayersStatistics.OnPreRemovingItems.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnRemovingPlayerStatisticsItems);
        AFPS_GameState::CurrentGameState->PlayersStatistics.OnPostAddingItems.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnAddingPlayerStatisticsItems);
    }
    else
    {
        FPS_LOG(Error, TEXT("'%s' is NOT 'AFPS_GameState'"),
            GetWorld()->GetGameState()
            ? *GetWorld()->GetGameState()->GetFName().ToString()
            : *FString("None"));
    }
}
//--------------------------------------------------------------------------------------
