//

#pragma once

// Core:
#include "CoreMinimal.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"

// Generated:
#include "PlayerStatisticsData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: Перед Удалением элементов Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPreRemovingItems, const TArray<int32>&, RemovedIndices, const int32&, FinalSize);

// Делегат: Добавлены элементы Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostAddingItems, const TArray<int32>&, AddedIndices, const int32&, FinalSize);

// Делегат: Изменены элементы Массива
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostChangingArrayData);
// ----------------------------------------------------------------------------------------------------



/* ---   ENums   --- */

// Сетевой Статус Игрока
UENUM(BlueprintType)
enum struct EPlayerNetworkStatus : uint8
{
    // Игрок НЕ Валиден
    PNS_NONE    UMETA(DisplayName = "NONE"),
    // Локальный контроллер
    PNS_Local   UMETA(DisplayName = "Local"),
    // Игрок-Сервер (локальный Контроллер для Сервера)
    PNS_ListenServer    UMETA(DisplayName = "Listen Server"),
    // Игрок с сетевым (отдалённым) контроллером
    PNS_Client UMETA(DisplayName = "Client"),

    PNS_Max UMETA(Hidden)
};
//----------------------------------------------------------------------------------------



/** Структура данных о Статистике Игрока */
USTRUCT(BlueprintType)
struct FPlayerStatisticsData : public FFastArraySerializerItem
{
    GENERATED_BODY()

    /* ---   Constructors   --- */

    FPlayerStatisticsData() {};
    //-------------------------------------------



    /* ---   Data   --- */

    // Указатель на 'Player State' Игрока, хранящий информацию о Имени и Пинге
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    APlayerState* PlayerState = nullptr;

    // Имя текущего игрока (NotReplicated)
    // @note    Необходим, так как 'PlayerState' может быть не валиден (теряются данные о Имени игрока)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NotReplicated,
        Category = "Statistics")
    FString PlayerName = FString("NONE");

    // Локальный сетевой статус Игрока (NotReplicated)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NotReplicated,
        Category = "Statistics")
    EPlayerNetworkStatus PlayerNetStatus = EPlayerNetworkStatus::PNS_NONE;

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

    // Количество Очков (NotReplicated)
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
        UpdateDataOnPlayerState();
        PointsCalculation();
    }
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

    FORCEINLINE void SetPlayerState(APlayerState* const State)
    {
        if (PlayerState != State)
        {
            PlayerState = State;
            UpdateDataOnPlayerState();
        }

        EmptyData();
    };

    /** Обновить данные, зависимые от PlayerState  */
    FORCEINLINE void UpdateDataOnPlayerState()
    {
        if (IsValid(PlayerState))
        {
            // Обновить Имя на актуальный
            if (PlayerName != PlayerState->GetPlayerName())
            {
                PlayerName = PlayerState->GetPlayerName();
            }

            // Обновить Сетевой Статус на актуальный
            if (PlayerState == PlayerState->GetWorld()->GetGameState()->PlayerArray[0])
            {
                PlayerNetStatus = EPlayerNetworkStatus::PNS_Local;
            }
            else
            {
                if (PlayerState->GetPing())
                {
                    PlayerNetStatus = EPlayerNetworkStatus::PNS_Client;
                }
                else
                {
                    PlayerNetStatus = EPlayerNetworkStatus::PNS_ListenServer;
                }
            }
        }
        else
        {
            PlayerNetStatus = EPlayerNetworkStatus::PNS_NONE;
        }
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
    //-------------------------------------------



    /* ---   Array Methods   --- */

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
            OnPreRemovingItems.Broadcast(TArray<int32>(RemovedIndices), FinalSize);

            FPS_Message("Pre Remove: %d, FS = %d", RemovedIndices.Num(), FinalSize);
        }
        else
        {
            FPS_ColorMessage(FColor::Orange, "Pre Remove");
        }
    }

    /** Предполагалось, что вызывается После Добавления элементов,
        однако вызывается Перед любым изменением массива и его элементов,
        но после срабатывания 'PreReplicatedRemove(*)' в массивах и добавленных элементов */
    void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
    {
        if (AddedIndices.Num())
        {
            for (const int32& Index : AddedIndices)
            {
                Items[Index].UpdateDataOnPlayerState();
            }

            OnPostAddingItems.Broadcast(TArray<int32>(AddedIndices), FinalSize);

            FPS_Message("Post Add: %d, FS = %d", AddedIndices.Num(), FinalSize);
        }
        else
        {
            FPS_ColorMessage(FColor::Orange, "Post Add");
        }
    }

    /** Предполагалось, что вызывается После Изменении Реплицируемых Данных элементов,
        однако вызывается Последним После любого изменения массива и его элементов */
    void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
    {
        if (ChangedIndices.Num())
        {
            OnPostChangingArrayData.Broadcast();

            FPS_Message("Post Change: %d, FS = %d", ChangedIndices.Num(), FinalSize);
        }
        else
        {
            FPS_ColorMessage(FColor::Orange, "Post Change");
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
