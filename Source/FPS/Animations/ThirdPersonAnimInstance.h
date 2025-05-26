//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "PersonAnimInstance.h"

// UE:
#include "Engine/DataTable.h"

// Generated:
#include "ThirdPersonAnimInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;
//--------------------------------------------------------------------------------------



/* ---   Structs   --- */

/* Структура Таблиц Данных:
Звуки в зависимости от Типа Поверхности
*/
USTRUCT(BlueprintType)
struct FSoundBySurfaceType : public FTableRowBase
{
    GENERATED_BODY()

    // Тип Поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<EPhysicalSurface> SurfaceType = EPhysicalSurface::SurfaceType_Default;

    // Звук, соответствующий поверхности
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* Sound = nullptr;
};
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UThirdPersonAnimInstance : public UPersonAnimInstance
{
    GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Native initialization override point */
    virtual void NativeInitializeAnimation() override;

    /** Executed when begin play is called on the owning component */
    virtual void NativeBeginPlay() override;
    //-------------------------------------------



protected:

    /* ---   Customized Base   --- */

    /** Пользовательский аналог UpdateAnimation() */
    virtual void CustomUpdateAnimation() override;
    //-------------------------------------------



public:

    /* ---   Movements   --- */

    // Текущее относительное Направление Перемещения
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Movements")
    float CurrentMovementDirection = 0;
    //-------------------------------------------



    /* ---   Sounds   --- */

    // Таблица Данных для Звуков Поверхностей при Шаге
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnim|Sounds|Step",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfaceType"))
    UDataTable* SurfaceSoundsWhenStep = nullptr;

    // TMap Данных для Звуков Поверхностей при Шаге
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Sounds|Step",
        meta = (ForceInlineRow))
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> MapOfSurfaceSoundsWhenStep;

    // Таблица Данных для Звуков Поверхностей при Падении
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnim|Sounds|Landing",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfaceType"))
    UDataTable* SurfaceSoundsWhenLanding = nullptr;

    // TMap Данных для Звуков Поверхностей при Падении
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Sounds|Landing",
        meta = (ForceInlineRow))
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> MapOfSurfaceSoundsWhenLanding;

    //

    /** Воспроизвести Звук согласно поверхности, на которой находиться игрок */
    UFUNCTION(BlueprintCallable,
        Category = "PersonAnim|Sounds")
    void PlaySurfaceSound(const TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*>& SoundsMap);

    /** Получить контейнер типа TMap из Таблицы Данных  */
    UFUNCTION(BlueprintPure,
        Category = "PersonAnim|Sounds")
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> GetMapForSound(const UDataTable* SoundTable);
    //-------------------------------------------



    /* ---   Activity   --- */

    // Текущий угол Вертикального Взора
    UPROPERTY(EditAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Activity")
    float CurrentVerticalViewingAngle = 0;
    //-------------------------------------------



private:

    /* ---   Sounds   --- */

    /** Инициализация Данных о Звуках */
    void SoundsInit();
    //-------------------------------------------
};
