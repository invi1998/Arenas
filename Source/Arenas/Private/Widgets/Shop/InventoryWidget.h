// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"

class UInventoryContextMenuWidget;
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
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryContextMenuWidget> InventoryContextMenuWidgetClass;	// 物品右键菜单类

	UPROPERTY()
	UInventoryContextMenuWidget* InventoryContextMenuWidget;	// 物品右键菜单实例

	UFUNCTION()
	void SellButtonClicked();
	
	UFUNCTION()
	void UseButtonClicked();

	UFUNCTION()
	void GetShowInShopButtonClickedEvent();

	void SetInventoryContextMenuVisible(bool bVisible);
	
	void SpawnInventoryContextMenuWidget();
	
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
	void ItemRemoved(const FInventoryItemHandle& InventoryItemHandle);
	void ItemStackChanged(const FInventoryItemHandle& InInventoryItemHandle, int NewStackCount);

	UInventoryItemWidget* GetNextAvailableSlot() const;		// 获取下一个可用的物品槽

	void HandleItemDragDrop(UInventoryItemWidget* TargetItemWidget, UInventoryItemWidget* SourceItemWidget);
	void OnItemRightButtonClicked(const FInventoryItemHandle& InventoryItemHandle);
	void OnItemLeftButtonClicked(const FInventoryItemHandle& InventoryItemHandle);

	FInventoryItemHandle CurrentFocusedItemHandle;	// 当前右键点击的物品句柄

	void ClearContextMenu();
	
};
