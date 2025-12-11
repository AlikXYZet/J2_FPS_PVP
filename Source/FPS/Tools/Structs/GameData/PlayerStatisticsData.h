//

#pragma once

// Core:
#include "CoreMinimal.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"

// Structs:
#include "PlayerData.h"

// Generated:
#include "PlayerStatisticsData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: Перед Удалением элементов Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPreRemovingItems, const TArray<int32>&, RemovedIndices, int32, FinalSize);

// Делегат: Добавлены элементы Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostAddingItems, const TArray<int32>&, AddedIndices, int32, FinalSize);

// Делегат: Изменены элементы Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostChangingArrayData);
// ----------------------------------------------------------------------------------------------------



/** Структура данных о Статистике Игрока */
USTRUCT(BlueprintType)
struct FPlayerStatisticsData : public FFastArraySerializerItem
{
    GENERATED_BODY()

    /* ---   Constructors   --- */

    FPlayerStatisticsData() {};

    FPlayerStatisticsData(const APlayerState* NewPlayer)
        : PlayerData(NewPlayer) {};
    //-------------------------------------------



    /* ---   Data   --- */

    // Данные игрока ('APlayerState', Имя и Сетевой статус)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    FPlayerData PlayerData;

    // Количество Убийств (+3 Points)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    int32 Kills = 0;

    // Количество Передач (помощи в убийстве, +1 Point)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    int32 Assists = 0;

    // Количество Смертей (-1 Point)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    int32 Deaths = 0;

    // Количество Очков (NotReplicated)
    // @note    На стороне Клиента вычисляется при изменении реплицируемых данных
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NotReplicated,
        Category = "Statistics")
    int32 Points = 0;

    // Готовность данного Игрока
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    bool bPlayerReadiness = false;
    /* PS: При расширении до Булеанов, заменить на:
    uint8 bPlayerReadiness : 1; */
    //-------------------------------------------



    /* ---   Serializer Item Methods   --- */

    /** Вызывается перед удалением текущего элемента */
    void PreReplicatedRemove(const struct FFastArraySerializer& InArraySerializer) {}

    /** Вызывается после добавления текущего элемента */
    void PostReplicatedAdd(const struct FFastArraySerializer& InArraySerializer)
    {
        UpdateDataOnPlayerState();
    }

    /** Вызывается после изменения Реплицируемых Данных текущего элемента */
    void PostReplicatedChange(const struct FFastArraySerializer& InArraySerializer)
    {
        UpdateDataOnPlayerState();
        PointsCalculation();
    }
    //-------------------------------------------



    /* ---   Operators   --- */

    FORCEINLINE bool operator == (const FPlayerStatisticsData& Other) const
    {
        return PlayerData == Other.PlayerData;
    };

    FORCEINLINE bool operator == (const APlayerState* OtherPlayerState) const
    {
        return PlayerData == OtherPlayerState;
    };
    //-------------------------------------------



    /* ---   Statics   --- */

    // Пустые данные Статистики
    static const FPlayerStatisticsData Empty;
    //-------------------------------------------



private:

    /* ---   Data Methods   --- */

    FORCEINLINE void AddKills()
    {
        ++Kills;
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
        Points = (Kills * 2) + Assists - Deaths;
    };

    FORCEINLINE void EmptyData()
    {
        Kills = 0;
        Assists = 0;
        Deaths = 0;
        Points = 0;
    };
    //-------------------------------------------



    /* ---   Data Methods: Player State   --- */

    FORCEINLINE void SetPlayerState(APlayerState* const PlayerState)
    {
        if (PlayerData.PlayerState != PlayerState)
        {
            PlayerData.SetPlayerState(PlayerState);
        }

        EmptyData();
    };

    /** Обновить данные, зависимые от PlayerState  */
    FORCEINLINE void UpdateDataOnPlayerState()
    {
        PlayerData.UpdateDataOnPlayerState();
    };
    //-------------------------------------------



    /* ---   friends   --- */

    friend FPlayerStatisticsArray;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------



/** Массива быстрой репликации для FPlayerStatisticsData */
USTRUCT(BlueprintType)
struct FPlayerStatisticsArray : public FFastArraySerializer
{
    GENERATED_BODY()

    /* ---   Delegates   --- */

    // Делегат: Перед Удалением элементов Массива
    FOnPreRemovingItems OnPreRemovingItems;

