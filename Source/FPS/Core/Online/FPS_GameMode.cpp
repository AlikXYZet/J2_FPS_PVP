//

// Base:
#include "FPS_GameMode.h"

// Global:
#include "FPS/Tools/GlobalFunctions.h"

// UE:
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameSession.h"

// Interaction:
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Interactive/AttributedActor.h"
#include "FPS_GameState.h"
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Общедоступный указатель на текущий экземпляр класса 'AFPS_GameMode'
AFPS_GameMode* AFPS_GameMode::CurrentGameMode = nullptr;
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFPS_GameMode::AFPS_GameMode()
{
    // Установка вызова функции Tick() в каждом кадре
    PrimaryActorTick.bCanEverTick = false; // Предварительно
    SetActorTickInterval(1.f); // 1 раз/сек.

    // Настройка репликации
    bReplicates = false;
    SetReplicateMovement(false);
    //-------------------------------------------


    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'AFPS_GameMode'
    CurrentGameMode = this;
    //-------------------------------------------


    /* ---   Match Management   --- */

    // Новые пользователи автоматически переходят в режим Наблюдателя
    bStartPlayersAsSpectators = true;
    // Отложенный старт: Матч НЕ начинается сразу, после создания первого игрока
    bDelayedStart = true;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AFPS_GameMode::BeginPlay()
{
    Super::BeginPlay();

    BaseInit();
}

void AFPS_GameMode::Tick(float DeltaSeconds)
{
    // Обход действия кода в 'AGameMode::Tick()'
    Super::Super::Tick(DeltaSeconds);
    // @note    Переносим логику контроля матча в 'AFPS_GameMode::CheckElapsedTimeValue(int32 Value)'
}

void AFPS_GameMode::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    /* ---   Statics   --- */

    CurrentGameMode = this;
    //-------------------------------------------
}

void AFPS_GameMode::Destroyed()
{
    /* ---   Statics   --- */

    CurrentGameMode = nullptr;
    //-------------------------------------------

    Super::Destroyed();
}

