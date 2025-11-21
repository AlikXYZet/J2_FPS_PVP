//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
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
    virtual void SetCustomMovementMode(ESpeedVariations Mode);
    //-------------------------------------------



    /* ---   Speed Control   --- */

    // Базовые скорости передвижения
    // 
    // PS: Массив НЕ динамический.
    // Количество элементов массива зависит от количества элементов в ESpeedVariations
    UPROPERTY(EditAnywhere,
        Category = "FPS Movement Component|Speed Control",
        meta = (ArraySizeEnum = "ESpeedVariations"))
    float BaseMovementSpeeds[ESpeedVariations::COUNT] = { 800, 600, 200, 0 };

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
    UFUNCTION(BlueprintCallable,
        Category = "FPS Movement Component|Speed Control",
        meta = (AutoCreateRefTerm = "Mode"))
    void SetSpeedType(ESpeedVariations Type);

    /** Изменить значение максимальной скорости */
    UFUNCTION(BlueprintCallable,
        Category = "FPS Movement Component|Speed Control",
        meta = (AutoCreateRefTerm = "Mode"))
    void SetMaxWalkSpeed(float Value);
    //-------------------------------------------



private:

    /* ---   Speed Control   --- */

    /* Массив "Контроллеров" Скорости
    @note   За эталон берётся максимальное значение ограничения */
    TArray<ESpeedVariations*> SpeedControllers;
    //-------------------------------------------



    /* ---   Speed Control | MaxWalkSpeed   --- */

    /** Server: Изменить значение максимальной скорости */
    UFUNCTION(Server, Reliable)
    void Server_SetMaxWalkSpeed(float Value);

    /** Multicast: Изменить значение максимальной скорости */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetMaxWalkSpeed(float Value);
    //-------------------------------------------
};
