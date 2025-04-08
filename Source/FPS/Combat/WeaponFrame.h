//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// Structs:
#include "FPS/Tools/Structs/Arsenal/WeaponData.h"
#include "FPS/Tools/Structs/Arsenal/WeaponSlotData.h"

// Generated:
#include "WeaponFrame.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UArrowComponent;

// Interaction:
class UWeaponSlotsComponent;
class AProjectile;
class APlayerCharacter;
//--------------------------------------------------------------------------------------



UCLASS()
class FPS_API AWeaponFrame : public AActor
{
    GENERATED_BODY()

public:

    /* ---   Constructors   --- */

    // Sets default values for this character's properties
    AWeaponFrame();
    //-------------------------------------------



    /* ---   Components   --- */

    // Компонент скелетного Меша Оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponSkeletalMesh = nullptr;

    // Компонент статического Меша Оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* WeaponStaticMesh = nullptr;

    // Направляющая Выстрела (красный)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* ShootGuidance = nullptr;

    // Направляющая вылета Гильзы (синий)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* CaseDropGuidance = nullptr;

    // Направляющая выпадения Накопителя (зелёный): Магазина, Обоймы и т.п.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* StorageDropGuidance = nullptr;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /** Overridable function called whenever this actor is being removed from a level */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //-------------------------------------------



public:

    /* ---   Weapon Frame | Editor   --- */

    /** Выгрузить Данные для выбранного Оружия из Таблицу Данных Оружий */
    UFUNCTION(CallInEditor,
        Category = "Editor")
    void LoadDataFromWeaponsDataTable();

    /** Сохранить текущие Данные в Таблицу Данных Оружий */
    UFUNCTION(CallInEditor,
        Category = "Editor")
    void SaveCurrentDataInWeaponsDataTable();
    //-------------------------------------------



    /* ---   Weapon Frame | Data   --- */

    /* Таблица данных местоположения фигур */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Frame | Data",
        meta = (RequiredAssetDataTags = "RowStructure=WeaponData"))
    UDataTable* WeaponsDataTable;

    // Текущее выбранное Оружие из Таблицы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Frame | Data",
        meta = (GetOptions = "GetRowNamesFromWeaponsDataTable"))
    FName SelectedWeapon = "NONE";

    //

    /** Сохранить Указатель на Указатель текущего Слота
    * @note    Организует прямую связь "текущий Слот" <=> "Оружие" */
    void SetPtrToPtrCurrentWeaponSlots(FWeaponSlotData** PtrToPtr);

    /** Обновить Оружие по его Имени */
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Frame | Data")
    void UpdateWeaponByName(const FName& Name);
    //-------------------------------------------



    /* ---   Weapon Frame | Control   --- */

    /** Управление Оружием: Стрельба */
    void Fire();

    /** Управление Оружием: Перезарядка */
    void Reload();

    /** Управление Оружием: Прицеливание */
    void Aiming();

    /** Управление Оружием: Прекратить прицеливание */
    void StopAiming();
    //-------------------------------------------



    /* ---   Weapon Frame | Direction of Fire   --- */


    //-------------------------------------------



private:

    /* ---   Weapon Frame | Data   --- */

    // Указатель на Указатель текущего слота
    FWeaponSlotData** PtrToPtrCurrentWeaponSlots;

    // Указатель на Данные выбранного Оружия из Таблицы
    FWeaponData* SelectedWeaponData;

    //

    /** Получение Названий строк из таблицы WeaponsDataTable */
    UFUNCTION()
    TArray<FName> GetRowNamesFromWeaponsDataTable() const;

    // Обновить Оружие по Данным
    void UpdateWeaponOnSelectedData();
    //-------------------------------------------



    /* ---   Weapon Frame | Control   --- */

    // Игрок-Владелец для данного Оружия
    APlayerCharacter* ParentPlayerCharacter = nullptr;

    //

    /** Предварительная инициализация Данных */
    void WeaponFrameInit();

    /** Настроить управление выбора Слота */
    void SetupPlayerInputs();

    /** Убрать управление выбора Слота */
    void RemovePlayerInputs();

    /** Запустить Анимацию Персонажа с проверкой */
    void PlayCharacterAnim(UAnimMontage* AnimMontage);
    //-------------------------------------------
};
