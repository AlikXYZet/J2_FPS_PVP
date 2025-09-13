//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Blueprint/UserWidget.h"

// Structs:
#include "FPS/Tools/Structs/Statistics/PlayerStatisticsData.h"

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

    // Возрастающий список
    NameUp     UMETA(DisplayName = "Name Up"),
    PingUp     UMETA(DisplayName = "Ping Up"),
    KillsUp    UMETA(DisplayName = "Kills Up"),
    AssistsUp  UMETA(DisplayName = "Assists Up"),
    DeathsUp   UMETA(DisplayName = "Deaths Up"),
    PointsUp   UMETA(DisplayName = "Points Up"),

    // Спадающий список
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
    virtual void NativeOnInitialized() override;

    /** Аналогичен Construct() */
    virtual void NativeConstruct() override;
    //--------------------------------------------



    /* ---   Events   --- */

    /** Событие BP: При Обновлении данных Статистики */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On Refreshing"))
    void Event_OnRefreshing(int32 Num);

    /** Событие BP: При Завершении Сортировки данных Статистики */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On End Sorting"))
    void Event_OnEndSorting();
    //--------------------------------------------



    /* ---   Players Statistics Data   --- */

    /** Функция изменения типа сортировки */
    UFUNCTION(BlueprintCallable,
        Category = "Players Statistics Data")
    void SetSortType(const EPlayerStatisticsSortingType InType);

    UFUNCTION(BlueprintCallable,
        Category = "Players Statistics Data",
        meta = (META_DATA_SPECIFIER))
    const FPlayerStatisticsData GetIndexData(const int32& Index);
    //--------------------------------------------



private:

    /* ---   Players Statistics Data   --- */

    // Указатель на текущий массив PlayerStatistics
    const TArray<FPlayerStatisticsData>* CurrentPlayerStatistics = nullptr;

    // Сортированный массив указателей на Статистики Игроков
    TArray<const FPlayerStatisticsData*> SortedPlayerStatistics;

    // Переменная типа сортировки
    EPlayerStatisticsSortingType SortType = EPlayerStatisticsSortingType::NameUp;

    // Предикат сортировки по указателю
    // @note    Переменная-Предикат создан для уменьшения времени сортировки
    TSortingPredicate SortingPredicate = GetSortingPredicate(SortType);

    //

    /** Инициализация данных Статистики */
    void InitStatisticsData();

    /** Обновить данные Статистики */
    UFUNCTION()
    void RefreshStatisticsData(int32 Size);

    /** Пересортировать данные Статистики */
    UFUNCTION()
    void SortStatisticsData();

    /** Получение предиката согласно выбору типа сортировки */
    TSortingPredicate GetSortingPredicate(const EPlayerStatisticsSortingType& InType);
    //--------------------------------------------
};
