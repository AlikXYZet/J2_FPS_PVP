// 

// Base:
#include "PlayerStatisticsWidget.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"
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

    ReInitOnVisibilityChanges(InVisibility);
}
//--------------------------------------------------------------------------------------



/* ---   Players Statistics Data   --- */

void UPlayerStatisticsWidget::SetSortType(const EPlayerStatisticsSortingType InType)
{
    // Если новый тип сортировки равен старому, то сменить на противоположный тип
    // Например: новый и старый тип - это "A" по возрастанию => сменить на "A" по убыванию
    if (SortType == InType)
        SortType = EPlayerStatisticsSortingType(uint8(InType) + 0x80);
    else
        SortType = InType;

    SortingPredicate = GetSortingPredicate(SortType);

    SortPostStatisticsData();
}

const FPlayerStatisticsData UPlayerStatisticsWidget::GetIndexData(const int32& Index)
{
    if (SortedPlayerStatistics.IsValidIndex(Index))
    {
        return *SortedPlayerStatistics[Index];
    }

    return FPlayerStatisticsData();
}

void UPlayerStatisticsWidget::InitStatisticsData()
{
    if (AFPS_GameState::CurrentGameState)
    {
        GetPlayerStatisticsArray().OnPreRemovingItems.AddDynamic(this, &UPlayerStatisticsWidget::OnPreRemovingStatisticsDataItems);
        GetPlayerStatisticsArray().OnPostAddingItems.AddDynamic(this, &UPlayerStatisticsWidget::OnPostAddingStatisticsDataItems);
    }
    else
    {
        FPS_LOG(Error, TEXT("'%s' is NOT 'AFPS_GameState'"),
            GetWorld()->GetGameState()
            ? *GetWorld()->GetGameState()->GetFName().ToString()
            : *FString("None"));
    }
}

void UPlayerStatisticsWidget::RefreshStatisticsData(const int32& Size)
{
    SortedPlayerStatistics.Empty(Size);

    for (const FPlayerStatisticsData& Item : GetPlayerStatistics())
    {
        SortedPlayerStatistics.Add(&Item);
    }

    SortPostStatisticsData();
}

void UPlayerStatisticsWidget::SortPostStatisticsData()
{
    if (SortedPlayerStatistics.Num() > 1)
    {
        SortedPlayerStatistics.Sort(SortingPredicate);
    }

    Event_OnEndSorting();
}

void UPlayerStatisticsWidget::OnPreRemovingStatisticsDataItems(const TArray<int32>& RemovedIndices, const int32& FinalSize)
{
    Event_OnRemovingItems(RemovedIndices, FinalSize);
    SortedPlayerStatistics.Empty(FinalSize);

    auto lDeletedIndexesIterator = RemovedIndices.CreateConstIterator();

    for (int32 i = 0; i < GetPlayerStatistics().Num(); ++i)
    {
        if (!lDeletedIndexesIterator
            || i != *lDeletedIndexesIterator)
        {
            SortedPlayerStatistics.Add(&GetPlayerStatistics()[i]);
        }
        else
        {
            // Сортированы по порядку возрастания
            ++lDeletedIndexesIterator;
        }
    }

    SortPostStatisticsData();
}

void UPlayerStatisticsWidget::OnPostAddingStatisticsDataItems(const TArray<int32>& AddedIndices, const int32& FinalSize)
{
    Event_OnAddingItems(AddedIndices, FinalSize);
    RefreshStatisticsData(FinalSize);
}

TSortingPredicate UPlayerStatisticsWidget::GetSortingPredicate(const EPlayerStatisticsSortingType& InType)
{
    switch (InType)
    {

        /* ---   Up   --- */

    case EPlayerStatisticsSortingType::NameUp:
        return SORTING_PREDICATE(PlayerName, < );
        break;

    case EPlayerStatisticsSortingType::PingUp:
        return SORTING_PREDICATE_ByPtr(PlayerState, GetPing(), uint8(255), < );
        break;

    case EPlayerStatisticsSortingType::KillsUp:
        return SORTING_PREDICATE(Kills, < );
        break;

    case EPlayerStatisticsSortingType::AssistsUp:
        return SORTING_PREDICATE(Assists, < );
        break;

    case EPlayerStatisticsSortingType::DeathsUp:
        return SORTING_PREDICATE(Deaths, < );
        break;

    case EPlayerStatisticsSortingType::PointsUp:
        return SORTING_PREDICATE(Points, < );
        break;


        /* ---   Down   --- */

    case EPlayerStatisticsSortingType::NameDown:
        return SORTING_PREDICATE(PlayerName, > );
        break;

    case EPlayerStatisticsSortingType::PingDown:
        return SORTING_PREDICATE_ByPtr(PlayerState, GetPing(), uint8(255), > );
        break;

    case EPlayerStatisticsSortingType::KillsDown:
        return SORTING_PREDICATE(Kills, > );
        break;

    case EPlayerStatisticsSortingType::AssistsDown:
        return SORTING_PREDICATE(Assists, > );
        break;

    case EPlayerStatisticsSortingType::DeathsDown:
        return SORTING_PREDICATE(Deaths, > );
        break;

    case EPlayerStatisticsSortingType::PointsDown:
        return SORTING_PREDICATE(Points, > );
        break;


        /* ---   Default   --- */

    default:
        return SORTING_PREDICATE_ByPtr(PlayerState, GetPlayerName(), FString("NONE"), < );
        break;
    }
}
//--------------------------------------------------------------------------------------
