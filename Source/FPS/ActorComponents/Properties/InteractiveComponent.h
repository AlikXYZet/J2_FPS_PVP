//

#pragma once

// Core:
#include "CoreMinimal.h"

// Base:
#include "Components/ActorComponent.h"

// Generated:
#include "InteractiveComponent.generated.h"
//--------------------------------------------------------------------------------------



/* ---   Pre-declaration of classes   --- */

// Interaction:
class APlayerCharacter;
//--------------------------------------------------------------------------------------



/* ---   Delegates   --- */

// Делегат: При Локальном срабатывании действия на Актор
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOwnerWasClicked, const FKey&, ButtonReleased);

// Делегат: При реакции Сервера на действия на Актор
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReactionToActions, ACharacter*, Instigator);
//--------------------------------------------------------------------------------------



/* ---   Structs   --- */

USTRUCT(BlueprintType)
struct FComponentRendering
{
    GENERATED_BODY()

    // Имена используемого Компонента
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        meta = (GetOptions = "GetNamesOfHighlightedComponents"))
    FName ComponentName;

    // Используемый Компонент
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UPrimitiveComponent* Component = nullptr;

    // Значение глубины Выделения
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

    /* Делегат: При Локальном срабатывании действия на Актор */
    UPROPERTY(BlueprintAssignable)
    FOnOwnerWasClicked OnOwnerWasClicked;

    /* Делегат: При реакции Сервера на действия на Актор */
    UPROPERTY(BlueprintAssignable)
    FOnReactionToActions OnServerReactionToActions;

    /* Делегат: При реакции Клиента на действия на Актор */
    UPROPERTY(BlueprintAssignable)
    FOnReactionToActions OnClientReactionToActions;
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

    /** Имена используемых Компонентов, которые требуют взаимодействие
    @warning    Присваивание значений следует проводить в BeginPlay() до вызова Super::BeginPlay()
                Иначе: Повторно вызывать InitUsedComponents() */
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Interactive|Highlighting",
        meta = (TitleProperty = "Component"))
    TArray<FComponentRendering> UsedComponents;

    /* Имена Функций-Предикатов, которые используются для критерия выделения
    @warning    Только const/pure функции с возвращаемым bool или uin8 (Byte)
                Все функции через логическое '&&' */
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



    /* ---   Actions   --- */

    // Выбранные группы Действий
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Interactive|Action",
        meta = (GetOptions = "GetActionGroupsNames"))
    FName SelectedActionGroups;

    //

    /* Событие нажатия на Актора */
    UFUNCTION()
    void ReactionActionsOnServer(ACharacter* Instigator);
    //-------------------------------------------



private:

    /* ---   Highlighting   --- */

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

    /* Multicast: Событие нажатия на Актора */
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_OwnerWasClicked(ACharacter* Instigator);
    //-------------------------------------------



    /* ===   For EDITOR only   === */

#if WITH_EDITOR

public:

    /* ---   Base: Debugs   --- */

    /** Вызывается, когда какое-либо свойство этого объекта было изменено
    @note   Используется для проверки изменённых переменных */
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /** Альтернативная версия PostEditChange, вызываемая при изменении свойств внутри структур */
    //virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    //-------------------------------------------



private:

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
    void ReInitActionGroup();
    //-------------------------------------------

#endif // WITH_EDITOR
    //===========================================
};