void AFPS_GameMode::BaseInit()
{
    IsValidStaticPointer();

    if (AFPS_GameState::IsValidStaticPointer())
    {
        GetFPSGameState()->OnElapsedTimeChange.AddDynamic(this, &AFPS_GameMode::CheckElapsedTimeValue);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Match Management   --- */

//void AFPS_GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
//{
//    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
//    FPS_ColorMessage(FColor::Green, "");
//}

void AFPS_GameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AFPS_GameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
}



//bool AFPS_GameMode::ReadyToStartMatch_Implementation()
//{
//    return Super::ReadyToStartMatch_Implementation();
//
//    /** Отложенный старт:
//    Определяет, Должна ли игра начинаться сразу после входа в систему первого игрока.
//    Влияет на поведение 'ReadyToStartMatch()' по умолчанию */
//    bDelayedStart;
//}

//bool AFPS_GameMode::ReadyToEndMatch_Implementation()
//{
//    return Super::ReadyToEndMatch_Implementation();
//}

//void AFPS_GameMode::StartPlay()
//{
//    Super::StartPlay();
//}



void AFPS_GameMode::HandleMatchIsWaitingToStart()
{
    Super::HandleMatchIsWaitingToStart();
    FPS_ColorMessage(FColor::Orange, "");
}

void AFPS_GameMode::HandleMatchHasStarted()
{
    InitDestructionAccounting();

    Super::HandleMatchHasStarted();
    FPS_ColorMessage(FColor::Orange, "");
}

void AFPS_GameMode::HandleMatchHasEnded()
{
    Super::HandleMatchHasEnded();
    FPS_ColorMessage(FColor::Orange, "");
}

void AFPS_GameMode::HandleLeavingMap()
{
    Super::HandleLeavingMap();
    FPS_ColorMessage(FColor::Orange, "");
}

void AFPS_GameMode::HandleMatchAborted()
{
    Super::HandleMatchAborted();
    FPS_ColorMessage(FColor::Orange, "");
}

//APawn* AFPS_GameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
//{
//    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
//}

void AFPS_GameMode::CheckElapsedTimeValue(int32 Value)
{
    if (Value == 0)
    {
        bDelayedStart = false;
        StartPlay();
    }
    else if (Value == GetFPSGameState()->MatchDurationTime)
    {
        bDelayedStart = true;
        EndMatch();
    }
}
//--------------------------------------------------------------------------------------



/* --- Destruction Accounting --- */

void AFPS_GameMode::DestructionRegistration(const UAbilitySystemComponent& TargetASC, const FGameplayEffectSpec& iSpec)
{
    TSet<APlayerController*> lPlayerWreckers;

    // Получение Игроков по Эффекту, что вызвал уничтожение
    GetAllInstigatorPlayers(iSpec, lPlayerWreckers);

    // Получить Игроков, что вызвали другие текущие активные Эффекты
    if (int32 lNum = TargetASC.GetNumActiveGameplayEffects())
    {
        TArray<FGameplayEffectSpec> OutSpecCopies;
        // Резервирование памяти
        OutSpecCopies.Reserve(lNum);
        // Копирование данных о всех Активных Эффектов.
        // PS: Данный метод почему-то НЕ имеет предварительного резервирования памяти
        TargetASC.GetAllActiveGameplayEffectSpecs(OutSpecCopies);

        GetAllInstigatorPlayers(OutSpecCopies, lPlayerWreckers);
    }

    if (lPlayerWreckers.Num())
    {
        // Если Цель является Атрибутированным Актором:
        if (Cast<AAttributedActor>(TargetASC.GetOwnerActor()))
        {
            // Заполнить массив его 'Вредителей'
            if (TSet<APlayerController*>* lAllWreckers = AllAttributedActor.Find((AAttributedActor*)TargetASC.GetOwnerActor()))
            {
                lAllWreckers->Append(lPlayerWreckers);
            }
        }
        // Если Цель является Игроком:
        else if (APlayerCharacter* lPlayer = Cast<APlayerCharacter>(TargetASC.GetOwnerActor()))
        {
            // Увеличить счётчик Смертей для Цели
            if (FPlayerStatisticsData** lTargetStats = PlayersStatisticsMap.Find(lPlayer->GetPlayerState()))
            {
                GetPlayersStatistics().AddDeaths(**lTargetStats);
            }

            auto lIterator = lPlayerWreckers.CreateIterator();
            FPlayerStatisticsData** lWreckerStats = PlayersStatisticsMap.Find((*lIterator)->PlayerState);

            // Увеличить счётчик Убийств для первого 'Вредителя'
            if (lWreckerStats
                && *lIterator != lPlayer->GetController())
            {
                GetPlayersStatistics().AddKills(**lWreckerStats);
            }

            // Увеличить счётчик Помощи для остальных 'Вредителей'
            for (++lIterator; lIterator; ++lIterator)
            {
                lWreckerStats = PlayersStatisticsMap.Find((*lIterator)->PlayerState);
                if (lWreckerStats
                    && *lIterator != lPlayer->GetController())
                {
                    GetPlayersStatistics().AddAssists(**lWreckerStats);
                }
            }

            GetPlayersStatistics().OnPostChangingArrayData.Broadcast();
        }
    }
}

void AFPS_GameMode::InitDestructionAccounting()
{
    // Заполнение Контейнера всех Игроков
    {
        PlayersStatisticsMap.Empty(GetPlayersStatistics().Items.Num());
        for (FPlayerStatisticsData& Data : GetPlayersStatistics().Items)
        {
            PlayersStatisticsMap.Add(Data.PlayerData.PlayerState, &Data);
        }
    }

    // Заполнение Контейнера всех Уничтожаемых объектов
    {
        TArray<AActor*> lActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAttributedActor::StaticClass(), lActors);
        if (lActors.Num())
        {
            AllAttributedActor.Empty(lActors.Num());
            for (AActor* Actor : lActors)
            {
                TSet<APlayerController*> lResult = AllAttributedActor.Add(
                    (AAttributedActor*)Actor, // Ключ в виде указателя на Атрибутированный Актор
                    TSet<APlayerController*>()); // Пустой конструктор с резервом по памяти

                // @note    Для 'TSet' нет конструктора с резервом памяти,
                //          поэтому резервируем следующим способом:
                lResult.Reserve(1);
            }
        }
    }
}

void AFPS_GameMode::GetAllInstigatorPlayers(const TArray<FGameplayEffectSpec>& iSpecs, TSet<APlayerController*>& oPlayers)
{
    for (const FGameplayEffectSpec& lSpec : iSpecs)
    {
        GetAllInstigatorPlayers(lSpec, oPlayers);
    }
}

void AFPS_GameMode::GetAllInstigatorPlayers(const FGameplayEffectSpec& iSpec, TSet<APlayerController*>& oPlayers)
{
    // Актор-Подстрекатель, что вызвал Эффект
    // @note    Может быть НЕ Валидным, но имеет адрес памяти [ != 0x(0) ]
    const AActor* lInstigator = iSpec.GetEffectContext().Get()->GetInstigator();

    if (TSet<APlayerController*>* lOriginalWreckers = AllAttributedActor.Find((const AAttributedActor*)lInstigator))
    {
        // Если данный "Разрушитель" был Атрибутированным Актором,
        // то скопировать его список
        oPlayers.Append(*lOriginalWreckers);
    }
    else if (const APlayerCharacter* lPC = Cast<APlayerCharacter>(lInstigator))
    {
        if (PlayersStatisticsMap.Find(lPC->GetPlayerState()))
        {
            // Если данный "Разрушитель" был Игроком,
            // то добавить его в список
            oPlayers.Add((APlayerController*)lPC->GetController());
        }
    }
}
//--------------------------------------------------------------------------------------
