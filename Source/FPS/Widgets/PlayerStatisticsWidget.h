//

#pragma once

// Core:
#include "CoreMinimal.h"

// Global:
#include "FPS/Tools/GlobalFunctions.h"

// Base:
#include "Blueprint/UserWidget.h"

// Structs:
#include "FPS/Tools/Structs/GameData/PlayerStatisticsData.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"

// Generated:
#include "PlayerStatisticsWidget.generated.h"
//--------------------------------------------------------------------------------------



/* ---   typedef   --- */

// Шаблонный Тип предиката сортировки
typedef bool (*TSortingPredicate)(const FPlayerStatisticsData& first, const FPlayerStatisticsData& second);
//--------------------------------------------------------------------------------------



/* ---   ENums   --- */

// Типы сортировки Статистики Игроков
UENUM(BlueprintType)
enum struct EPlayerStatisticsSortingType : uint8
{
    NONE,

    // Возрастающий список:

    NameUp     UMETA(DisplayName = "Name Up"),
    PingUp     UMETA(DisplayName = "Ping Up"),
    KillsUp    UMETA(DisplayName = "Kills Up"),
    AssistsUp  UMETA(DisplayName = "Assists Up"),
    DeathsUp   UMETA(DisplayName = "Deaths Up"),
    PointsUp   UMETA(DisplayName = "Points Up"),

    // Спадающий список:

    NameDown = NameUp + 0x80 UMETA(DisplayName = "Name Down"),
    PingDown    UMETA(DisplayName = "Ping Down"),
    KillsDown   UMETA(DisplayName = "Kills Down"),
    AssistsDown UMETA(DisplayName = "Assists Down"),
    DeathsDown  UMETA(DisplayName = "Deaths Down"),
    PointsDown  UMETA(DisplayName = "Points Down"),
};
//----------------------------------------------------------------------------------------



UCLASS()
class FPS_API UPlayerStatisticsWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Аналогичен OnInitialized() */
    //virtual void NativeOnInitialized() override;

    /** Аналогичен PreConstruct() */
    //virtual void NativePreConstruct() override;

    /** Аналогичен Construct() */
    virtual void NativeConstruct() override;

    /** Аналогичен Destruct() */
    //virtual void NativeDestruct() override;

    /** Установка видимости виджета
    @note   Расширен с отслеживанием реакции на изменение */
    virtual void SetVisibility(ESlateVisibility InVisibility) override;
    //--------------------------------------------



    /* ---   Events   --- */

    /** Событие BP: При Завершении Сортировки данных Статистики */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On End Sorting"))
    void Event_OnEndSorting();

    /** Событие BP: При Удалении элементов данных статистики */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On Removing Items"))
    void Event_OnRemovingItems(const TArray<int32>& RemovedIndices, const int32& FinalSize);

    /** Событие BP: При Добавлении элементов данных статистики */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On Adding Items"))
    void Event_OnAddingItems(const TArray<int32>& AddedIndices, const int32& FinalSize);
    //--------------------------------------------



    /* ---   Players Statistics Data   --- */

    /** Функция изменения типа сортировки */
    UFUNCTION(BlueprintCallable,
        Category = "Players Statistics Data")
    void SetSortType(const EPlayerStatisticsSortingType InType);

    /** Получить Данные по Индексу */
    UFUNCTION(BlueprintCallable,
        Category = "Players Statistics Data")
    const FPlayerStatisticsData GetIndexData(const int32& Index);
    //--------------------------------------------



private:

    /* ---   Players Statistics Data   --- */

    // Сортированный массив указателей на Статистики Игроков
    TArray<const FPlayerStatisticsData*> SortedPlayerStatistics;

    // Предикат сортировки по указателю
    // @note    Переменная-Предикат создан для уменьшения времени сортировки
    TSortingPredicate SortingPredicate = GetSortingPredicate(SortType);

    // Переменная типа сортировки
    EPlayerStatisticsSortingType SortType = EPlayerStatisticsSortingType::NameUp;

    //

    /** Получить текущие данные Статистики Игроков */
    FORCEINLINE const TArray<FPlayerStatisticsData>& GetPlayerStatistics()
    {
        return AFPS_GameState::CurrentGameState->PlayersStatistics.Items;
    };

    /** Получить текущие данные Статистики Игроков */
    FORCEINLINE FPlayerStatisticsArray& GetPlayerStatisticsArray()
    {
        return AFPS_GameState::CurrentGameState->PlayersStatistics;
    };

    /** Инициализация данных Статистики */
    void InitStatisticsData();

    /** ПереИнициализация данных Статистики при изменении Видимости */
    FORCEINLINE void ReInitOnVisibilityChanges(const ESlateVisibility& InVisibility)
    {
        // Is NOT Visible?
        // @note    Метод 'IsVisible()' более тяжёлый
        if (InVisibility != ESlateVisibility::Collapsed
            && InVisibility != ESlateVisibility::Hidden)
        {
            GetPlayerStatisticsArray().OnPostChangingArrayData.AddDynamic(this, &UPlayerStatisticsWidget::SortPostStatisticsData);

            if (!SortedPlayerStatistics.Num())
            {
                OnPostAddingStatisticsDataItems(GetNumbersSeries(0, GetPlayerStatistics().Num()), GetPlayerStatistics().Num());
            }
            else
            {
                SortPostStatisticsData();
            }
        }
        else
        {
            GetPlayerStatisticsArray().OnPostChangingArrayData.RemoveAll(this);
        }
    };

    /** Обновить данные Статистики */
    UFUNCTION()
    void RefreshStatisticsData(const int32& Size);

    /** Пересортировать данные Статистики */
    UFUNCTION()
    void SortPostStatisticsData();

    /** Событие Делегата: При Удалении элементов данных статистики */
    UFUNCTION()
    void OnPreRemovingStatisticsDataItems(const TArray<int32>& RemovedIndices, const int32& FinalSize);

    /** Событие Делегата: При Добавлении элементов данных статистики */
    UFUNCTION()
    void OnPostAddingStatisticsDataItems(const TArray<int32>& AddedIndices, const int32& FinalSize);

    /** Получение предиката согласно выбору типа сортировки */
    TSortingPredicate GetSortingPredicate(const EPlayerStatisticsSortingType& InType);
    //--------------------------------------------
};
