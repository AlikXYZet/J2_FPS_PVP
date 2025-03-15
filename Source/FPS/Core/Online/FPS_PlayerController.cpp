//

// Base:
#include "FPS_PlayerController.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFPS_PlayerController::AFPS_PlayerController()
{
    // Установка вызова функции Tick() в каждом кадре.
    PrimaryActorTick.bCanEverTick = true; // Принудительно
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void AFPS_PlayerController::BeginPlay()
{
    Super::BeginPlay();

}

void AFPS_PlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    SetMouseToCenter();
}
//--------------------------------------------------------------------------------------



/* ---   Mouse To Center   --- */

void AFPS_PlayerController::SetMouseToCenter()
{
    if (bMouseToCenter)
    {
        if (GetPawn())
        {
            if (!IsPaused())
            {
                if (GetMousePosition(MousePositionX, MousePositionY))
                {
                    GetViewportSize(SizeCenterX, SizeCenterY);

                    if (MousePositionX != int32(SizeCenterX / 2) || MousePositionY != int32(SizeCenterY / 2))
                    {
                        SetMouseLocation(SizeCenterX / 2, SizeCenterY / 2);
                    }
                }
            }
        }
        else
        {
            // Флаг: Прекратить контроль Мыши
            bMouseToCenter = false;
        }
    }
}
//--------------------------------------------------------------------------------------