    // Делегат: Добавлены элементы Массива
    FOnPostAddingItems OnPostAddingItems;

    // Делегат: Изменены данные Массива
    FOnPostChangingArrayData OnPostChangingArrayData;
    //-------------------------------------------



    /* ---   Array   --- */

    // Массив данных о Статистике Игрока
    UPROPERTY()
    TArray<FPlayerStatisticsData> Items;
    //-------------------------------------------



    /* ---   Data Methods   --- */

    FORCEINLINE void SetPlayerState(FPlayerStatisticsData& Item, APlayerState* const State)
    {
        Item.SetPlayerState(State);
        MarkItemDirty(Item);
    };

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

    FORCEINLINE void SetReadiness(FPlayerStatisticsData& Item, bool bReadiness)
    {
        Item.bPlayerReadiness = bReadiness;
        MarkItemDirty(Item);
        OnPostChangingArrayData.Broadcast();
    };
    //-------------------------------------------



    /* ---   Array Data Methods   --- */

    void AddPlayer(const APlayerState* NewPlayer)
    {
        int32 lIndex = Items.Find(NewPlayer);

        if (lIndex == INDEX_NONE)
        {
            lIndex = Items.AddUnique(FPlayerStatisticsData(NewPlayer));

            OnPostAddingItems.Broadcast(
                TArray<int32>{lIndex},
                Items.Num());

            MarkArrayDirty();
        }
    };

    void RemovePlayer(const APlayerState* OldPlayer)
    {
        int32 lIndex = Items.Find(OldPlayer);

        if (lIndex != INDEX_NONE)
        {
            OnPreRemovingItems.Broadcast(
                TArray<int32>{lIndex},
                Items.Num() - 1);

            Items.RemoveAt(lIndex);

            MarkArrayDirty();
        }
    };
    //-------------------------------------------



    /* ---   Array Methods   --- */

    FORCEINLINE int32 Num() const
    {
        return Items.Num();
    }

    FORCEINLINE int32 Find(const FPlayerStatisticsData& Item) const
    {
        return Items.Find(Item);
    }

    FORCEINLINE int32 Find(const APlayerState* PS) const
    {
        return Items.Find(PS);
    }

    FORCEINLINE void RemoveAt(int32 Index)
    {
        Items.RemoveAt(Index);
    }

    FORCEINLINE FPlayerStatisticsData& operator[](int32 Index)
    {
        return Items[Index];
    }

    FORCEINLINE const FPlayerStatisticsData& operator[](int32 Index) const
    {
        return Items[Index];
    }

    FORCEINLINE auto begin()
    {
        return Items.begin();
    }

    FORCEINLINE auto begin() const
    {
        return Items.begin();
    }

    FORCEINLINE auto end()
    {
        return Items.end();
    }

    FORCEINLINE auto end() const
    {
        return Items.end();
    }
    //-------------------------------------------



    /* ---   Serializer Methods   --- */

    bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
    {
        return FFastArraySerializer::FastArrayDeltaSerialize<FPlayerStatisticsData, FPlayerStatisticsArray>(Items, DeltaParams, *this);
    }

    /** Предполагалось, что вызывается Перед Удалением элементов,
        однако вызывается Перед любым изменением массива и его элементов */
    void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
    {
        if (RemovedIndices.Num())
        {
            OnPreRemovingItems.Broadcast(TArray<int32>(RemovedIndices), FinalSize);
        }
    }

    /** Предполагалось, что вызывается После Добавления элементов,
        однако вызывается Перед любым изменением массива и его элементов,
        но после срабатывания 'PreReplicatedRemove(*)' в массивах и добавленных элементов */
    void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
    {
        if (AddedIndices.Num())
        {
            for (int32 Index : AddedIndices)
            {
                Items[Index].UpdateDataOnPlayerState();
            }

            OnPostAddingItems.Broadcast(TArray<int32>(AddedIndices), FinalSize);
        }
    }

    /** Предполагалось, что вызывается После Изменении Реплицируемых Данных элементов,
        однако вызывается Последним После любого изменения массива и его элементов */
    void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
    {
        if (ChangedIndices.Num())
        {
            OnPostChangingArrayData.Broadcast();
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
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Пустые данные Статистики
__declspec(selectany) const FPlayerStatisticsData FPlayerStatisticsData::Empty = FPlayerStatisticsData();
//--------------------------------------------------------------------------------------
