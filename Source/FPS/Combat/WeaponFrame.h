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
class UCameraComponent;

// Actor Components:
class USmoothMovementComponent;
class USmoothRotationComponent;

// Interaction:
class UWeaponControlComponent;
class AProjectile;
class APlayerCharacter;
class AFPS_PlayerController;
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
        Category = "Components|Meshes",
        meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* WeaponSkeletalMesh = nullptr;

    // Компонент статического Меша Оружия
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Meshes",
        meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* WeaponStaticMesh = nullptr;

    //

    // Направляющая Выстрела (красный)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Arrows",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* ShootGuidance = nullptr;

    // Направляющая вылета Гильзы (синий)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Arrows",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* CaseDropGuidance = nullptr;

    // Направляющая выпадения Накопителя (зелёный): Магазина, Обоймы и т.п.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Arrows",
        meta = (AllowPrivateAccess = "true"))
    UArrowComponent* StorageDropGuidance = nullptr;

    //

    // Компонент плавного Перемещения
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Controls",
        meta = (AllowPrivateAccess = "true"))
    USmoothMovementComponent* SmoothMovementComponent;

    // Компонент плавного Вращения
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Components|Controls",
        meta = (AllowPrivateAccess = "true"))
    USmoothRotationComponent* SmoothRotationComponent;
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /** Overridable function called whenever this actor is being removed from a level */
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //-------------------------------------------



public:

    /* ---   Base   --- */

    // Function called every frame on this Actor
    virtual void Tick(float DeltaSeconds) override;
    //-------------------------------------------



    /* ---   Data   --- */

    /** Обновить Оружие по Данным */
    void UpdateWeaponOnSelectedData(const FWeaponData* lData);
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Data   --- */

    /* Таблица данных местоположения фигур */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Frame|Data",
        meta = (RequiredAssetDataTags = "RowStructure=WeaponData"))
    UDataTable* WeaponsDataTable;

    // Текущее выбранное Оружие из Таблицы
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Weapon Frame|Data",
        meta = (GetOptions = "GetRowNamesFromWeaponsDataTable"))
    FName WeaponName = NAME_None;
    //-------------------------------------------



    /* ---   Editor   --- */

    /** Выгрузить Данные для выбранного Оружия из Таблицы Данных Оружий */
    UFUNCTION(CallInEditor,
        Category = "Weapon Frame|Editor")
    void LoadDataFromWeaponsDataTable();

    /** Сохранить текущие Данные в Таблицу Данных Оружий */
    UFUNCTION(CallInEditor,
        Category = "Weapon Frame|Editor")
    void SaveCurrentDataInWeaponsDataTable();
    //-------------------------------------------
#endif
    //===========================================



private:

    /* ---   Control   --- */

    // Игрок-Владелец данного Оружия
    //APlayerCharacter* ParentPlayerCharacter = nullptr;
    //-------------------------------------------



    /* ---   Direction Fire   --- */

    //// Контроллер Игрока-Владельца данного Оружия
    //AFPS_PlayerController* ParentPlayerController = nullptr;

    //// Флаг отслеживания состояния прицеливания
    //bool bIsAiming = false;

    ////

    ///** Повернуть в сторону Результата Трассировки */
    //void RotateToTraceResult();

    ///** Прикрепить данное Оружие к Руке */
    //void AttachWhenStoppedAiming();

    ///** Прикрепить данное Оружие к Камере */
    //void AttachWhenAiming();
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Data   --- */

    // Указатель на Данные выбранного Оружия из Таблицы
    FWeaponData* SelectedWeaponData;

    //

    /** Получение Названий строк из таблицы WeaponsDataTable */
    UFUNCTION()
    TArray<FName> GetRowNamesFromWeaponsDataTable() const;
    //-------------------------------------------
#endif
    //===========================================
};
