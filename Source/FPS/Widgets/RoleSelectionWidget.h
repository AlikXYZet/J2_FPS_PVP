//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "PlayerStatisticsWidget.h"

// Generated:
#include "RoleSelectionWidget.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API URoleSelectionWidget : public UPlayerStatisticsWidget
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

    /** Установка видимости виджета */
    virtual void SetVisibility(ESlateVisibility InVisibility) override;
    //--------------------------------------------



    /* ---   Spectators Data   --- */

    /** Изменить Тип сортировки Данных Наблюдателей */
    UFUNCTION(BlueprintCallable,
        Category = "Spectators Data",
        meta = (DisplayName = "Set Data Sorting Type for Sorted Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    void SetDataSortingTypeForSortedSpectators(EPlayerStatisticsSortingType InType) const;

    /** Получить Данные по Индексу из сортированного списка Наблюдателей */
    UFUNCTION(BlueprintPure,
        Category = "Spectators Data",
        meta = (DisplayName = "Get Data by Index from Sorted Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    const FPlayerData& GetDataByIndexFromSortedSpectators(int32 Index) const;

    /** Получить сортированный список Наблюдателей */
    UFUNCTION(BlueprintPure,
        Category = "Spectators Data",
        meta = (DisplayName = "Get Sorted Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    const TArray<FPlayerData>& GetSortedSpectators() const;
    //--------------------------------------------



    /* ---   Role Selection   --- */

    /** Перейти к Наблюдателям */
    UFUNCTION(BlueprintCallable,
        Category = "Role Selection",
        meta = (DisplayName = "Go To Spectators",
            DefaultToSelf, HideSelfPin = "true"))
    void GoToSpectators() const;

    /** Перейти к Игрокам */
    UFUNCTION(BlueprintCallable,
        Category = "Role Selection",
        meta = (DisplayName = "Go To Players",
            DefaultToSelf, HideSelfPin = "true"))
    void GoToPlayers() const;

    /** Изменить Готовность Владельца */
    UFUNCTION(BlueprintCallable,
        Category = "Role Selection",
        meta = (DisplayName = "Set Owner Readiness",
            DefaultToSelf, HideSelfPin = "true"))
    void SetOwnerReadiness(bool bReadiness = false) const;
    //--------------------------------------------



    /* ---   Events   --- */

    /** Событие BP: При Завершении Сортировки данных Наблюдателей
    @note   НЕ Вызывается во время Матча */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Spectators Data",
        meta = (DisplayName = "On End Sorting Of Spectators"))
    void Event_OnEndSortingOfSpectators();

    /** Событие BP: При Удалении элементов данных Наблюдателей
    @note   НЕ Вызывается во время Матча */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Spectators Data",
        meta = (DisplayName = "On Removing Spectators Items"))
    void Event_OnRemovingSpectatorsItems(int32 FinalSize);

    /** Событие BP: При Добавлении элементов данных Наблюдателей
    @note   НЕ Вызывается во время Матча */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Spectators Data",
        meta = (DisplayName = "On Adding Spectators Items"))
    void Event_OnAddingSpectatorsItems(int32 FinalSize);

    /** Событие BP: При изменении Готовности Владельца */
    UFUNCTION(BlueprintImplementableEvent,
        Category = "Role Selection",
        meta = (DisplayName = "On Owner Readiness Change"))
    void Event_OnOwnerReadinessChange(bool bReadiness);
    //--------------------------------------------



private:

    /* ---   Role Selection   --- */

    /** Инициализация данных с GameState */
    void InitWithGameState();

    /** Инициализация данных с PlayerController */
    void InitWithPlayerController();
    //--------------------------------------------
};
