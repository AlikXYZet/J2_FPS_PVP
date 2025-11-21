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
    // Set this pawn to call Tick() every frame.
    // You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true; // Принудительно
    SetActorTickInterval(1.f); // 1 раз/сек.
    /* @note    Если требуется использовать 'Tick()' для чего-то ещё, то следует заменить данный способ на Циклический Таймер.
    Например:
    // Таймер: Контроль состояния Раунда
    // @note    Таймер с тактом раз в секунду для отслеживания времени
    GetWorld()->GetTimerManager().SetTimer(
        Timer_RoundStatusControl,
        this,
        &AFPS_GameMode::RemainingRoundTimeCounter,
        1.f,
        true);
    */

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
    FPS_LOGMessage("Test");

    BaseInit();
}

void AFPS_GameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //RemainingRoundTimeCounter();
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
}
//--------------------------------------------------------------------------------------



/* ---   Match Management   --- */

//void AFPS_GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
//{
//    Super::HandleStartingNewPlayer_Implementation(NewPlayer);
//    FPS_LOGMessage("Test");
//}
//
//void AFPS_GameMode::PostLogin(APlayerController* NewPlayer)
//{
//    Super::PostLogin(NewPlayer);
//    FPS_LOGMessage("Test");
//}
//
//void AFPS_GameMode::Logout(AController* Exiting)
//{
//    Super::Logout(Exiting);
//    FPS_LOGMessage("Test");
//}



//bool AFPS_GameMode::ReadyToStartMatch_Implementation()
//{
//    return Super::ReadyToStartMatch_Implementation();
//
//    /** Отложенный старт:
//    Определяет, Должна ли игра начинаться сразу после входа в систему первого игрока.
//    Влияет на поведение 'ReadyToStartMatch()' по умолчанию */
//    bDelayedStart;
//}
//
//bool AFPS_GameMode::ReadyToEndMatch_Implementation()
//{
//    return Super::ReadyToEndMatch_Implementation();
//}
//
//void AFPS_GameMode::HandleMatchIsWaitingToStart()
//{
//    Super::HandleMatchIsWaitingToStart();
//    FPS_LOGMessage("Test");
//}
//
//void AFPS_GameMode::HandleMatchHasStarted()
//{
//    Super::HandleMatchHasStarted();
//    FPS_LOGMessage("Test");
//}
//
//void AFPS_GameMode::HandleMatchHasEnded()
//{
//    Super::HandleMatchHasEnded();
//    FPS_LOGMessage("Test");
//}
//
//void AFPS_GameMode::HandleLeavingMap()
//{
//    Super::HandleLeavingMap();
//    FPS_LOGMessage("Test");
//}
//
//void AFPS_GameMode::HandleMatchAborted()
//{
//    Super::HandleMatchAborted();
//    FPS_LOGMessage("Test");
//}
//
//APawn* AFPS_GameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
//{
//    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
//}
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
            FPS_Message("Target is Actor");

            // Заполнить массив его 'Вредителей'
            if (TSet<APlayerController*>* lAllWreckers = AllAttributedActor.Find((AAttributedActor*)TargetASC.GetOwnerActor()))
            {
                lAllWreckers->Append(lPlayerWreckers);
            }
        }
        // Если Цель является Игроком:
        else if (APlayerCharacter* lPlayer = Cast<APlayerCharacter>(TargetASC.GetOwnerActor()))
        {
            FPS_Message("Target is Player");

            // Увеличить счётчик Смертей для Цели
            if (FPlayerStatisticsData** lTargetStats = PlayersStatisticsMap.Find((APlayerController*)lPlayer->GetController()))
            {
                GetPlayerStatistics().AddDeaths(**lTargetStats);

                FPS_ColorMessage(FColor::Green, "Add Deaths");
            }

            auto lIterator = lPlayerWreckers.CreateIterator();
            FPlayerStatisticsData** lWreckerStats = PlayersStatisticsMap.Find(*lIterator);

            // Увеличить счётчик Убийств для первого 'Вредителя'
            if (lWreckerStats
                && *lIterator != lPlayer->GetController())
            {
                GetPlayerStatistics().AddKills(**lWreckerStats);

                FPS_ColorMessage(FColor::Green, "Add Kills");
            }

            // Увеличить счётчик Помощи для остальных 'Вредителей'
            for (++lIterator; lIterator; ++lIterator)
            {
                lWreckerStats = PlayersStatisticsMap.Find(*lIterator);
                if (lWreckerStats
                    && *lIterator != lPlayer->GetController())
                {
                    GetPlayerStatistics().AddAssists(**lWreckerStats);

                    FPS_ColorMessage(FColor::Cyan, "Add Assists");
                }
            }

            GetPlayerStatistics().OnPostChangingArrayData.Broadcast();
        }
    }
}

void AFPS_GameMode::InitDestructionAccounting()
{
    TArray<AActor*> lActors;

    // Заполнение Контейнеров со всеми активными Игроками
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), lActors);
    if (lActors.Num())
    {
        int32 lOldNum = GetPlayerStatistics().Items.Num();
        int32 lNewNum = lActors.Num();

        // Резервирование и чистка памяти
        {
            PlayersStatisticsMap.Empty(lActors.Num());

            if (GameSession
                && GameSession->MaxPlayers != GetPlayerStatistics().Items.Max())
            {
                GetPlayerStatistics().Items.Reserve(GameSession->MaxPlayers);
            }

            // Убрать лишние и добавить недостающие
            GetPlayerStatistics().Items.SetNum(lNewNum, false);
        }

        // Заполнение данных
        {
            FPlayerStatisticsData* lPSD = nullptr;
            APlayerController* lPC = nullptr;

            int32 lIndex = lNewNum;
            while (lIndex)
            {
                --lIndex;

                lPC = (APlayerController*)lActors[lIndex];
                lPSD = &GetPlayerStatistics().Items[lIndex];

                // Заменить в Элементе массива указатель на 'Player State'
                GetPlayerStatistics().SetPlayerState(*lPSD, lPC->PlayerState);

                // Создание взаимосвязи между Игроком и его Статистикой (Элемент массива)
                PlayersStatisticsMap.Add(lPC, lPSD);
            }
        }

        // Информирование о изменении на стороне Сервера
        {
            if (lOldNum < lNewNum)
            {
                GetPlayerStatistics().OnPostAddingItems.Broadcast(
                    GetNumbersSeries(lOldNum, lNewNum),
                    lNewNum);

                // Пометить Массив как "изменённый"
                GetPlayerStatistics().MarkArrayDirty();
            }
            else if (lOldNum > lNewNum)
            {
                GetPlayerStatistics().OnPreRemovingItems.Broadcast(
                    GetNumbersSeries(lNewNum, lOldNum),
                    lNewNum);

                // Пометить Массив как "изменённый"
                GetPlayerStatistics().MarkArrayDirty();
            }
            else
            {
                GetPlayerStatistics().OnPostChangingArrayData.Broadcast();
            }
        }
    }

    // Заполнение Контейнера всех Уничтожаемых объектов
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
        if (PlayersStatisticsMap.Find((const APlayerController*)lPC->GetController()))
        {
            // Если данный "Разрушитель" был Игроком,
            // то добавить его в список
            oPlayers.Add((APlayerController*)lPC->GetController());
        }
    }
}
//--------------------------------------------------------------------------------------
