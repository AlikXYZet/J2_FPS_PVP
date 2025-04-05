//

#pragma once

// Base:
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// UE:
#include "Engine/DataTable.h"

// Generated:
#include "WeaponFrame.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// UE:
class UDataTable;
class UArrowComponent;

// Interaction:
class UWeaponSlotsComponent;
class AProjectile;
class APlayerCharacter;
//--------------------------------------------------------------------------------------



/* ---   Structs   --- */

/* Структура данных Оружия:
визуализация, анимации и характеристики
*/
USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_BODY()


    /* ---   Visualization   --- */

    /* Скелетный Меш Оружия */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization",
        meta = (EditCondition = "StaticMesh==nullptr"))
    USkeletalMesh* SkeletalMesh = nullptr;

    // Статический Меш Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization",
        meta = (EditCondition = "SkeletalMesh==nullptr"))
    UStaticMesh* StaticMesh = nullptr;

    // Трансформация Меша Оружия
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform MeshTransform;

    // Трансформация Направляющей Выстрела
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform ShootGuidanceTransform;

    // Трансформация Направляющей вылета Гильзы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform CaseDropGuidanceTransform;

    // Трансформация Направляющей выпадения Накопителя
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Visualization")
    FTransform StorageDropGuidanceTransform;
    //-------------------------------------------


    /* ---   Animations   --- */

    /** От Бедра */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* FromHip = nullptr;

    // Прицеливание
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* Aiming = nullptr;

    // Перезарядка от Бедра
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* Reload = nullptr;

    // Перезарядка при Прицеливании
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Animations")
    UAnimMontage* ReloadWhenAiming = nullptr;
    //-------------------------------------------


    /* ---   Specifications   --- */

    /* Оружие заряжаемое? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    bool bIsLoadable = true;

    // Максимальное Количество подготовленных Патронов в чём-либо (в магазине, обойме и т.п.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Specifications")
    int32 MaxPreparedCartridges = 0;
    //-------------------------------------------


    /* ---   Constructors   --- */

    FWeaponData() {};
    //-------------------------------------------
};
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

    /* ---   Weapon | Editor   --- */

    /** Выгрузить Данные для выбранного Оружия из Таблицу Данных Оружий */
    UFUNCTION(CallInEditor,
        Category = "Editor")
    void LoadDataFromWeaponsDataTable();

    /** Сохранить текущие Данные в Таблицу Данных Оружий */
    UFUNCTION(CallInEditor,
        Category = "Editor")
    void SaveCurrentDataInWeaponsDataTable();
    //-------------------------------------------



    /* ---   Weapon | Data   --- */

    // Таблица данных местоположения фигур
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Frame | Data",
        meta = (RequiredAssetDataTags = "RowStructure=WeaponData"))
    UDataTable* WeaponsDataTable;

    // Текущее выбранное Оружие из Таблицы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Frame | Data",
        meta = (GetOptions = "GetRowNamesFromWeaponsDataTable"))
    FName SelectedWeapon = "NONE";

    // Данные выбранного Оружия из Таблицы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Frame | Data")
    FWeaponData SelectedWeaponData;

    //

    // Сохранить указатель на текущий Компонент оружейных Слотов
    // @note    Организует прямую связь "Слот" <=> "Оружие"
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Frame | Data")
    void SetCurrentWeaponSlotsComponent(UWeaponSlotsComponent* NewComponent);

    // Обновить Оружие по его Имени
    UFUNCTION(BlueprintCallable,
        Category = "Weapon Frame | Data")
    void UpdateWeaponByName(const FName& Name);
    //-------------------------------------------



    /* ---   Weapon | Control   --- */

    /** Управление Оружием: Стрельба */
    void Fire();

    /** Управление Оружием: Перезарядка */
    void Reload();

    /** Управление Оружием: Прицеливание */
    void Aiming();

    /** Управление Оружием: Прекратить прицеливание */
    void StopAiming();
    //-------------------------------------------



private:

    /* ---   Weapon | Data   --- */

    // Массив со слотами (данными) оружия
    UPROPERTY(VisibleInstanceOnly,
        Category = "Weapon Frame | Check")
    UWeaponSlotsComponent* CurrentWeaponSlotsComponent = nullptr;

    //

    /** Получение Названий строк из таблицы WeaponsDataTable */
    UFUNCTION()
    TArray<FName> GetRowNamesFromWeaponsDataTable() const;

    // Обновить Оружие по Данным
    void UpdateWeaponOnSelectedData();
    //-------------------------------------------



    /* ---   Weapon | Control   --- */

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
