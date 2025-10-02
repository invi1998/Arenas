// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryWidget.h"

#include "InventoryItemWidget.h"
#include "Components/WrapBox.h"
#include "Inventory/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (OwnerInventoryComponent)
		{
			// 订阅库存组件的物品添加事件
			OwnerInventoryComponent->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded);
			// 订阅库存物品堆叠数量变更事件
			OwnerInventoryComponent->OnItemStackCountChanged.AddUObject(this, &UInventoryWidget::ItemStackChanged);
			int Capacity = OwnerInventoryComponent->GetCapacity();
			ItemListWrapBox->ClearChildren();
			InventoryItemsWidgets.Empty();
			for (int i = 0; i < Capacity; ++i)
			{
				if (UInventoryItemWidget* NewItemWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), InventoryItemWidgetClass))
				{
					NewItemWidget->SetSlotNumber(i);
					UWrapBoxSlot* NewSlot = ItemListWrapBox->AddChildToWrapBox(NewItemWidget);
					InventoryItemsWidgets.Add(NewItemWidget);

					// 订阅物品拖放事件
					NewItemWidget->OnInventoryItemDropped.AddUObject(this, &UInventoryWidget::HandleItemDragDrop);
				}
			}
		}
	}
	
}

void UInventoryWidget::ItemAdded(const UInventoryItem* NewInventoryItem)
{
	if (!NewInventoryItem) return;

	if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot())
	{
		NextAvailableSlot->SetInventoryItem(NewInventoryItem);
		PopulatedItemEntryWidgetsMap.Add(NewInventoryItem->GetHandle(), NextAvailableSlot);
		if (OwnerInventoryComponent)
		{
			OwnerInventoryComponent->ItemSlotChanged(NewInventoryItem->GetHandle(), NextAvailableSlot->GetSlotNumber());
		}
	}
}

void UInventoryWidget::ItemStackChanged(const FInventoryItemHandle& InInventoryItemHandle, int NewStackCount)
{
	if (UInventoryItemWidget* InInventoryItemWidget = PopulatedItemEntryWidgetsMap.FindRef(InInventoryItemHandle))
	{
		InInventoryItemWidget->UpdateStackCountText();
	}
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	for (UInventoryItemWidget* Widget : InventoryItemsWidgets)
	{
		if (Widget && Widget->IsEmpty())
		{
			return Widget;
		}
	}

	return nullptr;
}

void UInventoryWidget::HandleItemDragDrop(UInventoryItemWidget* TargetItemWidget, UInventoryItemWidget* SourceItemWidget)
{
	const UInventoryItem* SrcInventoryItem = SourceItemWidget->GetInventoryItem();
	const UInventoryItem* DstInventoryItem = TargetItemWidget->GetInventoryItem();

	TargetItemWidget->SetInventoryItem(SrcInventoryItem);
	SourceItemWidget->SetInventoryItem(DstInventoryItem);

	PopulatedItemEntryWidgetsMap[TargetItemWidget->GetInventoryItem()->GetHandle()] = TargetItemWidget;
	if (OwnerInventoryComponent)
	{
		OwnerInventoryComponent->ItemSlotChanged(TargetItemWidget->GetInventoryItem()->GetHandle(), TargetItemWidget->GetSlotNumber());
	}

	if (!SourceItemWidget->IsEmpty())
	{
		PopulatedItemEntryWidgetsMap[SourceItemWidget->GetInventoryItem()->GetHandle()] = SourceItemWidget;
		if (OwnerInventoryComponent)
		{
			OwnerInventoryComponent->ItemSlotChanged(SourceItemWidget->GetInventoryItem()->GetHandle(), SourceItemWidget->GetSlotNumber());
		}
	}
	
}
