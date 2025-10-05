// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ShopWidget.h"

#include "ItemTreeWidget.h"
#include "ShopItemWidget.h"
#include "Components/TileView.h"
#include "Framework/ArenasAssetManager.h"
#include "Inventory/InventoryComponent.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	LoadShopItems();
	ShopItemTileView->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetsGenerated);
	ShopItemTileView->OnItemSelectionChanged().AddUObject(this, &UShopWidget::HandleShopItemSelectedChanged);
	if (APawn* OwnerPawn = GetOwningPlayerPawn())
	{
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
	}
	
}

void UShopWidget::LoadShopItems()
{
	UArenasAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &UShopWidget::ShopItemLoadedComplete));
}

void UShopWidget::ShopItemLoadedComplete()
{
	TArray<const UPA_ShopItem*> ShopItems;
	UArenasAssetManager::Get().GetLoadedShopItems(ShopItems);
	for (const UPA_ShopItem* ShopItem : ShopItems)
	{
		ShopItemTileView->AddItem(const_cast<UPA_ShopItem*>(ShopItem));
	}
}

void UShopWidget::HandleShopItemSelected(const UShopItemWidget* ShopItemWidget)
{
	const UShopItemWidget** FoundWidget = ShopItemWidgetMap.Find(ShopItemWidget->GetShopItem());
	if (FoundWidget && *FoundWidget)
	{
		ShopItemTileView->SetSelectedItem(*FoundWidget);
	}
}

void UShopWidget::HandleInventoryItemNeedShowInShop(const FInventoryItemHandle& InventoryItemHandle)
{
	const UShopItemWidget** FoundWidget = ShopItemWidgetMap.Find(OwnerInventoryComponent->GetInventoryItemByHandle(InventoryItemHandle)->GetShopItem());
	if (FoundWidget && *FoundWidget)
	{
		ShopItemTileView->SetSelectedItem(*FoundWidget);
		// HandleShopItemSelected(*FoundWidget);
	}
}

void UShopWidget::ShopItemWidgetsGenerated(UUserWidget& NewShopItemWidget)
{
	if (UShopItemWidget* ShopItemWidget = Cast<UShopItemWidget>(&NewShopItemWidget))
	{
		if (OwnerInventoryComponent)
		{
			// 此处购买物品的委托的发起时机是玩家点击物品时发出的
			// 绑定物品购买时的委托，然后让库存组件去执行购买物品逻辑
			ShopItemWidget->OnItemPurchaseIssued.AddUObject(OwnerInventoryComponent, &UInventoryComponent::TryPurchase);
			// 订阅库存组件的物品需要在商店中显示的委托
			OwnerInventoryComponent->OnItemNeedShowInShop.AddUObject(this, &UShopWidget::HandleInventoryItemNeedShowInShop);
		}

		ShopItemWidget->OnShopItemSelected.AddUObject(this, &UShopWidget::HandleShopItemSelected);
		
		ShopItemWidgetMap.Add(ShopItemWidget->GetShopItem(), ShopItemWidget);
	}
}

void UShopWidget::HandleShopItemSelectedChanged(UObject* ItemWidgetObject)
{
	if (UShopItemWidget* SelectedItem = Cast<UShopItemWidget>(ItemWidgetObject))
	{
		if (CurrentSelectedShopItemWidget && CurrentSelectedShopItemWidget != SelectedItem)
		{
			CurrentSelectedShopItemWidget->OnItemSelected(false);
		}
		CurrentSelectedShopItemWidget = SelectedItem;
		SelectedItem->OnItemSelected(true);
		if (ItemCombinationTreeWidget)
		{
			ItemCombinationTreeWidget->DrawFromNode(SelectedItem);
		}
	}
}
