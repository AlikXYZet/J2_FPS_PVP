//

// Base:
#include "FPS_GameState.h"

// UE:
#include "GameFramework/GameSession.h"

// Net:
#include "Net/UnrealNetwork.h"

// Interaction:
#include "FPS_GameMode.h"
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Общедоступный указатель на текущий экземпляр класса 'AFPS_GameState'
AFPS_GameState* AFPS_GameState::CurrentGameState = nullptr;
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFPS_GameState::AFPS_GameState()
{
    PrimaryActorTick.bCanEverTick = true; // Принудительно
    SetActorTickInterval(1.f); // 1 раз/сек.
    /* @note    Если требуется использовать 'Tick()' для чего-то ещё, то следует заменить данный способ на Циклический Таймер.
    Например:
    // Таймер: Контроль состояния Раунда
    // @note    Таймер с тактом раз в секунду для отслеживания времени
    GetWorld()->GetTimerManager().SetTimer(
        Timer_RoundStatusControl,
        this,
        &AFPS_GameState::RemainingRoundTimeCounter,
        1.f,
        true);
    */

    // Настройка репликации
    bReplicates = true;
    SetReplicateMovement(false);
    //-------------------------------------------


    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'AFPS_GameState'
    CurrentGameState = this;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AFPS_GameState::BeginPlay()
{
    Super::BeginPlay();
    FPS_LOGMessage("");

    BaseInit();
    InitStatisticsData();
}

void AFPS_GameState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //FPS_ColorLOGMessage(FColor::Cyan, " %d", ElapsedTime);
}

void AFPS_GameState::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    /* ---   Statics   --- */

    CurrentGameState = this;
    //-------------------------------------------
}

void AFPS_GameState::Destroyed()
{
    /* ---   Statics   --- */

    CurrentGameState = nullptr;
    //-------------------------------------------

    Super::Destroyed();
}

void AFPS_GameState::BaseInit()
{
    IsValidStaticPointer();
}
//--------------------------------------------------------------------------------------



/* ---   Net   --- */

void AFPS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    //-------------------------------------------


    /* ---   Statistics   --- */

    DOREPLIFETIME(AFPS_GameState, PlayersStatistics);
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Match Management   --- */

void AFPS_GameState::OnRep_MatchState()
{
    //CurrentMatchState = EMatchState::Custom;

    //Super::OnRep_MatchState(); // Вместо этого Копируем, оптимизируем и расширяем код

    if (MatchState == MatchState::WaitingToStart || PreviousMatchState == MatchState::EnteringMap)
    {
        // Call MatchIsWaiting to start even if you join in progress at a later state
        HandleMatchIsWaitingToStart();
        CurrentMatchState = EMatchState::WaitingToStart;
    }

    // Warning: Без данного разрыва "if-else",
    // возникает баг с запуском Матча
    // при подключении к сессии с уже запущенном Матче
    if (MatchState == MatchState::InProgress)
    {
        HandleMatchHasStarted();
        CurrentMatchState = EMatchState::InProgress;
    }
    else if (MatchState == MatchState::WaitingPostMatch)
    {
        HandleMatchHasEnded();
        CurrentMatchState = EMatchState::WaitingPostMatch;
    }
    else if (MatchState == MatchState::LeavingMap)
    {
        HandleLeavingMap();
        CurrentMatchState = EMatchState::LeavingMap;
    }

    OnMatchStateChange.Broadcast(CurrentMatchState);

    PreviousMatchState = MatchState;
}

//void AFPS_GameState::OnRep_ElapsedTime()
//{
//    int32 lLocal = ElapsedTime;
//    FPS_LOGMessage(" %d", lLocal);
//}

void AFPS_GameState::HandleMatchIsWaitingToStart()
{
    Super::HandleMatchIsWaitingToStart();

    ReInitSpectatorsData();
}

//void AFPS_GameState::HandleMatchHasStarted()
//{
//    Super::HandleMatchHasStarted();
//}
//--------------------------------------------------------------------------------------



/* ---   Players Statistics Data   --- */

void AFPS_GameState::ReSortStatisticsData()
{
    if (OnEndSortingPlayerStatistics.IsBound())
    {
        if (SortedPlayerStatistics.Num() > 1)
        {
            SortedPlayerStatistics.Sort(SortingPlayerStatisticsPredicate);
        }

        OnEndSortingPlayerStatistics.Broadcast();
    }
}

void AFPS_GameState::InitStatisticsData()
{
    PlayersStatistics.OnPreRemovingItems.AddDynamic(this, &AFPS_GameState::OnPreRemovingStatisticsDataItems);
    PlayersStatistics.OnPostAddingItems.AddDynamic(this, &AFPS_GameState::OnPostAddingStatisticsDataItems);
    PlayersStatistics.OnPostChangingArrayData.AddDynamic(this, &AFPS_GameState::ReSortStatisticsData);
}

