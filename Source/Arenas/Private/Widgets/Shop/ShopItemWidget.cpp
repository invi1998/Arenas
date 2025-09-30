// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ShopItemWidget.h"

#include "Inventory/PA_ShopItem.h"

void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	ShopItem = Cast<UPA_ShopItem>(ListItemObject);
	if (!ShopItem) return;

	SetIcon(ShopItem->GetIcon());
	SetTooltipWidget(ShopItem);
}

void UShopItemWidget::RightButtonClicked()
{
	OnItemPurchaseIssued.Broadcast(GetShopItem());
}

void UShopItemWidget::LeftButtonClicked()
{
	OnShopItemSelected.Broadcast(this);
}
