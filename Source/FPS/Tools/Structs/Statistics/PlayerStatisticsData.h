//

#pragma once

// Core:
#include "CoreMinimal.h"

// Generated:
#include "PlayerStatisticsData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: Изменено количество элементов Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangingNumbers, int32, Size);

// Делегат: Изменены элементы Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangingArrayData);
// ----------------------------------------------------------------------------------------------------



/** Структура данных о Статистике Игрока */
USTRUCT(BlueprintType)
struct FPlayerStatisticsData : public FFastArraySerializerItem
{
    GENERATED_BODY()

    /* ---   Constructors   --- */

    FPlayerStatisticsData() {};
    FPlayerStatisticsData(APlayerState* InPlayerState) : PlayerState(InPlayerState) {};
    //-------------------------------------------



    /* ---   Data   --- */

    // Указатель на 'Player State' Игрока, хранящий информацию о Имени и Пинге
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    APlayerState* PlayerState = nullptr;

    // Количество Убийств (+3 Points)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    uint8 Kills = 0;

    // Количество Передач (помощи в убийстве, +1 Point)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    uint8 Assists = 0;

    // Количество Смертей (-1 Point)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    uint8 Deaths = 0;

    // Количество Очков
    // @note    На стороне Клиента вычисляется при изменении реплицируемых данных
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NotReplicated,
        Category = "Statistics")
    int32 Points = 0;
    //-------------------------------------------



    /* ---   Serializer Item Methods   --- */

    /** Вызывается перед удалением текущего элемента */
    void PreReplicatedRemove(const struct FFastArraySerializer& InArraySerializer) {}

    /** Вызывается после добавления текущего элемента */
    void PostReplicatedAdd(const struct FFastArraySerializer& InArraySerializer) {}

    /** Вызывается после изменения Реплицируемых Данных текущего элемента */
    void PostReplicatedChange(const struct FFastArraySerializer& InArraySerializer)
    {
        PointsCalculation();
    }
    //-------------------------------------------



private:

    /* ---   Data Methods   --- */

    FORCEINLINE void AddKills()
    {
        Kills += 3;
        PointsCalculation();
    };

    FORCEINLINE void AddAssists()
    {
        ++Assists;
        PointsCalculation();
    };

    FORCEINLINE void AddDeaths()
    {
        ++Deaths;
        PointsCalculation();
    };

    FORCEINLINE void PointsCalculation()
    {
        Points = Kills + Assists - Deaths;
    };
    //-------------------------------------------



    /* ---   friends   --- */

    friend FPlayerStatisticsArray;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------



/** Массива быстрой репликации для FPlayerStatisticsData */
USTRUCT()
struct FPlayerStatisticsArray : public FFastArraySerializer
{
    GENERATED_BODY()

    /* ---   Delegates   --- */

    // Делегат: Изменён сам Массив
    FOnChangingNumbers OnChangingNumbers;

    // Делегат: Изменены данные Массива
    FOnChangingArrayData OnChangingArrayData;
    //-------------------------------------------



    /* ---   Array   --- */

    // Массив данных о Статистике Игрока
    UPROPERTY()
    TArray<FPlayerStatisticsData> Items;
    //-------------------------------------------



    /* ---   Data Methods   --- */

    FORCEINLINE void AddKills(FPlayerStatisticsData& Item)
    {
        Item.AddKills();
        MarkItemDirty(Item);
    };

    FORCEINLINE void AddAssists(FPlayerStatisticsData& Item)
    {
        Item.AddAssists();
        MarkItemDirty(Item);
    };

    FORCEINLINE void AddDeaths(FPlayerStatisticsData& Item)
    {
        Item.AddDeaths();
        MarkItemDirty(Item);
    };
    //-------------------------------------------



    /* ---   Array Methods   --- */

    FORCEINLINE void Init(const FPlayerStatisticsData& Element, const int32& Number)
    {
        Items.Init(Element, Number);
        MarkArrayDirty();
        OnChangingNumbers.Broadcast(Number);
    }

    FORCEINLINE int32 Add(FPlayerStatisticsData&& Item)
    {
        int32 lIndex = Items.Add(Item);
        MarkArrayDirty();
        OnChangingNumbers.Broadcast(Items.Num());
        return lIndex;
    }
    //-------------------------------------------



    /* ---   Serializer Methods   --- */

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize<FPlayerStatisticsData, FPlayerStatisticsArray>(Items, DeltaParms, *this);
    }

    /** Предполагалось, что вызывается Перед Удалением элементов,
        однако вызывается Перед любым изменением массива и его элементов */
    void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
    {
        if (RemovedIndices.Num())
        {
            OnChangingNumbers.Broadcast(FinalSize);
        }
    }

    /** Предполагалось, что вызывается После Добавления элементов,
        однако вызывается Перед любым изменением массива и его элементов,
        но после срабатывания 'PreReplicatedRemove(*)' в массивах и добавленных элементов */
    void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
    {
        if (AddedIndices.Num())
        {
            OnChangingNumbers.Broadcast(FinalSize);
        }
    }

    /** Предполагалось, что вызывается После Изменении Реплицируемых Данных элементов,
        однако вызывается Последним После любого изменением массива и его элементов */
    void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
    {
        if (ChangedIndices.Num())
        {
            OnChangingArrayData.Broadcast();
        }
    }
    //-------------------------------------------
};

template<>
struct TStructOpsTypeTraits<FPlayerStatisticsArray> : public TStructOpsTypeTraitsBase2<FPlayerStatisticsArray>
{
    enum
    {
        WithNetDeltaSerializer = true,
    };
};
