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
    // Установка вызова функции Tick() в каждом кадре
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    SetActorTickInterval(1.f); // 1 раз/сек.

    // Настройка репликации
    bReplicates = true;
    SetReplicateMovement(false);
    //-------------------------------------------


    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'AFPS_GameState'
    CurrentGameState = this;
    //-------------------------------------------


    /* ---   Match Management   --- */

    // Выставить "блокируещее" значение по умолчанию
    ElapsedTime = MIN_int32;
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

//void AFPS_GameState::Tick(float DeltaSeconds)
//{
//    Super::Tick(DeltaSeconds);
//}

void AFPS_GameState::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    /* ---   Statics   --- */

    CurrentGameState = this;
    //-------------------------------------------
}

void AFPS_GameState::PostInitializeComponents()
{
    // Обход действия кода в 'AGameState::PostInitializeComponents()'
    Super::Super::PostInitializeComponents();
    // @note    Переносим инициализацию 'TimerHandle_DefaultTimer' в 'OnRep_ElapsedTime()'
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
            if (APlayerCharacterState* PCS = GetFirstPlayerState<APlayerCharacterState>())
            {
                PCS->SetCurrentNetStatus(ENetMode::NM_ListenServer);
            }
            else
            {
                FPS_Error("'APlayerState' Type is NOT 'APlayerCharacterState'");
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



/* ---   Match Management : Match State   --- */

void AFPS_GameState::OnRep_MatchState()
{
    //CurrentMatchState = EMatchState::Custom;

    //Super::OnRep_MatchState(); // Вместо этого Копируем, оптимизируем и расширяем код

    if (MatchState == MatchState::WaitingToStart || PreviousMatchState == MatchState::EnteringMap)
    {
        CurrentMatchState = EMatchState::WaitingToStart;
        // Вызываем 'MatchIsWaiting' для запуска, даже если идёт присоединение к процессу в более позднем состоянии
        HandleMatchIsWaitingToStart();
    }

    // Warning: Без данного разрыва "if-else",
    // возникает баг с запуском Матча
    // при подключении к сессии с уже запущенном Матче
    if (MatchState == MatchState::InProgress)
    {
        CurrentMatchState = EMatchState::PreProgress;
        HandleMatchHasStarted();
    }
    else if (MatchState == MatchState::WaitingPostMatch)
    {
        CurrentMatchState = EMatchState::WaitingPostMatch;
        HandleMatchHasEnded();
    }
    else if (MatchState == MatchState::LeavingMap)
    {
        CurrentMatchState = EMatchState::LeavingMap;
        HandleLeavingMap();
    }

    PreviousMatchState = MatchState;

    OnMatchStateChange.Broadcast(CurrentMatchState);
}

void AFPS_GameState::HandleMatchIsWaitingToStart()
{
    Super::HandleMatchIsWaitingToStart();

    ReInitSpectatorsData();
}

void AFPS_GameState::HandleMatchHasStarted()
{
    Super::HandleMatchHasStarted();

    SetElapsedTime(MatchStartDelayTime);
}

void AFPS_GameState::HandleMatchHasEnded()
{
    Super::HandleMatchHasEnded();

    SetElapsedTime(MatchEndingDelayTime);
}

void AFPS_GameState::MatchStateControl()
{
    switch (GetCurrentMatchState())
    {

    case EMatchState::WaitingToStart:
        if (HasAuthority())
        {
            GetFPSGameMode()->bDelayedStart = false;
            GetFPSGameMode()->StartPlay();
        }
        break;

    case EMatchState::PreProgress:
        // Local:
        {
            SetCurrentMatchState(EMatchState::InProgress);
            SetElapsedTime(MatchDurationTime);
        }
        break;

    case EMatchState::InProgress:
        if (HasAuthority())
        {
            GetFPSGameMode()->bDelayedStart = true;
            GetFPSGameMode()->EndMatch();
        }
        break;

    case EMatchState::WaitingPostMatch:
        if (HasAuthority())
        {
            GetFPSGameMode()->RestartGame();
        }
        break;

        //case EMatchState::LeavingMap:
        //    break;

        //case EMatchState::Aborted:
        //    break;

        //case EMatchState::Custom:
        //    break;

    default:
        break;
    }
}

void AFPS_GameState::CheckPlayersReadiness()
{
    /** @note
    Требуется обновлять на стороне Сервера при каждом изменении готовности среди Игроков:
    * Изменилась готовность;
    * Кто-то Добавился или Убрался из списка Игроков;
    * Кто-то вышел из игры, когда был в списке Игроков */
    int32 lCount = 0;

    for (const FPlayerStatisticsData& Data : PlayersStatistics)
    {
        if (Data.bPlayerReadiness)
        {
            ++lCount;
        }
    }

    if (IsMatchInWaitingToStart())
    {
        if (lCount)
        {
            if (lCount == PlayersStatistics.Num())
            {
                // Если все готовы, то Запустить Сокращённое время ожидания
                Multicast_SetElapsedTime(ShortWaitTimeForMatchToStart);
            }
            else
            {
                // Если готовы не все, но хотя бы один, то Запустить Длинное время ожидания
                Multicast_SetElapsedTime(LongWaitTimeForMatchToStart);
            }
        }
        else
        {
            // Если никто не готов, то Остановить время ожидания
            Multicast_SetElapsedTime(MIN_int32);
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Match Management : Elapsed Time   --- */

void AFPS_GameState::OnRep_ElapsedTime()
{
    // Проверка на значение, знаменующее останов таймера
    if (ElapsedTime == MIN_int32)
    {
        StopDefaultTimer();
    }
    else
    {
        StartDefaultTimer();
    }
}

void AFPS_GameState::DefaultTimer()
{
    // По умолчанию используется для отсчёта времени ("++ElapsedTime") во время матча
    //Super::DefaultTimer(); // Вместо этого Копируем, оптимизируем и расширяем код

    --ElapsedTime;

    // Все изменения состояния Матча будут происходить при 'ElapsedTime == 0'
    if (ElapsedTime == 0)
    {
        MatchStateControl();
    }

    OnElapsedTimeChange.Broadcast(ElapsedTime);

    StartDefaultTimer();
}

void AFPS_GameState::Multicast_SetElapsedTime_Implementation(int32 Time)
{
    SetElapsedTime(Time);
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

void AFPS_GameState::RemovePlayerStatisticsData(APlayerState* PlayerState)
{
    if (HasAuthority())
    {
        if (PlayersStatistics.RemovePlayer(PlayerState))
        {
            if (AFPS_PlayerController* PC = Cast<AFPS_PlayerController>(PlayerState->GetOwner()))
            {
                // Учёт количества "Наблюдателей"
                GetFPSGameMode()->PlayerSwitchedToSpectatorOnly(PC);

                /** Смена роли на "Наблюдатель"
                @note   Заменяем `PC->StartSpectatingOnly();`, так как в данный момент `StateName == NAME_Spectating` */
                PlayerState->SetIsSpectator(true);
                PlayerState->SetIsOnlyASpectator(true);
                PC->bPlayerIsWaiting = false;

                PC->Client_ChangedSelectedRole(false);
            }

            CheckPlayersReadiness();
        }
    }
}

void AFPS_GameState::AddPlayerStatisticsData(APlayerState* PlayerState)
{
    if (HasAuthority())
    {
        if (PlayersStatistics.AddPlayer(PlayerState))
        {
            if (AFPS_PlayerController* PC = Cast<AFPS_PlayerController>(PlayerState->GetOwner()))
            {
                // Учёт количества "Игроков"
                GetFPSGameMode()->PlayerSwitchedToPlayer(PC);

                // Смена роли на "Игрок"
                PlayerState->SetIsSpectator(false);
                PlayerState->SetIsOnlyASpectator(false);
                PC->bPlayerIsWaiting = true;

                PC->Client_ChangedSelectedRole(true);
            }

            CheckPlayersReadiness();
        }
    }
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
    /** Проверка синхронизации массива 'PlayersStatistics'
    @note   В момент подключения, 'PlayersStatistics' имеет "нулевые" указатели на 'PlayerState',
    так как экземпляры 'APlayerState' ещё не синхронизированы, а данные массива не действительны */
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



/* ---   Player Data   --- */

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

            // @note: Удаляем и на Сервере и на Клиентах без лишнего вызова
            // Делегата 'OnPreRemovingItems' массива 'PlayersStatistics'
            PlayersStatistics.RemoveAt(lIndex);

            SortedPlayerStatistics.Empty(PlayersStatistics.Num());
            for (FPlayerStatisticsData& Data : PlayersStatistics)
            {
                SortedPlayerStatistics.Add(&Data);
            }

            ReSortStatisticsData();

            if (HasAuthority())
            {
                CheckPlayersReadiness();
            }
        }

        if (RemoveSpectatorData(PlayerState))
        {
            Reaction_RemoveSpectatorData();
        }
    }
}

void AFPS_GameState::SetPlayerReadiness(const APlayerState* PlayerState, bool bReadiness)
{
    if (!IsMatchInProgress() && HasAuthority())
    {
        int32 lIndex = PlayersStatistics.Find(PlayerState);

        if (lIndex != INDEX_NONE)
        {
            PlayersStatistics.SetReadiness(PlayersStatistics[lIndex], bReadiness);

            if (AFPS_PlayerController* PC = Cast<AFPS_PlayerController>(PlayerState->GetOwner()))
            {
                PC->Client_ChangedMatchReadiness(bReadiness);
            }

            CheckPlayersReadiness();
        }
    }
}
//--------------------------------------------------------------------------------------
