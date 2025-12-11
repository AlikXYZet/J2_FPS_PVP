//

#pragma once

// Core:
#include "CoreMinimal.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"

// Generated:
#include "PlayerData.generated.h"
//--------------------------------------------------------------------------------------



/* ---   ENums   --- */

// Сетевой Статус Игрока
UENUM(BlueprintType)
enum struct EPlayerNetworkStatus : uint8
{
    // Игрок НЕ Валиден
    NONE,
    // Локальный контроллер
    Local,
    // Игрок-Сервер (локальный Контроллер для Сервера)
    ListenServer    UMETA(DisplayName = "Listen Server"),
    // Игрок с сетевым (отдалённым) контроллером
    Client,

    MAX UMETA(Hidden)
};
//----------------------------------------------------------------------------------------



/** Структура данных Наблюдателей */
USTRUCT(BlueprintType)
struct FPlayerData
{
    GENERATED_BODY()

    /* ---   Constructors   --- */

    FPlayerData() {};

    FPlayerData(const APlayerState* NewPlayer)
        : PlayerState(NewPlayer)
    {
        UpdateDataOnPlayerState();
    };
    //-------------------------------------------



    /* ---   Data   --- */

    // Указатель на 'Player State' Игрока, хранящий информацию о Имени и Пинге
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Statistics")
    const APlayerState* PlayerState = nullptr;

    // Имя текущего игрока (NotReplicated)
    // @note    Необходим, так как 'PlayerState' может быть не валиден (теряются данные о Имени игрока)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NotReplicated,
        Category = "Statistics")
    FString PlayerName = FString("NONE");

    // Локальный сетевой статус Игрока (NotReplicated)
    // @note    На стороне Клиента определяется при изменении 'PlayerState'
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, NotReplicated,
        Category = "Statistics")
    EPlayerNetworkStatus PlayerNetStatus = EPlayerNetworkStatus::NONE;
    //-------------------------------------------



    /* ---   Data Methods: Player State   --- */

    FORCEINLINE void SetPlayerState(APlayerState* const State)
    {
        if (PlayerState != State)
        {
            PlayerState = State;
            UpdateDataOnPlayerState();
        }
    };

    /** Обновить данные, зависимые от PlayerState  */
    void UpdateDataOnPlayerState();
    //-------------------------------------------



    /* ---   Operators   --- */

    FORCEINLINE bool operator == (const FPlayerData& Other) const
    {
        return PlayerState == Other.PlayerState;
    };

    FORCEINLINE bool operator == (const APlayerState* OtherPlayerState) const
    {
        return PlayerState == OtherPlayerState;
    };
    //-------------------------------------------



    /* ---   Statics   --- */
    // Пустые данные Наблюдателей
    static const FPlayerData Empty;
    //-------------------------------------------



private:
};
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Пустые данные Статистики
__declspec(selectany) const FPlayerData FPlayerData::Empty = FPlayerData();
//--------------------------------------------------------------------------------------
