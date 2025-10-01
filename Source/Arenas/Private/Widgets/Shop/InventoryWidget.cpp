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

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	for (UInventoryItemWidget* Widget : InventoryItemsWidgets)
	{
		if (Widget && Widget->EmptySlot())
		{
			return Widget;
		}
	}

	return nullptr;
}
