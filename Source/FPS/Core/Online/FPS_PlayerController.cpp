//

// Base:
#include "FPS_PlayerController.h"

// UE:
#include "GameFramework/HUD.h"
#include "GameFramework/InputSettings.h"

// Interaction:
#include "FPS/ActorComponents/Properties/InteractiveComponent.h"
#include "FPS/Core/Online/FPS_GameState.h"
//--------------------------------------------------------------------------------------



/* ---   Constructors   --- */

AFPS_PlayerController::AFPS_PlayerController()
{
    // Установка вызова функции Tick() в каждом кадре.
    PrimaryActorTick.bCanEverTick = true; // Принудительно
    //-------------------------------------------


    /* ---   Role Selection   --- */

    // Дистанция подбора предмета (трассировки наведения Мыши)
    HitResultTraceDistance = 280000.f;
    EnableMouseEvents(true);

    /** "Наблюдатель" по умолчанию
    @note   Заменяем `PC->StartSpectatingOnly();`, так как в данный момент:
    * `StateName == NAME_Spectating`;
    * `PlayerState == nullptr`. */
    bPlayerIsWaiting = false;
    //-------------------------------------------
}
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

//void AFPS_PlayerController::BeginPlay()
//{
//    Super::BeginPlay();
//
//}

void AFPS_PlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    SetMouseToCenter();
}
//--------------------------------------------------------------------------------------



/* ---   Mouse   --- */

bool AFPS_PlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
    bool bResult = false;
    if (PlayerInput)
    {
        bResult = PlayerInput->InputKey(Key, EventType, AmountDepressed, bGamepad);
        if (bEnableClickEvents && (ClickEventKeys.Contains(Key) || ClickEventKeys.Contains(EKeys::AnyKey)))
        {
            FVector2D MousePosition;
            UGameViewportClient* ViewportClient = CastChecked<ULocalPlayer>(Player)->ViewportClient;
            if (ViewportClient && ViewportClient->GetMousePosition(MousePosition))
            {
                if (!(GetHUD() && GetHUD()->UpdateAndDispatchHitBoxClickEvents(MousePosition, EventType)))
                {
                    UPrimitiveComponent* ClickedPrimitive = nullptr;
                    if (bEnableMouseOverEvents)
                    {
                        ClickedPrimitive = CurrentClickablePrimitive.Get();
                    }
                    else
                    {
                        FHitResult HitResult;
                        if (GetHitResultAtScreenPosition(MousePosition, CurrentClickTraceChannel, true, HitResult))
                        {
                            ClickedPrimitive = HitResult.Component.Get();
                        }
                    }

                    if (ClickedPrimitive)
                    {
                        switch (EventType)
                        {
                        case IE_Pressed:
                        case IE_DoubleClick:
                            ClickedPrimitive->DispatchOnClicked(Key);
                            break;

                        case IE_Released:
                            ClickedPrimitive->DispatchOnReleased(Key);
                            OnClickingOnComponent.Broadcast(ClickedPrimitive, Key);
                            break;

                        case IE_Axis:
                        case IE_Repeat:
                            break;
                        }
                    }
                }

                bResult = true;
            }
        }
    }

    return bResult;
}

FORCEINLINE void AFPS_PlayerController::SetMouseToCenter()
{
    // @note    'FORCEINLINE' действует в пределах данного '.cpp'
    if (bMouseToCenter)
    {
        if (GetPawn())
        {
            if (!IsPaused()
                && GetMousePosition(MousePositionX, MousePositionY))
            {
                GetViewportSize(SizeCenterX, SizeCenterY);

                if (MousePositionX != int32(SizeCenterX / 2) || MousePositionY != int32(SizeCenterY / 2))
                {
                    SetMouseLocation(SizeCenterX / 2, SizeCenterY / 2);
                }
            }
        }
        else
        {
            // Флаг: Прекратить контроль Мыши
            bMouseToCenter = false;
        }
    }
}
//--------------------------------------------------------------------------------------



/* ---   Action   --- */

