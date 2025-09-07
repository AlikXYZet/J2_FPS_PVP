//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/GameMode.h"

// GAS:
#include "GameplayEffect.h"

// Interaction:
#include "FPS_GameState.h"

// Generated:
#include "FPS_GameMode.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class AAttributedActor;
class APlayerCharacter;
class UAbilitySystemComponent;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AFPS_GameMode : public AGameMode
{
    GENERATED_BODY()

protected:

    /* ---   Base   --- */

    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Получить текущий 'Game State', приведённый к типу "AFPS_GameState" */
    UFUNCTION(BlueprintCallable, BlueprintPure,
        Category = "FPS Game Mode",
        meta = (DisplayName = "Get FPS Game State"))
    const AFPS_GameState* BP_GetFPSGameState() const;

    /** Получить текущий 'Game State', приведённый к типу "AFPS_GameState" */
    FORCEINLINE const AFPS_GameState* GetFPSGameState() const
    {
        return (AFPS_GameState*)GameState;
    };
    //-------------------------------------------



    /* ---   Game Control   --- */


    //-------------------------------------------



    /* --- Destruction Accounting --- */

    /** Регистрация уничтожения Актора, имеющего Атрибут количества жизней
    @param  TargetASC - Компонент 'AbilitySystem' цели, что был уничтожен
    @param  Spec - Спецификация игрового Эффекта */
    void DestructionRegistration(const UAbilitySystemComponent& TargetASC, const FGameplayEffectSpec& Spec);
    //-------------------------------------------



private:

    /* ---   Base   --- */

    /** Инициализация базовых Данных */
    void BaseInit();
    //-------------------------------------------



    /* --- Destruction Accounting --- */

    // Таймер: Test
    FTimerHandle Timer_DestructionAccounting_Test;

    /* Контейнер всех Уничтожаемых объектов с указателями на Игроков, что их уничтожил ("Разрушители")
    @note   Указатели на Атр.Актор и на Игрока может быть НЕ Валиден.
            Например, Арт.Актор: Уничтожение после взрыва.
            Например, Игрок: После выхода из сервера */
    TMap<AAttributedActor*, TSet<APlayerCharacter*>> AllAttributedActor;

    /* Контейнер с указателями на данные Статистики всех активных Игроков
    @note   Указатель на Игрока (Key) может быть НЕ Валиден.
            Например: После выхода из сервера
    @note   Указатель на статистику (Value) будет ВСЕГДА Валиден (см. 'InitDestructionAccounting()'),
            поэтому дополнительная проверка излишне */
    TMap<APlayerCharacter*, FPlayerStatisticsData*> PlayersStatisticsMap;

    //

    /** Инициализация данных Учёта Уничтожения */
    void InitDestructionAccounting();

    /** Получить Массив Игроков-Разрушителей из данных игрового Эффекта
    @param  Specs - Спецификации Эффектов
    @param  OutPlayers - Заполняемый массив Игроков */
    void GetAllInstigatorPlayers(const TArray<FGameplayEffectSpec>& Specs, TSet<APlayerCharacter*>& OutPlayers);

    /** Получить Массив Игроков-Разрушителей из данных игрового Эффекта
    @param  Spec - Спецификация Эффекта
    @param  OutPlayers - Заполняемый массив Игроков */
    void GetAllInstigatorPlayers(const FGameplayEffectSpec& Spec, TSet<APlayerCharacter*>& OutPlayers);

    /** Получить контейнер со Статистиками всех активных Игроков */
    FORCEINLINE FPlayerStatisticsArray& GetPlayerStatistics() const
    {
        return ((AFPS_GameState*)GameState)->PlayersStatistics;
    };
    //-------------------------------------------
};
