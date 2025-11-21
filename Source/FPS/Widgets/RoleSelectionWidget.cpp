// 

// Base:
#include "RoleSelectionWidget.h"

// Global:
#include "FPS/Tools/GlobalMacros.h"

// UE:
#include "GameFramework/PlayerState.h"

// Interaction:
#include "FPS/Core/Online/FPS_GameState.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void URoleSelectionWidget::NativeConstruct()
{
    if (!AFPS_GameState::IsValidStaticPointer())
    {
        FPS_LOG(Error, TEXT("Current 'Game State' is NOT 'AFPS_GameState'"));
    }

    Super::NativeConstruct();

    InitRoleSelection();
}

void URoleSelectionWidget::SetVisibility(ESlateVisibility InVisibility)
{
    Super::SetVisibility(InVisibility);

    /** ПереИнициализация данных Статистики при изменении Видимости
    "Is NOT Visible?"
    @note   Метод 'IsVisible()' более тяжёлый */
    if (InVisibility != ESlateVisibility::Collapsed
        && InVisibility != ESlateVisibility::Hidden)
    {
        AFPS_GameState::CurrentGameState->OnEndSortingSpectators.AddDynamic(this, &URoleSelectionWidget::Event_OnEndSortingOfSpectators);
        AFPS_GameState::CurrentGameState->ReSortSpectatorsData();
    }
    else
    {
        AFPS_GameState::CurrentGameState->OnEndSortingSpectators.RemoveDynamic(this, &URoleSelectionWidget::Event_OnEndSortingOfSpectators);
    }
}
//--------------------------------------------------------------------------------------



/* ---   Spectators Data   --- */

void URoleSelectionWidget::SetDataSortingTypeForSortedSpectators(EPlayerStatisticsSortingType InType)
{
    AFPS_GameState::CurrentGameState->SetDataSortingTypeForSortedSpectators(InType);
}

const APlayerState* URoleSelectionWidget::GetDataByIndexFromSortedSpectators(int32 Index) const
{
    return AFPS_GameState::CurrentGameState->GetDataByIndexFromSortedSpectators(Index);
}

const TArray<APlayerState*>& URoleSelectionWidget::GetSortedSpectators() const
{
    return AFPS_GameState::CurrentGameState->GetSortedSpectators();
}
//--------------------------------------------------------------------------------------



/* ---   Role Selection   --- */

void URoleSelectionWidget::SetOwnerReadiness(bool bReadiness)
{
    AFPS_GameState::CurrentGameState->SetClientReadiness(bReadiness);
}

void URoleSelectionWidget::InitRoleSelection()
{
    AFPS_GameState::CurrentGameState->OnClientReadinessChange.AddDynamic(this, &URoleSelectionWidget::Event_OnOwnerReadinessChange);
}
//--------------------------------------------------------------------------------------
