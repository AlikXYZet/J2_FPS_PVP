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
        GetFPSGameState()->OnEndSortingPlayerStatistics.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnEndSortingOfPlayerStatistics);
        GetFPSGameState()->ReSortStatisticsData();
    }
    else
    {
        GetFPSGameState()->OnEndSortingPlayerStatistics.RemoveDynamic(this, &UPlayerStatisticsWidget::Event_OnEndSortingOfPlayerStatistics);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Players Statistics Data   --- */

void UPlayerStatisticsWidget::SetDataSortingTypeForSortedPlayerStatistics(EPlayerStatisticsSortingType InType)
{
    GetFPSGameState()->SetDataSortingTypeForSortedPlayerStatistics(InType);
}

const FPlayerStatisticsData& UPlayerStatisticsWidget::GetDataByIndexFromSortedPlayerStatistics(int32 Index) const
{
    return GetFPSGameState()->GetDataByIndexFromSortedPlayerStatistics(Index);
}

void UPlayerStatisticsWidget::InitStatisticsData()
{
    GetFPSGameState()->OnMatchStateChange.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnMatchStateChange);
    GetFPSGameState()->OnPreRemovingPlayerStatistics.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnRemovingPlayerStatisticsItems);
    GetFPSGameState()->OnPostAddingPlayerStatistics.AddDynamic(this, &UPlayerStatisticsWidget::Event_OnAddingPlayerStatisticsItems);
}
//--------------------------------------------------------------------------------------