void AFPS_GameState::OnPreRemovingStatisticsDataItems(const TArray<int32>& RemovedIndices, int32 FinalSize)
{
    auto lDeletedIndexesIterator = RemovedIndices.CreateConstIterator();
    bool lIsAddedSpectator = false;

    for (int32 i = 0; i < PlayersStatistics.Items.Num(); ++i)
    {
        if (lDeletedIndexesIterator
            && i == *lDeletedIndexesIterator)
        {
            SortedPlayerStatistics.Remove(&PlayersStatistics.Items[i]);

            lIsAddedSpectator |= AddSpectatorData(PlayersStatistics.Items[i].PlayerData.PlayerState);

            // Нумерация в 'RemovedIndices' идёт по возрастанию
            ++lDeletedIndexesIterator;
        }
    }

    ReSortStatisticsData();

    if (lIsAddedSpectator)
    {
        Reaction_AddSpectatorData();
    }
}

void AFPS_GameState::OnPostAddingStatisticsDataItems(const TArray<int32>& AddedIndices, int32 FinalSize)
{
    auto lDeletedIndexesIterator = AddedIndices.CreateConstIterator();
    bool lIsRemovedSpectator = false;

    for (int32 i = 0; i < PlayersStatistics.Items.Num(); ++i)
    {
        if (lDeletedIndexesIterator
            && i == *lDeletedIndexesIterator)
        {
            SortedPlayerStatistics.Add(&PlayersStatistics.Items[i]);

            lIsRemovedSpectator |= RemoveSpectatorData(PlayersStatistics.Items[i].PlayerData.PlayerState);

            // Нумерация в 'AddedIndices' идёт по возрастанию
            ++lDeletedIndexesIterator;
        }
    }

    ReSortStatisticsData();

    if (lIsRemovedSpectator)
    {
        Reaction_RemoveSpectatorData();
    }
}

TSortingPredicate AFPS_GameState::GetSortingPredicateForPlayerStatistics(EPlayerStatisticsSortingType InType) const
{
    switch (InType)
    {

        /* ---   Up   --- */

    case EPlayerStatisticsSortingType::NameUp:
        return SORTING_PREDICATE(PlayerData.PlayerName, < );
        break;

    case EPlayerStatisticsSortingType::PingUp:
        return SORTING_PREDICATE_ByPtr(PlayerData.PlayerState, GetPing(), uint8(255), < );
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
        return SORTING_PREDICATE(PlayerData.PlayerName, > );
        break;

    case EPlayerStatisticsSortingType::PingDown:
        return SORTING_PREDICATE_ByPtr(PlayerData.PlayerState, GetPing(), uint8(255), > );
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
        return SORTING_PREDICATE(PlayerData.PlayerName, < );
        break;
    }
}
//--------------------------------------------------------------------------------------



/* ---   Spectators Data   --- */

void AFPS_GameState::ReInitSpectatorsData()
{
    int32 lReserved = (HasAuthority() && GetFPSGameMode()->GameSession)
        ? GetFPSGameMode()->GameSession->MaxPlayers
        : 16;

    SortedSpectators.Empty(16);
    SortedSpectators.Append(PlayerArray);
    Reaction_AddSpectatorData();
}

TSortingSpectatorsPredicate AFPS_GameState::GetSortingPredicateForSpectators(EPlayerStatisticsSortingType InType) const
{
    FPS_LOG(Warning, "Start: %d", (uint32)InType);

    switch (InType)
    {

        /* ---   Up   --- */

    case EPlayerStatisticsSortingType::NameUp:
        return SORTING_PREDICATE(PlayerName, < );
        break;

    case EPlayerStatisticsSortingType::PingUp:
        return SORTING_PREDICATE_ByPtr(PlayerState, GetPing(), uint8(255), < );
        break;


        /* ---   Down   --- */

    case EPlayerStatisticsSortingType::NameDown:
        return SORTING_PREDICATE(PlayerName, > );
        break;

    case EPlayerStatisticsSortingType::PingDown:
        return SORTING_PREDICATE_ByPtr(PlayerState, GetPing(), uint8(255), > );
        break;


        /* ---   Default   --- */

    default:
        return SORTING_PREDICATE(PlayerName, < );
        break;
    }

    FPS_LOG(Warning, "End: %d", (uint32)InType);
}
//--------------------------------------------------------------------------------------



/* ---   Role Selection   --- */

void AFPS_GameState::Server_SetClientReadiness_Implementation(const APlayerController* Client, bool bReadiness)
{
    if (FPlayerStatisticsData** lData = GetFPSGameMode()->PlayersStatisticsMap.Find(Client))
    {
        (*lData)->bPlayerReadiness = bReadiness;
        Client_SetClientReadiness(bReadiness);
        // PS: Обновляем без проверки на неравенство, так как есть шанс утраты данных
    }
}

void AFPS_GameState::Client_SetClientReadiness_Implementation(bool bReadiness)
{
    OnClientReadinessChange.Broadcast(bReadiness);
}

void AFPS_GameState::AddPlayerState(APlayerState* PlayerState)
{
    Super::AddPlayerState(PlayerState);

    if (!IsMatchInProgress())
    {
        if (AddSpectatorData(PlayerState))
        {
            Reaction_AddSpectatorData();
        }
    }
}

void AFPS_GameState::RemovePlayerState(APlayerState* PlayerState)
{
    Super::RemovePlayerState(PlayerState);

    if (!IsMatchInProgress())
    {
        if (RemoveSpectatorData(PlayerState))
        {
            Reaction_RemoveSpectatorData();
        }
    }
}
//--------------------------------------------------------------------------------------
