//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "PersonAnimInstance.h"

// Structs:
#include "FPS/Tools/Structs/Sounds/SoundBySurfacesData.h"

// Generated:
#include "ThirdPersonAnimInstance.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API UThirdPersonAnimInstance : public UPersonAnimInstance
{
	GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Native initialization override point */
    virtual void NativeInitializeAnimation() override;

    /** Native Uninitialize override point */
    virtual void NativeUninitializeAnimation() override;

    /** Executed when begin play is called on the owning component */
    virtual void NativeBeginPlay() override;
    //-------------------------------------------

public:

    /* ---   Sounds   --- */

    // Таблица Данных для Звуков Поверхностей при Шаге
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnim|Sounds|Step",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfacesType"))
    UDataTable* SurfaceSoundsWhenStep = nullptr;

    // TMap Данных для Звуков Поверхностей при Шаге
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "PersonAnim|Sounds|Step",
        meta = (ForceInlineRow))
    TMap<TEnumAsByte<EPhysicalSurface>, USoundBase*> MapOfSurfaceSoundsWhenStep;

    // Таблица Данных для Звуков Поверхностей при Падении
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "PersonAnim|Sounds|Landing",
        meta = (RequiredAssetDataTags = "RowStructure=SoundBySurfacesType"))
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



private:

    /* ---   Sounds   --- */

    /** Инициализация Данных о Звуках */
    void InitSounds();
    //-------------------------------------------
};
