// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryItemWidget.h"

#include "ItemToolTip.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/PA_ShopItem.h"

void UInventoryItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
	return !InventoryItem || !(InventoryItem->IsValid());
}

void UInventoryItemWidget::EmptySlot()
{
	InventoryItem = nullptr;
	SetIcon(EmptyIcon);
	SetToolTip(nullptr);

	StackCountText->SetVisibility(ESlateVisibility::Hidden);
	CooldownText->SetVisibility(ESlateVisibility::Hidden);
	CooldownDurabilityText->SetVisibility(ESlateVisibility::Hidden);
	ManaCostText->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::SetInventoryItem(const UInventoryItem* NewInventoryItem)
{
	InventoryItem = NewInventoryItem;
	if (!InventoryItem || !(InventoryItem->IsValid()) || InventoryItem->GetStackCount() <= 0)
	{
		EmptySlot();
		return;
	}

	SetIcon(InventoryItem->GetShopItem()->GetIcon());
	if (UItemToolTip* ItemToolTipWidget = SetTooltipWidget(InventoryItem->GetShopItem()))
	{
		ItemToolTipWidget->SetPrice(InventoryItem->GetShopItem()->GetSellPrice());
	}

	if (InventoryItem->GetShopItem()->IsStackable())
	{
		// 如果物品是可堆叠的，则显示堆叠数量
		StackCountText->SetVisibility(ESlateVisibility::Visible);
		UpdateStackCountText();
	}
	else
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryItemWidget::UpdateStackCountText()
{
	if (!InventoryItem || !(InventoryItem->IsValid())) return;
	StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
}
