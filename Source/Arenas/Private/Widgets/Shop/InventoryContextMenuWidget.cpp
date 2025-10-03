// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryContextMenuWidget.h"

#include "Widgets/Component/ArenasButton.h"

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetSellButtonClickedEvent() const
{
	return SellButton->ButtonArea->OnClicked;
}

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetUseButtonClickedEvent() const
{
	return UseButton->ButtonArea->OnClicked;
}

FOnButtonClickedEvent& UInventoryContextMenuWidget::GetShowInShopButtonClickedEvent() const
{
	return ShowInShopButton->ButtonArea->OnClicked;
}
