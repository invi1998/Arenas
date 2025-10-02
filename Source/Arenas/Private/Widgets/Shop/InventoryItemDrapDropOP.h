// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryItemDrapDropOP.generated.h"

class UItemWidget;
/**
 * 库存物品拖放操作
 */
UCLASS()
class ARENAS_API UInventoryItemDrapDropOP : public UDragDropOperation
{
	GENERATED_BODY()

public:
	void SetDraggedItem(class UInventoryItemWidget* InDraggedItemWidget);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UItemWidget> DragVisualClass;		// 拖动时的视觉类

	
	
	
};
