#pragma once

// Base:
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// Structs:
#include "FPS/Tools/Saving/Settings/SettingsData.h"

// Generated:
#include "SettingsWidget.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class USaveSettings;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    /* ---   Base   --- */

    /** Аналогичен OnInitialized() */
    virtual void NativeOnInitialized() override;

    /** Аналогичен Construct() */
    virtual void NativeConstruct() override;
    //--------------------------------------------



    /* ---   Settings System | Saving   --- */

    // Флаг контроля изменений Настроек Видео
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite,
        Category = "Settings System|Saving",
        meta = (DisplayName = "Is Video Settings Dirty?"))
    bool bIsVideoSettingsDirty = false;

    // Новые (изменённые) Настроеки
    UPROPERTY(BlueprintReadWrite,
        Category = "Settings System|Saving")
    FSettingsData NewSettingsData;

    //

    /** Получить частоту обновления Экрана монитора */
    UFUNCTION(BlueprintCallable,
        Category = "Settings System")
    const int32 GetDisplayFrequency() const;

    /** Выгрузить Настройки */
    UFUNCTION(BlueprintCallable,
        Category = "Settings System|Saving")
    void LoadSettings();

    /** Сохранение Настроек */
    UFUNCTION(BlueprintCallable,
        Category = "Settings System|Saving")
    void ApplySettings();

    /** Определить изменены ли настройки Настройки */
    UFUNCTION(BlueprintPure,
        Category = "Settings System|Saving",
        meta = (CompactNodeTitle = "Is Settings Dirty?"))
    bool IsSettingsDirty();
    //-------------------------------------------



    /* ---   Settings System | Sounds   --- */

    /** Сохранение Настроек */
    UFUNCTION(BlueprintCallable,
        Category = "Settings System|Saving")
    void UpdateSounds();
    //-------------------------------------------



private:

    /* ---   Settings System | Saving   --- */

    // Текущий GameUserSettings
    UGameUserSettings* GameUserSettings = nullptr;

    // Наименование слотов сохранения данных Настроек
    FString SettingsDataSlot = "SettingsData";

    // Сохраняемые данные Настроек
    USaveSettings* SaveSettings = nullptr;

    //

    // Инициализация данных
    void InitWidgetData();
    //-------------------------------------------
};
