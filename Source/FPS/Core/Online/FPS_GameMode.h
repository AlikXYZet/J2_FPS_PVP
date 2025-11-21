//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/GameMode.h"

// Macros:
#include "FPS/Tools/GlobalMacros.h"

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

public:

    /* ---   Statics   --- */

    // Общедоступный указатель на текущий экземпляр класса 'AFPS_GameMode'
    // @note    Используется для уменьшения зависимостей и использования излишних функций
    //          Например, функций 'Cast<>' и методов Инициализации в других классах
    static AFPS_GameMode* CurrentGameMode;

    //

    /** Метод проверки валидности статического указателя 'Current Game Mode' */
    FORCEINLINE static bool IsValidStaticPointer()
    {
        if (!IsValid(CurrentGameMode))
        {
            FPS_LOG_Empty(Error,
                "Current GameMode is NOT 'AFPS_GameMode' class. "
                "See 'World Settings'");

            return false;
        }
        return true;
    };
    //-------------------------------------------



    /* ---   Constructors   --- */

    AFPS_GameMode();

    ~AFPS_GameMode()
    {
        CurrentGameMode = nullptr;
    }
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    virtual void Tick(float DeltaSeconds) override;

    /** Вызывается при размещении экземпляра данного класса (в редакторе) или его запуске.
    @param	Transform	- Трансформация данного Актора */
    virtual void OnConstruction(const FTransform& Transform) override;

    /** Вызывается, когда этот субъект явно уничтожается во время игрового процесса или в редакторе,
    * но не вызывается во время трансляции уровней или завершения игрового процесса */
    virtual void Destroyed() override;
    //-------------------------------------------



    /* ---   Match Management   --- */

    ///** Сигнализирует о том, что игрок готов войти в игру, что может привести к ее запуску
    //@param  NewPlayer - Контроллер Игрока, уведомляющий о готовности */
    //virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    ///** Вызывается после успешного подключения нового Игрока.
    //@note   Это первое безопасное место для вызова реплицированных функций в PlayerController.
    //@param  NewPlayer - Контроллер подключившегося Игрока */
    //virtual void PostLogin(APlayerController* NewPlayer) override;

    ///** Вызывается, когда Игрок (контроллер с 'PlayerState') покидает игру или уничтожается
    //@param  Exiting - Контроллер ушедшего Игрока */
    //virtual void Logout(AController* Exiting) override;



    // 'Игры' должны переопределять эти функции, чтобы иметь дело со своей логикой, специфичной для игры

    ///** Возвращает значение true, если всё готово для начала матча
    //@note   'Игры' должны переопределять это значение */
    //virtual bool ReadyToStartMatch_Implementation() override;

    ///** Возвращает значение true, если матч готов к завершению
    //@note   'Игры' должны переопределять это значение */
    //virtual bool ReadyToEndMatch_Implementation() override;

    ///** Вызывается при переходе в состояние 'WaitingToStart' ('Ожидание Начала') */
    //virtual void HandleMatchIsWaitingToStart() override;

    ///** Вызывается при переходе в состояние 'InProgress' ('В Процессе') */
    //virtual void HandleMatchHasStarted() override;

    ///** Вызывается, когда карта переходит в режим  'WaitingPostMatch' ('Ожидание После Матча') */
    //virtual void HandleMatchHasEnded() override;

    ///** Вызывается при переходе в состояние 'LeavingMap' ('Покидая Карту') */
    //virtual void HandleLeavingMap() override;

    ///** Вызывается при переходе в состояние 'Aborted' ('Прервано') */
    //virtual void HandleMatchAborted() override;

    ///** Вызывается во время распределения Пешек Контроллерам
    //@param  NewPlayer - Контроллер, которому передаётся (создаётся) Пешка
    //@param  StartSpot - Метка создания
    //@return Пешка, которая отдана под управлением Контроллера */
    //virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;
    //-------------------------------------------



    /* ---   Destruction Accounting   --- */

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

    /* Контейнер всех Уничтожаемых объектов с указателями на Игроков, что их уничтожил ("Разрушители")
    @note   Указатели на Атр.Актор и на Контроллер Игрока может быть НЕ Валиден.
            Например, Арт.Актор: Уничтожение после взрыва.
            Например, Контроллер Игрока: После выхода из сервера */
    TMap<AAttributedActor*, TSet<APlayerController*>> AllAttributedActor;

    /* Контейнер с указателями на данные Статистики всех активных Игроков
    @note   Указатель на Контроллер Игрока (Key) может быть НЕ Валиден.
            Например: После выхода из сервера
    @note   Указатель на статистику (Value) будет Валиден в течении  (см. 'InitDestructionAccounting()'),
            поэтому дополнительная проверка излишне */
    TMap<APlayerController*, FPlayerStatisticsData*> PlayersStatisticsMap;

    //

    /** Инициализация данных Учёта Уничтожения */
    void InitDestructionAccounting();

    /** Получить Массив Игроков-Разрушителей из данных игрового Эффекта
    @param  Specs - Спецификации Эффектов
    @param  OutPlayers - Заполняемый массив Игроков */
    void GetAllInstigatorPlayers(const TArray<FGameplayEffectSpec>& Specs, TSet<APlayerController*>& OutPlayers);

    /** Получить Массив Игроков-Разрушителей из данных игрового Эффекта
    @param  Spec - Спецификация Эффекта
    @param  OutPlayers - Заполняемый массив Игроков */
    void GetAllInstigatorPlayers(const FGameplayEffectSpec& Spec, TSet<APlayerController*>& OutPlayers);

    /** Получить контейнер со Статистиками всех активных Игроков */
    FORCEINLINE FPlayerStatisticsArray& GetPlayerStatistics() const
    {
        return AFPS_GameState::CurrentGameState->PlayersStatistics;
    };
    //-------------------------------------------



    /* ---   friends   --- */

    friend class AFPS_GameState;
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------



/* ---   Statics   --- */

/** Получить текущий экземпляр класса 'AFPS_GameMode' */
FORCEINLINE static AFPS_GameMode* const GetFPSGameMode()
{
    return AFPS_GameMode::CurrentGameMode;
};
//--------------------------------------------------------------------------------------
