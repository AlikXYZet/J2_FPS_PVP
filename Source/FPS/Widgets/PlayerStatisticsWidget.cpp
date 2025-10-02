// 

// Base:
#include "PlayerStatisticsWidget.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void UPlayerStatisticsWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    InitStatisticsData();
}

void UPlayerStatisticsWidget::NativeConstruct()
{
    Super::NativeConstruct();
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

    SortStatisticsData();
}

const FPlayerStatisticsData UPlayerStatisticsWidget::GetIndexData(const int32& Index)
{
    if (0 <= Index && Index < SortedPlayerStatistics.Num())
    {
        return *SortedPlayerStatistics[Index];
    }
    return FPlayerStatisticsData();
}

void UPlayerStatisticsWidget::InitStatisticsData()
{
    if (AFPS_GameState::CurrentGameState)
    {
        FPlayerStatisticsArray* lPSArray = &AFPS_GameState::CurrentGameState->PlayersStatistics;
        lPSArray->OnChangingNumbers.AddDynamic(this, &UPlayerStatisticsWidget::RefreshStatisticsData);
        lPSArray->OnChangingArrayData.AddDynamic(this, &UPlayerStatisticsWidget::SortStatisticsData);

        CurrentPlayerStatistics = &lPSArray->Items;

        RefreshStatisticsData(CurrentPlayerStatistics->Num());
    }
    else
    {
        FPS_LOG(Error, TEXT("'%s' is NOT 'AFPS_GameState'"),
            GetWorld()->GetGameState()
            ? *GetWorld()->GetGameState()->GetFName().ToString()
            : *FString("None"));
    }
}

void UPlayerStatisticsWidget::RefreshStatisticsData(int32 Size)
{
    if (Size != SortedPlayerStatistics.Num())
    {
        int32 lPreviousNum = SortedPlayerStatistics.Num();

        SortedPlayerStatistics.Empty(Size);
        for (const FPlayerStatisticsData& Item : *CurrentPlayerStatistics)
        {
            SortedPlayerStatistics.Add(&Item);
        }

        Event_OnRefreshing(Size);

        SortStatisticsData();
    }
}

void UPlayerStatisticsWidget::SortStatisticsData()
{
    if (SortedPlayerStatistics.Num() > 1)
    {
        SortedPlayerStatistics.Sort(SortingPredicate);

        Event_OnEndSorting();
    }
}

TSortingPredicate UPlayerStatisticsWidget::GetSortingPredicate(const EPlayerStatisticsSortingType& InType)
{
    switch (InType)
    {

        /* ---   Up   --- */

    case EPlayerStatisticsSortingType::NameUp:
        return SORTING_PREDICATE_ByPtr(PlayerState, GetPlayerName(), FString("NONE"), < );
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
        return SORTING_PREDICATE_ByPtr(PlayerState, GetPlayerName(), FString("NONE"), > );
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
