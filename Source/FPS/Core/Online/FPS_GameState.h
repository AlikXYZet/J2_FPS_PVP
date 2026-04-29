//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/GameState.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// Structs:
#include "FPS/Tools/Structs/GameData/MatchStateData.h"
#include "FPS/Tools/Structs/GameData/PlayerStatisticsData.h"

// Interaction:
#include "FPS/Characters/PlayerCharacterState.h"

// Generated:
#include "FPS_GameState.generated.h"
//--------------------------------------------------------------------------------------



/* ---   typedef   --- */

// Шаблонный Тип предиката сортировки
typedef bool (*TSortingPredicate)(const FPlayerStatisticsData& first, const FPlayerStatisticsData& second);

// Шаблонный Тип предиката сортировки Наблюдателей
typedef bool (*TSortingSpectatorsPredicate)(const FPlayerData& first, const FPlayerData& second);
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При изменении состояния Матча
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChange, EMatchState, NewMatchState);

// Делегат: Перед Удалением элементов Массива данных Статистики Игроков
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPreRemovingPlayerStatistics, int32, OldSize, int32, NewSize);

// Делегат: Добавлены элементы Массива данных Статистики Игроков
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostAddingPlayerStatistics, int32, OldSize, int32, NewSize);

// Делегат: При Завершении сортировки Массива данных Статистики Игроков
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSortingPlayerStatistics);

// Делегат: Перед Удалением элементов Массива данных Наблюдателей
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreRemovingSpectators, int32, FinalSize);

// Делегат: Добавлены элементы Массива данных Наблюдателей
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPostAddingSpectators, int32, FinalSize);

// Делегат: При Завершении сортировки Массива данных Наблюдателей
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSortingSpectators);

// Делегат: При изменении Прошедшего Времени ('ElapsedTime')
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnElapsedTimeChange, int32, ElapsedTime);
//--------------------------------------------------------------------------------------



/* ---   ENums   --- */

// Типы сортировки Статистики Игроков
UENUM(BlueprintType)
enum struct EPlayerStatisticsSortingType : uint8
{
    NONE,

    // Возрастающий список:

    NameUp      UMETA(DisplayName = "Name Up"),
    PingUp      UMETA(DisplayName = "Ping Up"),
    KillsUp     UMETA(DisplayName = "Kills Up"),
    AssistsUp   UMETA(DisplayName = "Assists Up"),
    DeathsUp    UMETA(DisplayName = "Deaths Up"),
    PointsUp    UMETA(DisplayName = "Points Up"),

    // Спадающий список:

    NameDown = NameUp + MAX_int8    UMETA(DisplayName = "Name Down"),
    PingDown    UMETA(DisplayName = "Ping Down"),
    KillsDown   UMETA(DisplayName = "Kills Down"),
    AssistsDown UMETA(DisplayName = "Assists Down"),
    DeathsDown  UMETA(DisplayName = "Deaths Down"),
    PointsDown  UMETA(DisplayName = "Points Down"),
};
//----------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Static Functions:
static AFPS_GameState* const GetFPSGameState();

