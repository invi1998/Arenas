// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ShopWidget.h"

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

void UShopWidget::ShopItemWidgetsGenerated(UUserWidget& NewShopItemWidget)
{
	if (UShopItemWidget* ShopItemWidget = Cast<UShopItemWidget>(&NewShopItemWidget))
	{
		if (OwnerInventoryComponent)
		{
			// 此处购买物品的委托的发起时机是玩家点击物品时发出的
			// 绑定物品购买时的委托，然后让库存组件去执行购买物品逻辑
			ShopItemWidget->OnItemPurchaseIssued.AddUObject(OwnerInventoryComponent, &UInventoryComponent::TryPurchase);
		}
		
		ShopItemWidgetMap.Add(ShopItemWidget->GetShopItem(), ShopItemWidget);
	}
}
