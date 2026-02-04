#pragma once

// Base:
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// Structs:
#include "FPS/Tools/Structs/Sessions/SessionData.h"

// Generated:
#include "SessionDataWidget.generated.h"
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API USessionDataWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Аналогичен OnInitialized() */
    virtual void NativeOnInitialized() override;

    /** Аналогичен Construct() */
    virtual void NativeConstruct() override;
    //--------------------------------------------



    /* ---   Session Data   --- */

    // Таблица Данных: Выбираемые Карты Арен
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Session Data|Arenas",
        meta = (RequiredAssetDataTags = "RowStructure=MapSelection"))
    UDataTable* SelectableArenaMaps = nullptr;

    //

    /* Получить данные выбираемых Карт Арен (из 'Selectable Arena Maps') */
    UFUNCTION(BlueprintPure,
        Category = "Session Data|Arenas")
    FMapSelection GetSelectableArenaMapsDataByIndex(int32 Index);
    //-------------------------------------------



private:

};
