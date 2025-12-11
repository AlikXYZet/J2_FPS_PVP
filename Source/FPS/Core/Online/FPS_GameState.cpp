//

// Base:
#include "FPS_GameState.h"

// UE:
#include "GameFramework/GameSession.h"

// Net:
#include "Net/UnrealNetwork.h"

// Interaction:
#include "FPS_GameMode.h"
#include "FPS_PlayerController.h"
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
    BaseInit();

    Super::BeginPlay();

    InitStatisticsData();
}

void AFPS_GameState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //FPS_ColorMessage(FColor::Cyan, " %d", ElapsedTime);
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

    if (HasAuthority())
    {
        if (GetNetMode() == ENetMode::NM_ListenServer)
        {
            if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetFirstPlayerState()))
            {
                PCS->SetCurrentNetStatus(ENetMode::NM_ListenServer);
            }
            else
            {
                FPS_LOG(Error, "'APlayerState' Type is NOT 'APlayerCharacterState'");
            }
        }
    }
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
//    FPS_Message(" %d", lLocal);
//}

void AFPS_GameState::HandleMatchIsWaitingToStart()
{
    Super::HandleMatchIsWaitingToStart();

    ReInitSpectatorsData();
}

void AFPS_GameState::HandleMatchHasStarted()
{
    Super::HandleMatchHasStarted();
}
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
    // Локальный флаг: Добавлен хотя бы один "Наблюдатель"
    bool blIsAddedSpectator = false;

    OnPreRemovingPlayerStatistics.Broadcast(FinalSize + RemovedIndices.Num(), FinalSize);

    // Удаляем указатели на старые элементы массива и перенос Игроков в Наблюдатели
    // @note    Необходимо для корректного отображения данных перед их смещением в массиве 'PlayersStatistics'
    //          в момент вызова делегата 'OnEndSortingPlayerStatistics', что находится в "ReSortStatisticsData()"
    for (int32 i : RemovedIndices)
    {
        SortedPlayerStatistics.Remove(&PlayersStatistics[i]);
        blIsAddedSpectator |= AddSpectatorData(PlayersStatistics[i].PlayerData.PlayerState);
    }

    ReSortStatisticsData();

    // Перезаполняем массив указателями, что будут валидны после смещения данных в массиве 'PlayersStatistics'
    // Очистка и добавление по возрастающим значениям диапазона [0; FinalSize)
    SortedPlayerStatistics.Empty(FinalSize);
    for (int32 i = 0; i < FinalSize; ++i)
    {
        SortedPlayerStatistics.Add(&PlayersStatistics[i]);
    }

    if (blIsAddedSpectator)
    {
        Reaction_AddSpectatorData();
    }
}

void AFPS_GameState::OnPostAddingStatisticsDataItems(const TArray<int32>& AddedIndices, int32 FinalSize)
{
    /* ---   Players Statistics Data   --- */

    // Добавляем указатели на новые элементы массива 'PlayersStatistics'
    // Добавление по возрастающим значениям диапазона [OldSize; NewSize)
    for (int32 i = FinalSize - AddedIndices.Num(); i < FinalSize; ++i)
    {
        SortedPlayerStatistics.Add(&PlayersStatistics[i]);
    }

    OnPostAddingPlayerStatistics.Broadcast(FinalSize - AddedIndices.Num(), FinalSize);

    ReSortStatisticsData();
    //-------------------------------------------


    /* ---   Spectators Data   --- */

    // Локальный флаг: Удалён хотя бы один "Наблюдатель"
    bool blIsRemovedSpectator = false;

    for (int32 i : AddedIndices)
    {
        blIsRemovedSpectator |= RemoveSpectatorData(PlayersStatistics[i].PlayerData.PlayerState);
    }

    if (blIsRemovedSpectator)
    {
        Reaction_RemoveSpectatorData();
    }
    //-------------------------------------------
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

void AFPS_GameState::OnRep_PlayersStatistics()
{
    if (!bIsPlayersStatisticsSynchronized)
    {
        bIsPlayersStatisticsSynchronized = true;
        bool blIsRemovedSpectator = false;

        for (FPlayerStatisticsData& Data : PlayersStatistics)
        {
            if (Data.PlayerData.PlayerState)
            {
                if (Data.PlayerData.PlayerName.IsEmpty())
                {
                    Data.PlayerData.UpdateDataOnPlayerState();
                }

                blIsRemovedSpectator |= RemoveSpectatorData(Data.PlayerData.PlayerState);
            }
            else
            {
                bIsPlayersStatisticsSynchronized = false;
            }
        }

        if (blIsRemovedSpectator)
        {
            Reaction_RemoveSpectatorData();
        }

        if (bIsPlayersStatisticsSynchronized)
        {
            OnPostAddingPlayerStatistics.Broadcast(0, PlayersStatistics.Num());

            SortedPlayerStatistics.Empty(PlayersStatistics.Num());
            for (FPlayerStatisticsData& Data : PlayersStatistics)
            {
                SortedPlayerStatistics.Add(&Data);
            }

            ReSortStatisticsData();
        }
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
}
//--------------------------------------------------------------------------------------



/* ---   Role Selection   --- */

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
        int32 lIndex = PlayersStatistics.Find(PlayerState);

        if (lIndex != INDEX_NONE)
        {
            OnPreRemovingPlayerStatistics.Broadcast(PlayersStatistics.Num(), PlayersStatistics.Num() - 1);

            // @note: Удаляем на сервере и на клиентах без вызова делегатов изменения
            PlayersStatistics.RemoveAt(lIndex);

            SortedPlayerStatistics.Empty(PlayersStatistics.Num());
            for (FPlayerStatisticsData& Data : PlayersStatistics)
            {
                SortedPlayerStatistics.Add(&Data);
            }

            ReSortStatisticsData();
        }

        if (RemoveSpectatorData(PlayerState))
        {
            Reaction_RemoveSpectatorData();
        }
    }
}

void AFPS_GameState::SetPlayerReadiness(const APlayerState* PlayerState, bool bReadiness)
{
    if (!IsMatchInProgress())
    {
        int32 lIndex = PlayersStatistics.Find(PlayerState);

        if (lIndex != INDEX_NONE)
        {
            PlayersStatistics.SetReadiness(PlayersStatistics[lIndex], bReadiness);

            if (AFPS_PlayerController* PC = Cast<AFPS_PlayerController>(PlayerState->GetOwner()))
            {
                PC->Client_SetMatchReadiness(bReadiness);
            }
        }
    }
}
//--------------------------------------------------------------------------------------
