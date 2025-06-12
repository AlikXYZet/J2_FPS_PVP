//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

// Structs:
#include "FPS/Tools/Structs/Movement/SpeedControlData.h"

// Generated:
#include "FPS_CharacterMovementComponent.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UFPS_CharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UFPS_CharacterMovementComponent();
    //-------------------------------------------



protected:

    /* ---   Movement Mode   --- */

    /** Called after MovementMode has changed. Base implementation does special handling for starting certain modes, then notifies the CharacterOwner. */
    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
    //-------------------------------------------



public:

    /* ---   Movement Mode   --- */

    UFUNCTION(BlueprintCallable,
        Category = "FPS Movement Component|Movement Mode",
        meta = (AutoCreateRefTerm = "Mode"))
    virtual void SetCustomMovementMode(const ESpeedVariations& Mode);
    //-------------------------------------------



    /* ---   Speed Control   --- */

    // Базовые скорости передвижения
    // 
    // PS: Массив НЕ динамический.
    // Количество элементов массива зависит от количества элементов в ESpeedVariations
    UPROPERTY(EditAnywhere,
        Category = "FPS Movement Component|Speed Control",
        meta = (ArraySizeEnum = "ESpeedVariations"))
    float BaseMovementSpeeds[ESpeedVariations::COUNT] = { 800, 600, 100, 0 };

    //

    /** Добавить контроль Скорости */
    void AddSpeedControl(ESpeedVariations& Controller);

    /** Удалить контроль Скорости */
    void RemoveSpeedControl(ESpeedVariations& Controller);

    /** Обновить ограничение максимальной скорости */
    void UpdateMaxSpeed();
    //-------------------------------------------



    /* ---   Speed Control | MaxWalkSpeed   --- */

    /** Изменить значение максимальной скорости */
    void SetSpeedType(const ESpeedVariations& Type);

    /** Обновить значение максимальной скорости через Сервер */
    UFUNCTION(Server, Reliable)
    void Server_SetSpeedType(const ESpeedVariations& Type);

    /** Обновить значение максимальной скорости у Всех, кроме владельца */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetSpeedType(const ESpeedVariations& Type);

    /** Изменить значение максимальной скорости */
    void SetMaxWalkSpeed(const float& Value);

    /** Обновить значение максимальной скорости через Сервер */
    UFUNCTION(Server, Reliable)
    void Server_SetMaxWalkSpeed(const float& Value);

    /** Обновить значение максимальной скорости у Всех, кроме владельца */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetMaxWalkSpeed(const float& Value);
    //-------------------------------------------



private:

    /* ---   Speed Control   --- */

    /* Массив "Контроллеров" Скорости
    @note   За эталон берётся максимальное значение ограничения */
    TArray<ESpeedVariations*> SpeedControllers;
    //-------------------------------------------
};
