// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"

class UInventoryItem;
class UInventoryComponent;
class UInventoryItemWidget;
class UWrapBox;
/**
 * 
 */
UCLASS()
class ARENAS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UWrapBox* ItemListWrapBox;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemWidget> InventoryItemWidgetClass;

	UPROPERTY()
	UInventoryComponent* OwnerInventoryComponent;

	UPROPERTY()
	TArray<UInventoryItemWidget*> InventoryItemsWidgets;

	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItemWidget*> PopulatedItemEntryWidgetsMap;		// 已填充的物品条目小部件映射表

	void ItemAdded(const UInventoryItem* NewInventoryItem);
	void ItemStackChanged(const FInventoryItemHandle& InInventoryItemHandle, int NewStackCount);

	UInventoryItemWidget* GetNextAvailableSlot() const;		// 获取下一个可用的物品槽

	void HandleItemDragDrop(UInventoryItemWidget* TargetItemWidget, UInventoryItemWidget* SourceItemWidget);
	
};
