// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryItemWidget.h"

#include "InventoryItemDrapDropOP.h"
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

	if (InventoryItem->GetStackCount() <= 1)
	{
		StackCountText->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	StackCountText->SetVisibility(ESlateVisibility::Visible);
	StackCountText->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
}

UTexture2D* UInventoryItemWidget::GetIconTexture() const
{
	if (InventoryItem && InventoryItem->IsValid() && InventoryItem->GetShopItem())
	{
		return InventoryItem->GetShopItem()->GetIcon();
	}
	return nullptr;
}

FInventoryItemHandle UInventoryItemWidget::GetInventoryItemHandle() const
{
	return !IsEmpty() ? InventoryItem->GetHandle() : FInventoryItemHandle::InvalidHandle();
}

void UInventoryItemWidget::StartCooldown(float CooldownDuration, float CooldownTimeRemaining)
{
}

void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (!IsEmpty() && DragDropOperationClass)
	{
		if (UInventoryItemDrapDropOP* DragDropOp = NewObject<UInventoryItemDrapDropOP>(this, DragDropOperationClass))
		{
			DragDropOp->SetDraggedItem(this);
			OutOperation = DragDropOp;
		}
	}
}

bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	if (UInventoryItemWidget* OtherItemWidget = InOperation ? Cast<UInventoryItemWidget>(InOperation->Payload) : nullptr)
	{
		// 确保拖放的物品不是自己，并且拖放的物品不是空的
		if (OtherItemWidget && OtherItemWidget != this && !OtherItemWidget->IsEmpty())
		{
			// 广播物品被放下的事件, 由监听该事件的对象（如物品栏组件）来处理物品交换或移动的逻辑
			OnInventoryItemDropped.Broadcast(this, OtherItemWidget);
			return true;
		}
	}
	
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryItemWidget::RightButtonClicked()
{
	if (!IsEmpty())
	{
		OnRightButtonClicked.Broadcast(GetInventoryItemHandle());
	}
}

void UInventoryItemWidget::LeftButtonClicked()
{
	if (!IsEmpty())
	{
		OnLeftButtonClicked.Broadcast(GetInventoryItemHandle());
	}
}
