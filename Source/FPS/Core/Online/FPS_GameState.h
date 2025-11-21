//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/GameState.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// Structs:
#include "FPS/Tools/Structs/GameData/PlayerStatisticsData.h"
#include "FPS/Tools/Structs/GameData/MatchStateData.h"

// Generated:
#include "FPS_GameState.generated.h"
//--------------------------------------------------------------------------------------



/* ---   typedef   --- */

// Шаблонный Тип предиката сортировки
typedef bool (*TSortingPredicate)(const FPlayerStatisticsData& first, const FPlayerStatisticsData& second);

// Шаблонный Тип предиката сортировки Наблюдателей
typedef bool (*TSortingSpectatorsPredicate)(const APlayerState& first, const APlayerState& second);
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При изменении состояния Матча
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchStateChange, EMatchState, NewMatchState);

// Делегат: При Завершении сортировки Массива данных Статистики Игроков
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSortingPlayerStatistics);

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

    // Эффективно реплицируемый Контейнер о Статистиках всех активных Игроков
    UPROPERTY(Replicated)
    FPlayerStatisticsArray PlayersStatistics;

    //

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
    const APlayerState* GetDataByIndexFromSortedSpectators(int32 Index) const
    {
        if (SortedSpectators.IsValidIndex(Index))
        {
            return SortedSpectators[Index];
        }

        return nullptr;
    };

    /** Получить сортированный список Наблюдателей */
    UFUNCTION(BlueprintPure,
        Category = "FPS Game State|Spectators Data",
        meta = (DisplayName = "Get Sorted Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    const TArray<APlayerState*>& GetSortedSpectators() const
    {
        return SortedSpectators;
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
    TArray<APlayerState*> SortedSpectators;

    // Предикат сортировки Наблюдателей
    // @note    Переменная-Предикат создан для уменьшения времени сортировки
    TSortingSpectatorsPredicate SortingSpectatorsPredicate = GetSortingPredicateForSpectators(SpectatorsSortingType);

    // Переменная типа сортировки для массива Данных Наблюдателей
    EPlayerStatisticsSortingType SpectatorsSortingType = EPlayerStatisticsSortingType::NameUp;

    //

    /** Инициализация данных Наблюдателей */
    void ReInitSpectatorsData();

    /** Добавить данные Наблюдателя в список с предварительной проверкой на уничтожение */
    FORCEINLINE bool AddSpectatorData(APlayerState* PlayerState)
    {
        if (CurrentMatchState == EMatchState::WaitingToStart
            && PlayerState
            && !PlayerState->IsInactive())
        {
            SortedSpectators.Add(PlayerState);
            return true;
        }
        return false;
    };

    /** Удалить данные Наблюдателя из списка */
    FORCEINLINE bool RemoveSpectatorData(APlayerState* PlayerState)
    {
        if (CurrentMatchState == EMatchState::WaitingToStart)
        {
            SortedSpectators.Remove(PlayerState);
            return true;
        }
        return false;
    };

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

    friend class AFPS_GameMode;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

/** Получить текущий экземпляр класса 'AFPS_GameState' */
FORCEINLINE static AFPS_GameState* const GetSKGameState()
{
    return AFPS_GameState::CurrentGameState;
};
//--------------------------------------------------------------------------------------
