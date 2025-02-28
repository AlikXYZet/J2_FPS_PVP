//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

// Generated:
#include "FPS_GameInstance.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UFPS_GameInstance : public UGameInstance
{
    GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Инициализация данных GameInstance */
    virtual void Init() override;
    //-------------------------------------------
};
