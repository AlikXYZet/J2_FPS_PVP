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
        &AFPS_GameMode::RoundTimerCounter,
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
    Super::Tick(DeltaSeconds);

    RoundTimerCounter();
}

const AFPS_GameState* AFPS_GameMode::BP_GetFPSGameState() const
{
    return AFPS_GameState::CurrentGameState;
}

void AFPS_GameMode::BaseInit()
{
    if (!AFPS_GameState::IsValidStaticPointer())
    {
        FPS_LOG(Error, TEXT("'%s' is NOT 'AFPS_GameState'"),
            GameState
            ? *GameState->GetFName().ToString()
            : *FString("None"));
    }
}
//--------------------------------------------------------------------------------------



/* ---   Game Control   --- */


//--------------------------------------------------------------------------------------



/* --- Destruction Accounting --- */

void AFPS_GameMode::DestructionRegistration(const UAbilitySystemComponent& TargetASC, const FGameplayEffectSpec& iSpec)
{
    TSet<APlayerCharacter*> lPlayerWreckers;

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
            if (TSet<APlayerCharacter*>* lAllWreckers = AllAttributedActor.Find((AAttributedActor*)TargetASC.GetOwnerActor()))
            {
                lAllWreckers->Append(lPlayerWreckers);
            }
        }
        // Если Цель является Игроком:
        else if (APlayerCharacter* lPlayer = Cast<APlayerCharacter>(TargetASC.GetOwnerActor()))
        {
            // Увеличить счётчик Смертей для Цели
            if (FPlayerStatisticsData** lTargetStats = PlayersStatisticsMap.Find(lPlayer))
            {
                GetPlayerStatistics().AddDeaths(**lTargetStats);
            }

            auto lIterator = lPlayerWreckers.CreateIterator();
            FPlayerStatisticsData** lWreckerStats = PlayersStatisticsMap.Find(*lIterator);

            // Увеличить счётчик Убийств для первого 'Вредителя'
            if (lWreckerStats
                && *lIterator != lPlayer)
            {
                GetPlayerStatistics().AddKills(**lWreckerStats);
            }

            // Увеличить счётчик Помощи для остальных 'Вредителей'
            for (++lIterator; lIterator; ++lIterator)
            {
                lWreckerStats = PlayersStatisticsMap.Find(*lIterator);
                if (lWreckerStats
                    && *lIterator != lPlayer)
                {
                    GetPlayerStatistics().AddAssists(**lWreckerStats);
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
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), lActors);
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
            APlayerCharacter* lPC = nullptr;

            int32 lIndex = lNewNum;
            while (lIndex)
            {
                --lIndex;

                lPC = (APlayerCharacter*)lActors[lIndex];
                lPSD = &GetPlayerStatistics().Items[lIndex];

                // Заменить в Элементе массива указатель на 'Player State'
                GetPlayerStatistics().SetPlayerState(*lPSD, lPC->GetPlayerState());

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
            TSet<APlayerCharacter*> lResult = AllAttributedActor.Add(
                (AAttributedActor*)Actor, // Ключ в виде указателя на Атрибутированный Актор
                TSet<APlayerCharacter*>()); // Пустой конструктор с резервом по памяти

            // @note    Для 'TSet' нет конструктора с резервом памяти,
            //          поэтому резервируем следующим способом:
            lResult.Reserve(1);
        }
    }
}

void AFPS_GameMode::GetAllInstigatorPlayers(const TArray<FGameplayEffectSpec>& iSpecs, TSet<APlayerCharacter*>& oPlayers)
{
    for (const FGameplayEffectSpec& lSpec : iSpecs)
    {
        GetAllInstigatorPlayers(lSpec, oPlayers);
    }
}

void AFPS_GameMode::GetAllInstigatorPlayers(const FGameplayEffectSpec& iSpec, TSet<APlayerCharacter*>& oPlayers)
{
    // Актор-Подстрекатель, что вызвал Эффект
    // @note    Может быть НЕ Валидным, но имеет адрес памяти [ != 0x(0) ]
    const AActor* lInstigator = iSpec.GetEffectContext().Get()->GetInstigator();

    if (PlayersStatisticsMap.Find((const APlayerCharacter*)lInstigator))
    {
        // Если данный "Разрушитель" был Игроком,
        // то добавить его в список
        oPlayers.Add((APlayerCharacter*)lInstigator);
    }
    else if (TSet<APlayerCharacter*>* lOriginalWreckers = AllAttributedActor.Find((const AAttributedActor*)lInstigator))
    {
        // Если данный "Разрушитель" был Атрибутированным Актором,
        // то скопировать его список
        oPlayers.Append(*lOriginalWreckers);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Round Control   --- */

void AFPS_GameMode::RoundTimerCounter()
{
    --RemainingTimeCounter;

    if (RemainingTimeCounter <= 0)
    {
        switch (NextRoundStatus())
        {
        case ERoundStatus::Selection:
            RemainingTimeCounter = SelectionPeriodTime;
            break;

        case ERoundStatus::Round:
            InitDestructionAccounting();
            RemainingTimeCounter = RoundPeriodTime;
            break;

        case ERoundStatus::Results:
            RemainingTimeCounter = ResultsPeriodTime;
            break;

        default:
            break;
        }

        FPS_LOG(Warning, "New Status == %d",
            GetRoundStatus());
    }
}
//--------------------------------------------------------------------------------------
