// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "TeamSelectionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTeamSelectionWidget::SetSlotID(uint8 InSlotID)
{
	SlotID = InSlotID;
}

void UTeamSelectionWidget::UpdateSlotInfoText(const FString& PlayerNickName)
{
	InfoTextBlock->SetText(FText::FromString(PlayerNickName));
}

void UTeamSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SelectButton->OnClicked.AddDynamic(this, &UTeamSelectionWidget::OnSelectButtonClicked);
	
}

void UTeamSelectionWidget::OnSelectButtonClicked()
{
	OnSlotClicked.Broadcast(SlotID);
}
