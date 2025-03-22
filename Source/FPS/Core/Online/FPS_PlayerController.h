//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

// Generated:
#include "FPS_PlayerController.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AFPS_PlayerController : public APlayerController
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this actor's properties
    AFPS_PlayerController();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    /** Функция, вызываемая каждый кадр в этом Акторе */
    virtual void Tick(float DeltaSeconds) override;
    //-------------------------------------------



    /* ---   Mouse To Center   --- */

    // Флаг контроля Мыши в центре Экрана
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Settings System|Saving",
        meta = (DisplayName = "Hold Mouse in Center?"))
    bool bMouseToCenter = true;
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
};
