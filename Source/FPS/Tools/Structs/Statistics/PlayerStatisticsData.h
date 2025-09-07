//

#pragma once

// Core:
#include "CoreMinimal.h"

// Generated:
#include "PlayerStatisticsData.generated.h"
//--------------------------------------------------------------------------------------



/** Структура данных о Статистике Игрока */
USTRUCT(BlueprintType)
struct FPlayerStatisticsData : public FFastArraySerializerItem
{
    GENERATED_BODY()

    friend FPlayerStatisticsArray;

    /* ---   Data   --- */

    // Имя Игрока
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    FName Name = NAME_None;

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

    /** Вызывается после изменения Реплицируемых Данных текущего элемента */
    FORCEINLINE void PostReplicatedChange(const struct FFastArraySerializer& InArraySerializer)
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
};
//--------------------------------------------------------------------------------------



/** Массива быстрой репликации для FPlayerStatisticsData */
USTRUCT()
struct FPlayerStatisticsArray : public FFastArraySerializer
{
    GENERATED_BODY()

    /* ---   Array   --- */

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
    }
    //-------------------------------------------



    /* ---   Serializer Methods   --- */

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize<FPlayerStatisticsData, FPlayerStatisticsArray>(Items, DeltaParms, *this);
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
