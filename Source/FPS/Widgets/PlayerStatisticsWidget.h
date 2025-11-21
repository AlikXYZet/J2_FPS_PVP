//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Blueprint/UserWidget.h"

// Structs:
#include "FPS/Tools/Structs/GameData/PlayerStatisticsData.h"
#include "FPS/Tools/Structs/GameData/MatchStateData.h"

// Generated:
#include "PlayerStatisticsWidget.generated.h"
//--------------------------------------------------------------------------------------



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



    /* ---   Players Statistics Data   --- */

    /** Изменить Тип сортировки Данных Статистики Игроков */
    UFUNCTION(BlueprintCallable,
        Category = "Players Statistics Data",
        meta = (DisplayName = "Set Data Sorting Type for Sorted Player Statistics", AutoCreateRefTerm = "InType",
            DefaultToSelf, HideSelfPin = "true"))
    void SetDataSortingTypeForSortedPlayerStatistics(EPlayerStatisticsSortingType InType);

    /** Получить Данные по Индексу из сортированного списка Статистики Игроков */
    UFUNCTION(BlueprintPure,
        Category = "Players Statistics Data",
        meta = (DisplayName = "Get Data by Index from Sorted Player Statistics", AutoCreateRefTerm = "Index",
            DefaultToSelf, HideSelfPin = "true"))
    const FPlayerStatisticsData& GetDataByIndexFromSortedPlayerStatistics(int32 Index) const;
    //--------------------------------------------



    /* ---   Events   --- */

    /** Событие BP: При изменении состояния Матча */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Match Management",
        meta = (DisplayName = "On Match State Change"))
    void Event_OnMatchStateChange(EMatchState State);

    /** Событие BP: При Завершении Сортировки данных Статистики Игроков */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On End Sorting of Player Statistics"))
    void Event_OnEndSortingOfPlayerStatistics();

    /** Событие BP: При Удалении элементов данных Статистики Игроков
    @note   НЕ Вызывается во время Матча */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On Removing Player Statistics Items"))
    void Event_OnRemovingPlayerStatisticsItems(const TArray<int32>& RemovedIndices, int32 FinalSize);

    /** Событие BP: При Добавлении элементов данных Статистики Игроков
    @note   НЕ Вызывается во время Матча */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Players Statistics Data",
        meta = (DisplayName = "On Adding Player Statistics Items"))
    void Event_OnAddingPlayerStatisticsItems(const TArray<int32>& AddedIndices, int32 FinalSize);
    //--------------------------------------------



private:

    /* ---   Players Statistics Data   --- */

    /** Инициализация данных Статистики */
    void InitStatisticsData();
    //--------------------------------------------
};
