// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryItemDrapDropOP.h"
#include "InventoryItemWidget.h"
#include "ItemWidget.h"

void UInventoryItemDrapDropOP::SetDraggedItem(class UInventoryItemWidget* InDraggedItemWidget)
{
	Payload = InDraggedItemWidget;
	if (DragVisualClass)
	{
		UItemWidget* DragItemWidgetVisual = CreateWidget<UItemWidget>(GetWorld(), DragVisualClass);
		if (DragItemWidgetVisual && InDraggedItemWidget)
		{
			DragItemWidgetVisual->SetIcon(InDraggedItemWidget->GetIconTexture());
			DefaultDragVisual = DragItemWidgetVisual;
		}
	}
}
