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

    FPlayerData(APlayerState* NewPlayer)
        : PlayerState(NewPlayer)
    {
        UpdateDataOnPlayerState();
    };
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
    void UpdateDataOnPlayerState()
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
                PlayerNetStatus = EPlayerNetworkStatus::Local;
            }
            else
            {
                if (PlayerState->GetPlayerId() == MAX_uint8 + 2
                    && PlayerState->GetPing() == 0)
                {
                    PlayerNetStatus = EPlayerNetworkStatus::ListenServer;
                }
                else
                {
                    PlayerNetStatus = EPlayerNetworkStatus::Client;
                }
            }
        }
        else
        {
            PlayerNetStatus = EPlayerNetworkStatus::NONE;
        }
    };
    //-------------------------------------------



    /* ---   Operators   --- */

    FORCEINLINE bool operator == (const FPlayerData& Other) const
    {
        return PlayerState == Other.PlayerState;
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
