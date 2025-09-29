// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ShopWidget.h"

#include "ShopItemWidget.h"
#include "Components/TileView.h"
#include "Framework/ArenasAssetManager.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	LoadShopItems();
	ShopItemTileView->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetsGenerated);
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
		ShopItemWidgetMap.Add(ShopItemWidget->GetShopItem(), ShopItemWidget);
	}
}