// Interaction:
class AFPS_PlayerController;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AFPS_GameState : public AGameState
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    // Делегат: При изменении состояния Матча
    UPROPERTY(BlueprintAssignable)
    FOnMatchStateChange OnMatchStateChange;

    // Делегат: Перед Удалением элементов Массива данных Статистики Игроков
    FOnPreRemovingPlayerStatistics OnPreRemovingPlayerStatistics;

    // Делегат: Добавлены элементы Массива данных Статистики Игроков
    FOnPostAddingPlayerStatistics OnPostAddingPlayerStatistics;

    // Делегат: При Завершении сортировки Массива данных Статистики Игроков
    FOnEndSortingPlayerStatistics OnEndSortingPlayerStatistics;

    // Делегат: Перед Удалением элементов Массива данных Наблюдателей
    FOnPreRemovingSpectators OnRemovingSpectators;

    // Делегат: Добавлены элементы Массива данных Наблюдателей
    FOnPostAddingSpectators OnAddingSpectators;

    // Делегат: При Завершении сортировки Массива данных Наблюдателей
    FOnEndSortingSpectators OnEndSortingSpectators;

    // Делегат: При изменении Прошедшего Времени ('ElapsedTime')
    UPROPERTY(BlueprintAssignable)
    FOnElapsedTimeChange OnElapsedTimeChange;
    //-------------------------------------------



    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'AFPS_GameState'
    // @note    Используется для уменьшения зависимостей и использования излишних функций
    //          Например, функций 'Cast<>' и методов Инициализации в других классах
    static AFPS_GameState* CurrentGameState;

    //

    /** Метод проверки валидности статического указателя 'Current Game State' */
    FORCEINLINE static bool IsValidStaticPointer()
    {
        if (!IsValid(CurrentGameState))
        {
            FPS_LOG_Empty(Error,
                "Current GameState is NOT 'AFPS_GameState' class. "
                "See Settings of current 'Game Mode'");

            return false;
        }
        return true;
    };
    //-------------------------------------------



    /* ---   Constructors   --- */

    AFPS_GameState();

    ~AFPS_GameState()
    {
        CurrentGameState = nullptr;
    };
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Вызывается при Запуске игры или при Спавне в уже запущенной игре
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Функция, вызываемая каждый кадр в этом Акторе, если не назначена другая частота */
    //virtual void Tick(float DeltaSeconds) override;

    /** Вызывается при размещении экземпляра данного класса (в редакторе) или его запуске.
    @param	Transform - Трансформация данного Актора */
    virtual void OnConstruction(const FTransform& Transform) override;

    /** Вызывается после инициализации всех компонентов только во время игрового процесса */
    virtual void PostInitializeComponents() override;

    /** Вызывается, когда этот субъект явно уничтожается во время игрового процесса или в редакторе,
    * но не вызывается во время трансляции уровней или завершения игрового процесса */
    virtual void Destroyed() override;
    //-------------------------------------------



    /* ---   Net   --- */

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    //-------------------------------------------



    /* ---   Match Management : Match State   --- */

    /** При Репликации: 'MatchState' */
    virtual void OnRep_MatchState() override;

    /** Вызывается при переходе в состояние 'WaitingToStart' ('Ожидание Начала') */
    virtual void HandleMatchIsWaitingToStart() override;

    /** Вызывается при переходе в состояние 'InProgress' ('В Процессе') */
    virtual void HandleMatchHasStarted() override;

    /** Вызывается при переходе в состояние 'WaitingPostMatch' ('Ожидание после Матча') */
    virtual void HandleMatchHasEnded() override;

    /** Возвращает значение true, если состояние матча находится 'WaitingToStart' ('Ожидание Начала') */
    FORCEINLINE virtual bool IsMatchInWaitingToStart() const
    {
        return GetCurrentMatchState() == EMatchState::WaitingToStart;
    }

    /** Возвращает значение true, если состояние матча находится 'InProgress' ('В Процессе') */
    FORCEINLINE virtual bool IsMatchInProgress() const override
    {
        return GetCurrentMatchState() == EMatchState::PreProgress
            || GetCurrentMatchState() == EMatchState::InProgress;
    }

    /** Получить Текущее состояние матча */
    FORCEINLINE EMatchState GetCurrentMatchState() const
    {
        return CurrentMatchState;
    };
    //-------------------------------------------



    /* ---   Match Management : Elapsed Time   --- */

    // Время Долгого ожидания начала Матча, когда готов хотябы Один Игрок
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "FPS Game State|Match Management",
        meta = (ForceUnits = Seconds))
    uint8 LongWaitTimeForMatchToStart = 60;

    // Время Короткого ожидания начала Матча, когда готовы все Игроки
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "FPS Game State|Match Management",
        meta = (ForceUnits = Seconds))
    uint8 ShortWaitTimeForMatchToStart = 5;

    // Время задержки начала Матча, используемая для сетевой инициализации в момент запуска Матча
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "FPS Game State|Match Management",
        meta = (ForceUnits = Seconds))
    int32 MatchStartDelayTime = 5;

    // Время продолжительности Матча
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "FPS Game State|Match Management",
        meta = (ForceUnits = Seconds))
    int32 MatchDurationTime = 300;

    // Время задержки окончания Матча, используемая для отображения результатов
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "FPS Game State|Match Management",
        meta = (ForceUnits = Seconds))
    int32 MatchEndingDelayTime = 10;

    // Время задержки перезапуска Игрока после уничтожения
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        Category = "FPS Game State|Match Management",
        meta = (ForceUnits = Seconds))
    int32 PlayerRestartTime = 5;

    //

    /** При Репликации: Только первая репликация 'ElapsedTime' ('COND_InitialOnly') */
    virtual void OnRep_ElapsedTime() override;

    /** Вызывается стандартным Таймером Матча с частотой примерно 1 раз/сек. с учётом сетевой задержки */
    virtual void DefaultTimer() override;
    //-------------------------------------------



    /* ---   Players Statistics Data   --- */

    /** Получить контейнер со Статистиками всех активных Игроков */
    FORCEINLINE const FPlayerStatisticsArray& GetPlayersStatistics() const
    {
        return PlayersStatistics;
    };

    /** Изменить Тип сортировки Данных Статистики Игроков */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Game State|Players Statistics Data",
        meta = (DisplayName = "Set Data Sorting Type for Sorted Player Statistics",
            DefaultToSelf, HideSelfPin = "true"))
    void SetDataSortingTypeForSortedPlayerStatistics(EPlayerStatisticsSortingType InType)
    {
        // Если новый тип сортировки равен старому, то сменить на противоположный тип
        // Например: новый и старый тип - это "A" по возрастанию => сменить на "A" по убыванию
        if (PlayerStatisticsSortingType == InType)
            PlayerStatisticsSortingType = EPlayerStatisticsSortingType(uint8(InType) + MAX_int8);
        else
            PlayerStatisticsSortingType = InType;

        SortingPlayerStatisticsPredicate = GetSortingPredicateForPlayerStatistics(PlayerStatisticsSortingType);

        ReSortStatisticsData();
    };

    /** Получить Данные по Индексу из сортированного списка Статистики Игроков */
    UFUNCTION(BlueprintPure,
        Category = "FPS Game State|Players Statistics Data",
        meta = (DisplayName = "Get Data by Index from Sorted Player Statistics",
            DefaultToSelf, HideSelfPin = "true"))
    const FPlayerStatisticsData& GetDataByIndexFromSortedPlayerStatistics(int32 Index) const
    {
        if (SortedPlayerStatistics.IsValidIndex(Index))
        {
            return *SortedPlayerStatistics[Index];
        }
        return FPlayerStatisticsData::Empty;
    };

    /** Пересортировать данные Статистики Игроков */
    UFUNCTION()
    void ReSortStatisticsData();
    //--------------------------------------------



    /* ---   Spectators Data   --- */

    /** Пересортировать данные Наблюдателей */
    FORCEINLINE void ReSortSpectatorsData()
    {
        SortedSpectators.Sort(SortingSpectatorsPredicate);
        OnEndSortingSpectators.Broadcast();
    };

    /** Изменить Тип сортировки Данных Наблюдателей */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Game State|Spectators Data",
        meta = (DisplayName = "Set Data Sorting Type for Sorted Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    void SetDataSortingTypeForSortedSpectators(EPlayerStatisticsSortingType InType)
    {
        // Если новый тип сортировки равен старому, то сменить на противоположный тип
        // Например: новый и старый тип - это "A" по возрастанию => сменить на "A" по убыванию
        if (SpectatorsSortingType == InType)
            SpectatorsSortingType = EPlayerStatisticsSortingType(uint8(InType) + MAX_int8);
        else
            SpectatorsSortingType = InType;

        SortingSpectatorsPredicate = GetSortingPredicateForSpectators(SpectatorsSortingType);

        ReSortSpectatorsData();
    };

    /** Получить Данные по Индексу из сортированного списка Наблюдателей */
    UFUNCTION(BlueprintPure,
        Category = "FPS Game State|Spectators Data",
        meta = (DisplayName = "Get Data by Index from Sorted Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    const FPlayerData& GetDataByIndexFromSortedSpectators(int32 Index)
    {
        if (SortedSpectators.IsValidIndex(Index))
        {
            if (SortedSpectators[Index].PlayerName.IsEmpty())
            {
                SortedSpectators[Index].UpdateDataOnPlayerState();
                // @note    Данные игрока могут быть всё ещё пусты, так как 'PlyerState' на момент 
                //          вызова может быть ещё не полностью подгружен (длительность репликации)
            }
            return SortedSpectators[Index];
        }

        return FPlayerData::Empty;
    };

    /** Получить сортированный список Наблюдателей */
    UFUNCTION(BlueprintPure,
        Category = "FPS Game State|Spectators Data",
        meta = (DisplayName = "Get Sorted Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    const TArray<FPlayerData>& GetSortedSpectators() const
    {
        return SortedSpectators;
    };
    //--------------------------------------------



    /* ---   Player Data   --- */

    /** Получить Сетевой Статус Игрока */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Game State|Player Data")
    static inline EPlayerNetworkStatus GetPlayerNetStatus(const APlayerState* PlayerState)
    {
        if (IsValid(PlayerState))
        {
            if (PlayerState == GetFPSGameState()->GetFirstPlayerState())
            {
                return EPlayerNetworkStatus::Local;
            }
            else if (((APlayerCharacterState*)PlayerState)->GetCurrentNetStatus() == ENetMode::NM_ListenServer)
            {
                return EPlayerNetworkStatus::ListenServer;
            }
            else
            {
                return EPlayerNetworkStatus::Client;
            }
        }
        else
        {
            return EPlayerNetworkStatus::NONE;
        }
    };

    /** Получить первого игрока */
    FORCEINLINE APlayerState* GetFirstPlayerState() const
    {
        if (PlayerArray.Num())
        {
            // PS: 'PlayerArray.Num()' может быть == 0
            return PlayerArray[0];
        }
        return nullptr;
    };

    /** Получить первого игрока */
    template<class T>
    FORCEINLINE T* GetFirstPlayerState() const
    {
        if (PlayerArray.Num())
        {
            // PS: 'PlayerArray.Num()' может быть == 0
            return Cast<T>(PlayerArray[0]);
        }
        return nullptr;
    };
    //-------------------------------------------



    /* ---   Role Selection   --- */

    /** Add PlayerState to the PlayerArray */
    virtual void AddPlayerState(APlayerState* PlayerState) override;

    /** Remove PlayerState from the PlayerArray. */
    virtual void RemovePlayerState(APlayerState* PlayerState) override;

    /** Перевести Клиента к Наблюдателям */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Game State|Role Selection",
        meta = (DisplayName = "Client Go To Spectators"))
    void ClientGoToSpectators(APlayerState* PlayerState)
    {
        if (!IsMatchInProgress())
        {
            RemovePlayerStatisticsData(PlayerState);
        }
    };

    /** Перевести Клиента к Игрокам */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Game State|Role Selection",
        meta = (DisplayName = "Client Go To Players"))
    void ClientGoToPlayers(APlayerState* PlayerState)
    {
        if (!IsMatchInProgress())
        {
            AddPlayerStatisticsData(PlayerState);
        }
    };

    /** Изменить Готовность Первого (локально) Игрока */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Game State|Role Selection",
        meta = (DisplayName = "Set Player Readiness"))
    void SetPlayerReadiness(const APlayerState* PlayerState, bool bReadiness = false);
    //-------------------------------------------



    /* ---   Events   --- */

    //-------------------------------------------



private:

    /* ---   Base   --- */

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* ---   Statics   --- */

    /** Получить текущий экземпляр класса 'AFPS_GameState' в среде 'Blueprint' */
    UFUNCTION(BlueprintPure,
        Category = "FPS Game",
        meta = (DisplayName = "Get FPS Game State"))
    static class AFPS_GameState* BP_GetFPSGameState()
    {
        return GetFPSGameState();
    };
    //-------------------------------------------



    /* ---   Match Management : Match State   --- */

    // Текущее состояние матча
    EMatchState CurrentMatchState;

    //

    /** Контроль и Запуск изменения состояния Матча */
    void MatchStateControl();

    /** Установить новое значение состояния Матча
    @note   Без сетевого взаимодействия */
    FORCEINLINE void SetCurrentMatchState(EMatchState State)
    {
        CurrentMatchState = State;
        OnMatchStateChange.Broadcast(CurrentMatchState);
    };

    /** Обновить 'ElapsedTime' в зависимости от количества готовых игроков */
    void CheckPlayersReadiness();
    //-------------------------------------------



    /* ---   Match Management : Elapsed Time   --- */

    /** Запустить стандартный таймер отслеживания Длительности Матча (см. 'ElapsedTime') */
    FORCEINLINE void StartDefaultTimer()
    {
        // Дублирован из 'Super::DefaultTimer()'
        GetWorldTimerManager().SetTimer(
            TimerHandle_DefaultTimer,
            this,
            &AFPS_GameState::DefaultTimer,
            GetWorldSettings()->GetEffectiveTimeDilation() / GetWorldSettings()->DemoPlayTimeDilation,
            true);
    };

    /** Остановить стандартный таймер отслеживания Длительности Матча (см. 'ElapsedTime') */
    FORCEINLINE void StopDefaultTimer()
    {
        GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);
    };

    /** Установить новое значение времени
    @note   Без сетевого взаимодействия */
    FORCEINLINE void SetElapsedTime(int32 Time)
    {
        ElapsedTime = Time;

        // Контроль таймера в зависимости от значения 'ElapsedTime'
        OnRep_ElapsedTime();
    }

    /** Установить новое значение времени с ретрансляцией на Клиенты */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetElapsedTime(int32 Time);
    //-------------------------------------------



    /* ---   Players Statistics Data   --- */

    // Эффективно реплицируемый Контейнер о Статистиках всех активных Игроков
    UPROPERTY(ReplicatedUsing = OnRep_PlayersStatistics)
    FPlayerStatisticsArray PlayersStatistics;

    // Флаг проверки синхронизации контейнера 'PlayersStatistics'
    // @note    TRUE, если все указатели != nullptr (см. 'OnRep_PlayersStatistics()')
    bool bIsPlayersStatisticsSynchronized = false;

    // Сортированный массив указателей на Статистики Игроков
    TArray<const FPlayerStatisticsData*> SortedPlayerStatistics;

    // Предикат сортировки
    // @note    Переменная-Предикат создан для уменьшения времени сортировки
    TSortingPredicate SortingPlayerStatisticsPredicate = GetSortingPredicateForPlayerStatistics(PlayerStatisticsSortingType);

    // Переменная типа сортировки
    EPlayerStatisticsSortingType PlayerStatisticsSortingType = EPlayerStatisticsSortingType::NameUp;

    //

    /** Инициализация данных Статистики */
    void InitStatisticsData();

    /** Удалить из списка данные Статистики данного Игрока */
    void RemovePlayerStatisticsData(APlayerState* PlayerState);

    /** Добавить в список данные Статистики для данного Игрока */
    void AddPlayerStatisticsData(APlayerState* PlayerState);

    /** Событие Делегата: При Удалении элементов данных статистики */
    UFUNCTION()
    void OnPreRemovingStatisticsDataItems(const TArray<int32>& RemovedIndices, int32 FinalSize);

    /** Событие Делегата: При Добавлении элементов данных статистики */
    UFUNCTION()
    void OnPostAddingStatisticsDataItems(const TArray<int32>& AddedIndices, int32 FinalSize);

    /** Получение предиката согласно выбору типа сортировки Статистики Игроков */
    TSortingPredicate GetSortingPredicateForPlayerStatistics(EPlayerStatisticsSortingType InType) const;

    /** При Репликации: 'PlayersStatistics' */
    UFUNCTION()
    void OnRep_PlayersStatistics();
    //-------------------------------------------



    /* ---   Spectators Data   --- */

    // Сортированный массив указателей на Данные Наблюдателей
    TArray<FPlayerData> SortedSpectators;

    // Предикат сортировки Наблюдателей
    // @note    Переменная-Предикат создан для уменьшения времени сортировки
    TSortingSpectatorsPredicate SortingSpectatorsPredicate = SORTING_PREDICATE(PlayerName, < );

    // Переменная типа сортировки для массива Данных Наблюдателей
    EPlayerStatisticsSortingType SpectatorsSortingType = EPlayerStatisticsSortingType::NameUp;

    //

    /** Инициализация данных Наблюдателей */
    void ReInitSpectatorsData();

    /** Удалить данные Наблюдателя из списка */
    FORCEINLINE bool RemoveSpectatorData(const APlayerState* PlayerState)
    {
        if (CurrentMatchState == EMatchState::WaitingToStart)
        {
            // Если был удалён хоть один элемент, то 'true'
            return (bool)SortedSpectators.RemoveAllSwap(
                [PlayerState](const FPlayerData& Item)
                {
                    return Item.PlayerState == PlayerState;
                });
        }
        return false;
    };

    /** Добавить данные Наблюдателя в список с предварительной проверкой на уничтожение */
    FORCEINLINE bool AddSpectatorData(const APlayerState* PlayerState)
    {
        if (CurrentMatchState == EMatchState::WaitingToStart
            && PlayerState
            && !PlayerState->IsInactive())
        {
            // Если значение является новым, то 'true' (старый размер == новому индексу)
            return SortedSpectators.Num() == SortedSpectators.AddUnique(FPlayerData(PlayerState));
        }
        return false;
    };

    /** Реакция на удаление данных Наблюдателя из списка */
    FORCEINLINE void Reaction_RemoveSpectatorData()
    {
        OnRemovingSpectators.Broadcast(SortedSpectators.Num());
        ReSortSpectatorsData();
    }

    /** Реакция на добавление данных Наблюдателя из списка */
    FORCEINLINE void Reaction_AddSpectatorData()
    {
        OnAddingSpectators.Broadcast(SortedSpectators.Num());
        ReSortSpectatorsData();
    }

    /** Получение предиката согласно выбору типа сортировки Наблюдателей */
    TSortingSpectatorsPredicate GetSortingPredicateForSpectators(EPlayerStatisticsSortingType InType) const;
    //-------------------------------------------



    /* ---   friends   --- */

    friend class UPlayerStatisticsWidget;
    friend class AFPS_GameMode;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

/** Получить текущий экземпляр класса 'AFPS_GameState' */
FORCEINLINE static AFPS_GameState* const GetFPSGameState()
{
#if WITH_EDITOR

    if (!AFPS_GameState::CurrentGameState)
    {
        return AFPS_GameState::CurrentGameState = GEngine->GameViewport->GetWorld()->GetGameState<AFPS_GameState>();
    }
    else

#endif // WITH_EDITOR

    {
        // В режиме "Play In Editor" данный указатель очищается, однако стабильно работает в готовой сборке
        return AFPS_GameState::CurrentGameState;
    }
};
//--------------------------------------------------------------------------------------
