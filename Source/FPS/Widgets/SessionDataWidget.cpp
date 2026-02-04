//

// Base:
#include "SessionDataWidget.h"
//--------------------------------------------------------------------------------------



/* ---   Base   --- */

void USessionDataWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void USessionDataWidget::NativeConstruct()
{
    Super::NativeConstruct();
}
//--------------------------------------------------------------------------------------



/* ---   Session Data   --- */

FMapSelection USessionDataWidget::GetSelectableArenaMapsDataByIndex(int32 Index)
{
    if (SelectableArenaMaps)
    {
        TArray<FMapSelection*> lAllRows;

        SelectableArenaMaps->GetAllRows<FMapSelection>(__FUNCTION__, lAllRows);

        if (lAllRows.IsValidIndex(Index))
        {
            return *lAllRows[Index];
        }
    }

    return FMapSelection();
}
//--------------------------------------------------------------------------------------
