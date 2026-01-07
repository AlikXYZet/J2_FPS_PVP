//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "GameFramework/PlayerController.h"

// Generated:
#include "FPS_PlayerController.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При изменении выбранной Роли
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedRoleChange, bool, bIsPlayer);

// Делегат: При изменении Готовности текущего (локального) Игрока
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerReadinessChange, bool, bReadiness);
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class AFPS_GameState;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AFPS_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    // Делегат: При изменении выбранной Роли
    UPROPERTY(BlueprintAssignable)
    FOnSelectedRoleChange OnSelectedRoleChange;

    // Делегат: При изменении Готовности текущего (локального) Игрока
    UPROPERTY(BlueprintAssignable)
    FOnPlayerReadinessChange OnPlayerReadinessChange;
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this actor's properties
    AFPS_PlayerController();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Вызывается при Запуске игры или при Спавне в уже запущенной игре
    //virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Функция, вызываемая каждый кадр в этом Акторе, если не назначена другая частота */
    virtual void Tick(float DeltaSeconds) override;
    //-------------------------------------------



    /* ---   Mouse To Center   --- */

    // Флаг контроля Мыши в центре Экрана
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "FPS Player Controller|Mouse To Center",
        meta = (DisplayName = "Hold Mouse in Center?"))
    bool bMouseToCenter = true;
    //-------------------------------------------



    /* ---   Match Management   --- */

    /** Получить текущий 'Game State', приведённый к типу "AFPS_GameState" */
    UFUNCTION(BlueprintPure,
        Category = "FPS Player Controller|Match Management",
        meta = (DisplayName = "Get FPS Game State",
            DefaultToSelf, HideSelfPin = "true"))
    const AFPS_GameState* BP_GetFPSGameState();

    /** Возвращает значение true, если состояние матча находится "В Процессе" ('InProgress') */
    UFUNCTION(BlueprintPure,
        Category = "FPS Player Controller|Match Management",
        meta = (DefaultToSelf, HideSelfPin = "true"))
    bool IsMatchInProgress() const;
    //-------------------------------------------



    /* ---   Role Selection   --- */

    /** Перейти к Наблюдателям */
    UFUNCTION(BlueprintCallable,
        Category = "Role Selection",
        meta = (DefaultToSelf, HideSelfPin = "true"))
    void GoToSpectators() const
    {
        Server_GoToSpectators();
    };

    /** Перейти к Игрокам */
    UFUNCTION(BlueprintCallable,
        Category = "Role Selection",
        meta = (DefaultToSelf, HideSelfPin = "true"))
    void GoToPlayers() const
    {
        Server_GoToPlayers();
    };

    /** Изменить Готовность к Матчу */
    UFUNCTION(BlueprintCallable,
        Category = "Role Selection",
        meta = (DefaultToSelf, HideSelfPin = "true"))
    void SetMatchReadiness(bool bReadiness = false) const
    {
        Server_SetMatchReadiness(bReadiness);
    };

    /** Client: Изменилась выбранная Роль */
    UFUNCTION(Client, Reliable)
    void Client_ChangedSelectedRole(bool bIsPlayer = false) const;

    /** Client: Изменилась Готовность к Матчу */
    UFUNCTION(Client, Reliable)
    void Client_ChangedMatchReadiness(bool bReadiness = false) const;
    //-------------------------------------------



private:

    /* ---   Mouse To Center   --- */

    // Текущая позиция мыши по оси X
    float MousePositionX = 0;

    // Текущая позиция мыши по оси Y
    float MousePositionY = 0;

    // Текущий центр экрана по оси X
    int32 SizeCenterX = 0;

    // Текущий центр экрана по оси Y
    int32 SizeCenterY = 0;

    //

    /** Установить Мышь в центр экрана */
    void SetMouseToCenter();
    //-------------------------------------------



    /* ---   Role Selection   --- */

    /** @note:
    Действия данных RPC-методов исполняем в экземпляре 'AFPS_PlayerController',
    а не в 'AFPS_GameState' для уменьшения количества передаваемых по сети данных */

    /** Server: Перейти к Наблюдателям */
    UFUNCTION(Server, Reliable)
    void Server_GoToSpectators() const;

    /** Server: Перейти к Игрокам */
    UFUNCTION(Server, Reliable)
    void Server_GoToPlayers() const;

    /** Server: Изменить Готовность к Матчу */
    UFUNCTION(Server, Reliable)
    void Server_SetMatchReadiness(bool bReadiness) const;
    //-------------------------------------------
};