TArray<FName> AFPS_PlayerController::GetActionsGroup(FKey Key)
{
    TArray<FName> ActualGroups;

    if (SelectedActionGroups.Num())
    {
        UInputSettings* InputSettings = UInputSettings::GetInputSettings();
        TArray<FInputActionKeyMapping> lArray;
        lArray.Reserve(4);

        // Пройтись по всем зарегистрированным группам
        for (FName& NameGroup : SelectedActionGroups)
        {
            UInputSettings::GetInputSettings()->GetActionMappingByName(NameGroup, lArray);

            // Поиск соответствующей клавиши
            if (lArray.FindByPredicate([Key](const FInputActionKeyMapping& Item) { return Item.Key == Key; }))
            {
                ActualGroups.Add(NameGroup);
            }

            lArray.Empty(4);
        }
    }

    return ActualGroups;
}

bool AFPS_PlayerController::ToInteract(UInteractiveComponent* InteractionComponent)
{
    if (InteractionComponent)
    {
        Server_ToInteract(InteractionComponent);
        return true;
    }
    return false;
}

void AFPS_PlayerController::Server_ToInteract_Implementation(UInteractiveComponent* InteractionComponent)
{
    if (IsValid(InteractionComponent))
    {
        InteractionComponent->ReactionInteractionOnServer(GetCharacter());
    }
}
//--------------------------------------------------------------------------------------



/* ---   Match Management   --- */

const AFPS_GameState* AFPS_PlayerController::BP_GetFPSGameState()
{
    return GetFPSGameState();
}

bool AFPS_PlayerController::IsMatchInProgress() const
{
    return GetFPSGameState()->IsMatchInProgress();
}
//--------------------------------------------------------------------------------------



/* ---   Role Selection   --- */

void AFPS_PlayerController::Server_GoToSpectators_Implementation() const
{
    GetFPSGameState()->ClientGoToSpectators(PlayerState);
}

void AFPS_PlayerController::Server_GoToPlayers_Implementation() const
{
    GetFPSGameState()->ClientGoToPlayers(PlayerState);
}

void AFPS_PlayerController::Server_SetMatchReadiness_Implementation(bool bReadiness) const
{
    GetFPSGameState()->SetPlayerReadiness(PlayerState, bReadiness);
}

void AFPS_PlayerController::Client_ChangedSelectedRole_Implementation(bool bIsPlayer) const
{
    OnSelectedRoleChange.Broadcast(bIsPlayer);

    // Если роль сменилась на "Наблюдатель", то уведомить о смене готовности игрока как "false"
    if (!bIsPlayer)
    {
        OnPlayerReadinessChange.Broadcast(false);
    }
}

void AFPS_PlayerController::Client_ChangedMatchReadiness_Implementation(bool bReadiness) const
{
    OnPlayerReadinessChange.Broadcast(bReadiness);
}
//--------------------------------------------------------------------------------------



/* ===   For EDITOR only   === */

#if WITH_EDITOR

/* ---   Debugs   --- */

void AFPS_PlayerController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // Здесь можно написать логику проверки изменённого свойства.
    FName PropertyName = (PropertyChangedEvent.Property != nullptr)
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(AFPS_PlayerController, SelectedActionGroups))
    {
        InitActionGroup();
    }
};
//--------------------------------------------------------------------------------------



/* ---   Actions   --- */

void AFPS_PlayerController::InitActionGroup()
{
    ClickEventKeys.Empty();

    if (SelectedActionGroups.Num())
    {
        UInputSettings* InputSettings = UInputSettings::GetInputSettings();
        TArray<FInputActionKeyMapping> lArray;

        for (FName& NameGroup : SelectedActionGroups)
        {
            InputSettings->GetActionMappingByName(NameGroup, lArray);

            for (FInputActionKeyMapping& Data : lArray)
            {
                ClickEventKeys.AddUnique(Data.Key);
            }
        }
    }
}

TArray<FName> AFPS_PlayerController::GetActionNames()
{
    UInputSettings* InputSettings = UInputSettings::GetInputSettings();

    TArray<FName> ActionNames;

    InputSettings->GetActionNames(ActionNames);

    return ActionNames;
}
//--------------------------------------------------------------------------------------
#endif
//======================================================================================
