// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "WaitWidget.h"

#include "Components/TextBlock.h"
#include "Widgets/Component/ArenasButton.h"

void UWaitWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FOnButtonClickedEvent& UWaitWidget::ClearAndGetButtonClickedEvent()
{
	CancelButton->ButtonArea->OnClicked.Clear();
	return CancelButton->ButtonArea->OnClicked;
}

void UWaitWidget::SetWaitInfo(const FText& InWaitInfo, bool bAllowedCancel)
{
	if (CancelButton)
	{
		CancelButton->SetVisibility(bAllowedCancel ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
	if (WaitInfoText)
	{
		WaitInfoText->SetText(InWaitInfo);
	}
}
