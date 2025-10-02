//

// Base:
#include "FPS_GameMode.h"

// UE:
#include "Kismet/GameplayStatics.h"

// Interaction:
#include "FPS/Characters/PlayerCharacter.h"
#include "FPS/Interactive/AttributedActor.h"
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

// Общедоступный указатель на текущий экземпляр класса 'AFPS_GameMode'
AFPS_GameMode* AFPS_GameMode::CurrentGameMode = nullptr;
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AFPS_GameMode::BeginPlay()
{
    Super::BeginPlay();

    BaseInit();

    // Test: Имитация процесса запуска игровой сессии
    GetWorld()->GetTimerManager().SetTimer(
        Timer_DestructionAccounting_Test,
        this,
        &AFPS_GameMode::InitDestructionAccounting,
        3.f,
        false);
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

            GetPlayerStatistics().OnChangingArrayData.Broadcast();
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
        PlayersStatisticsMap.Reserve(lActors.Num());
        GetPlayerStatistics().Items.Empty(lActors.Num());
        // @note    Предварительный резерв необходим для сохранения
        //          валидности указателей при заполнении контейнеров

        for (int32 lIndex = 0; lIndex < lActors.Num(); ++lIndex)
        {
            // Создание данных с указателем на 'Player State'
            GetPlayerStatistics().Add(
                FPlayerStatisticsData(
                    ((APlayerCharacter*)lActors[lIndex])->GetPlayerState()));

            // Создание взаимосвязи между Игроком и его Статистикой
            PlayersStatisticsMap.Add(
                (APlayerCharacter*)lActors[lIndex],
                &GetPlayerStatistics().Items[lIndex]);
        }
    }

    // Заполнение Контейнера всех Уничтожаемых объектов
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAttributedActor::StaticClass(), lActors);
    if (lActors.Num())
    {
        AllAttributedActor.Reserve(lActors.Num());
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
