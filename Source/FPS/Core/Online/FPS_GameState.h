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

// Делегат: При Завершении сортировки Массива данных Статистики Игроков
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSortingPlayerStatistics);

// Делегат: При удалении Наблюдателей
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemovingSpectators, int32, FinalSize);

// Делегат: При добавлении Наблюдателей
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAddingSpectators, int32, FinalSize);

// Делегат: При Завершении сортировки Массива данных Наблюдателей
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSortingSpectators);

// Делегат: При изменении Готовности текущего Клиента
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClientReadinessChange, bool, bReadiness);
// ----------------------------------------------------------------------------------------------------



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

    NameDown = NameUp + 0x80    UMETA(DisplayName = "Name Down"),
    PingDown    UMETA(DisplayName = "Ping Down"),
    KillsDown   UMETA(DisplayName = "Kills Down"),
    AssistsDown UMETA(DisplayName = "Assists Down"),
    DeathsDown  UMETA(DisplayName = "Deaths Down"),
    PointsDown  UMETA(DisplayName = "Points Down"),
};
//----------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

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

    // Делегат: При Завершении сортировки Массива данных Статистики Игроков
    FOnEndSortingPlayerStatistics OnEndSortingPlayerStatistics;

    // Делегат: Перед Удалением элементов Массива
    FOnRemovingSpectators OnRemovingSpectators;

    // Делегат: Добавлены элементы Массива
    FOnAddingSpectators OnAddingSpectators;

    // Делегат: При Завершении сортировки Массива данных Наблюдателей
    FOnEndSortingSpectators OnEndSortingSpectators;

    // Делегат: При изменении Готовности текущего Клиента
    FOnClientReadinessChange OnClientReadinessChange;
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

    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    virtual void Tick(float DeltaSeconds) override;

    /** Вызывается при размещении экземпляра данного класса (в редакторе) или его запуске.
    @param	Transform - Трансформация данного Актора */
    virtual void OnConstruction(const FTransform& Transform) override;

    /** Вызывается, когда этот субъект явно уничтожается во время игрового процесса или в редакторе,
    * но не вызывается во время трансляции уровней или завершения игрового процесса */
    virtual void Destroyed() override;
    //-------------------------------------------



    /* ---   Net   --- */

    /** Используется для регистрации реплицируемых Переменных и Компонентов */
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Получить Текущее сетевое состояние */
    FORCEINLINE ENetMode GetCurrentNetMode() const
    {
        return CurrentNetMode;
    };
    //-------------------------------------------



    /* ---   Match Management   --- */

    /** При Репликации: 'MatchState' */
    virtual void OnRep_MatchState() override;

    /** При Репликации: 'ElapsedTime' */
    //virtual void OnRep_ElapsedTime() override;

    /** Вызывается при переходе в состояние 'WaitingToStart' ('Ожидание Начала') */
    virtual void HandleMatchIsWaitingToStart() override;

    /** Вызывается при переходе в состояние 'InProgress' ('В Процессе') */
    //virtual void HandleMatchHasStarted() override;

    /** Возвращает значение true, если состояние матча находится 'InProgress' ('В Процессе') */
    FORCEINLINE bool IsMatchInProgress() const
    {
        return CurrentMatchState == EMatchState::InProgress;
    }

    /** Получить Текущее состояние матча */
    FORCEINLINE EMatchState GetCurrentMatchState() const
    {
        return CurrentMatchState;
    };
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
            PlayerStatisticsSortingType = EPlayerStatisticsSortingType(uint8(InType) + 0x80);
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
            SpectatorsSortingType = EPlayerStatisticsSortingType(uint8(InType) + 0x80);
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
    static EPlayerNetworkStatus GetPlayerNetStatus(const APlayerState* PlayerState)
    {
        if (IsValid(PlayerState))
        {
            if (PlayerState == PlayerState->GetWorld()->GetGameState()->PlayerArray[0])
            {
                return EPlayerNetworkStatus::Local;
            }
            else
            {
                if (PlayerState->GetPlayerId() == MAX_uint8 + 2
                    && PlayerState->GetPing() == 0)
                {
                    return EPlayerNetworkStatus::Client;
                }
                else
                {
                    return EPlayerNetworkStatus::ListenServer;
                }
            }
        }
        else
        {
            return EPlayerNetworkStatus::NONE;
        }
    };
    //-------------------------------------------



    /* ---   Role Selection   --- */

    /** Add PlayerState to the PlayerArray */
    virtual void AddPlayerState(APlayerState* PlayerState) override;

    /** Remove PlayerState from the PlayerArray. */
    virtual void RemovePlayerState(APlayerState* PlayerState) override;

    /** Изменить Готовность Первого (локально) Игрока */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Game State|Role Selection",
        meta = (DisplayName = "Set First Player Readiness"))
    void SetClientReadiness(bool bReadiness = false)
    {
        Server_SetClientReadiness(GetWorld()->GetFirstPlayerController(), bReadiness);
    };
    //-------------------------------------------



    /* ---   Events   --- */

    //-------------------------------------------



private:

    /* ---   Base   --- */

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* ---   Net   --- */

    // Текущее сетевое состояние
    ENetMode CurrentNetMode;
    //-------------------------------------------



    /* ---   Match Management   --- */

    // Текущее состояние матча
    EMatchState CurrentMatchState;
    //-------------------------------------------



    /* ---   Players Statistics Data   --- */

    // Эффективно реплицируемый Контейнер о Статистиках всех активных Игроков
    UPROPERTY(Replicated)
    FPlayerStatisticsArray PlayersStatistics;

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

    /** Событие Делегата: При Удалении элементов данных статистики */
    UFUNCTION()
    void OnPreRemovingStatisticsDataItems(const TArray<int32>& RemovedIndices, int32 FinalSize);

    /** Событие Делегата: При Добавлении элементов данных статистики */
    UFUNCTION()
    void OnPostAddingStatisticsDataItems(const TArray<int32>& AddedIndices, int32 FinalSize);

    /** Получение предиката согласно выбору типа сортировки Статистики Игроков */
    TSortingPredicate GetSortingPredicateForPlayerStatistics(EPlayerStatisticsSortingType InType) const;
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
    FORCEINLINE bool RemoveSpectatorData(APlayerState* PlayerState)
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

    /** Реакция на удаление данных Наблюдателя из списка */
    FORCEINLINE void Reaction_RemoveSpectatorData()
    {
        OnRemovingSpectators.Broadcast(SortedSpectators.Num());
        ReSortSpectatorsData();
    }

    /** Добавить данные Наблюдателя в список с предварительной проверкой на уничтожение */
    FORCEINLINE bool AddSpectatorData(APlayerState* PlayerState)
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

    /** Реакция на добавление данных Наблюдателя из списка */
    FORCEINLINE void Reaction_AddSpectatorData()
    {
        OnAddingSpectators.Broadcast(SortedSpectators.Num());
        ReSortSpectatorsData();
    }

    /** Получение предиката согласно выбору типа сортировки Наблюдателей */
    TSortingSpectatorsPredicate GetSortingPredicateForSpectators(EPlayerStatisticsSortingType InType) const;
    //-------------------------------------------



    /* ---   Role Selection   --- */

    /** Изменить Готовность Первого (локально) Игрока */
    UFUNCTION(Server, Reliable)
    void Server_SetClientReadiness(const APlayerController* Client, bool bReadiness = false);

    /** Изменить Готовность Первого (локально) Игрока на стороне вызываемого клиента */
    UFUNCTION(Client, Reliable)
    void Client_SetClientReadiness(bool bReadiness = false);
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
