//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Components/ActorComponent.h"

// Generated:
#include "InteractiveComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При срабатывании действия на Актор
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOwnerWasClicked, const FKey&, ButtonReleased);
// ----------------------------------------------------------------------------------------------------



/* ---   Structs   --- */

USTRUCT(BlueprintType)
struct FComponentRendering
{
    GENERATED_BODY()

    // Используемый Компонент
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UPrimitiveComponent* Component = nullptr;

    // Используемый Компонент
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    uint8 DepthStencilValue = 0;
    //-------------------------------------------


    /* ---   Operators | ==   --- */

    FORCEINLINE bool operator==(const FComponentRendering& Second) const
    {
        return Component == Second.Component
            && DepthStencilValue == Second.DepthStencilValue;
    }
    //-------------------------------------------
};
//--------------------------------------------------------------------------------------



// Компонент интерактивности для подсвечивания Актора через изменение трафаретного буфера (Stencil Buffer) рендеринга
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FPS_API UInteractiveComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    /* ---   Delegates   --- */

    // Делегат: При срабатывании действия на Актор
    UPROPERTY(BlueprintAssignable)
    FOnOwnerWasClicked OnOwnerWasClicked;
    //-------------------------------------------



    /* ---   Constructors   --- */

    // Sets default values for this component's properties
    UInteractiveComponent();
    //-------------------------------------------



protected:

    /* ---   Base   --- */

    // Called when the game starts
    virtual void BeginPlay() override;
    //-------------------------------------------



public:

    /* ---   Highlighting   --- */

    // Имена используемых Компонентов, которые требуется выделять
    // @warning  Присваивание значений следует проводить в BeginPlay() до вызова Super::BeginPlay()
    //           Иначе: Повторно вызывать InitializationUsedComponents()
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Interactive|Highlighting",
        meta = (TitleProperty = "Component"))
    TArray<FComponentRendering> UsedComponents;

    /* Имена Функций-Предикатов, которые используются для критерия выделения
    *
    * @warning  Только const/pure функции с возвращаемым bool или uin8 (Byte)
    *           Все функции через логическое &&
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Interactive|Highlighting",
        meta = (GetOptions = "GetNamesOfPredicateFunctions"))
    TArray<FName> PredicateFunctionNames;

    //

    /* Добавить Наименование Предиката с проверкой его Типа */
    UFUNCTION(BlueprintCallable,
        Category = "Interactive|Highlighting")
    bool AddNamePredicate(const FName& NameFunction);

    /* Предварительная инициализация используемых Компонентов */
    UFUNCTION(BlueprintCallable,
        Category = "Interactive|Highlighting")
    void InitUsedComponents();
    //-------------------------------------------



    /* ---   Action   --- */

    // Выбранные группы Действий
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Interactive|Action",
        meta = (GetOptions = "GetActionGroupsNames"))
    FName SelectedActionGroups;
    //-------------------------------------------



private:

    /* ---   Highlighting   --- */

    // Используемые Компоненты, которые требуется выделять
    TArray<UPrimitiveComponent*> ComponentsUsed;

    //

    /* Событие выделения Актора при наведении */
    UFUNCTION()
    void CursorWasBeginOverOwner(AActor* TouchedActor);

    /* Событие прекращения выделения Актора при прекращении наведения */
    UFUNCTION()
    void CursorWasEndFromOwner(AActor* TouchedActor);

    /* Проверка функции в данном Акторе
    @return 0 == OK */
    uint8 CheckFunction(UObject* Owner, UFunction* Function);
    //-------------------------------------------



    /* ---   Actions   --- */

    // Контейнер с быстрым поиском Клавиши
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly,
        Category = "Interactive|Action|Check",
        meta = (AllowPrivateAccess = true))
    TSet<FKey> ActionKeys;

    //

    /* Предварительная инициализация используемых Клавиш */
    void InitActionGroup();

    /* Событие нажатия на Актора */
    UFUNCTION()
    void OwnerWasClicked(AActor* TouchedActor, FKey ButtonReleased);
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

    /* ---   Base: Debugs   --- */

    /** Вызывается, когда какое-либо свойство этого объекта было изменено
    * @note Используется для проверки изменённых переменных
    */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    //-------------------------------------------



    /* ---   Highlighting   --- */

    /* Получить имена всех Компонентов, что способны к выделению */
    UFUNCTION()
    TArray<FName> GetNamesOfHighlightedComponents();

    /* Получить имена всех Функций-Предикатов Актора-Владельца */
    UFUNCTION()
    TArray<FName> GetNamesOfPredicateFunctions();
    //-------------------------------------------



    /* ---   Actions   --- */

    /* Получить имена всех Функций-Предикатов Актора-Владельца */
    UFUNCTION()
    TArray<FName> GetActionGroupsNames();

    /* Повторная инициализация используемых Клавиш */
    void ReinitActionGroup();
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};
