// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UItemTreeWidget;
class UInventoryComponent;
class UShopItemWidget;
class UPA_ShopItem;
class UTileView;
/**
 * 
 */
UCLASS()
class ARENAS_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta=(BindWidget))
	UTileView* ShopItemTileView;		// 商店物品列表

	UPROPERTY(meta=(BindWidget))
	UItemTreeWidget* ItemCombinationTreeWidget;		// 物品合成树

	void LoadShopItems();
	void ShopItemLoadedComplete();
	void HandleShopItemSelected(const UShopItemWidget* ShopItemWidget);
	void ShopItemWidgetsGenerated(UUserWidget& NewShopItemWidget);

	UPROPERTY()
	TMap<const UPA_ShopItem*, const UShopItemWidget*> ShopItemWidgetMap;

	UPROPERTY()
	UInventoryComponent* OwnerInventoryComponent;
	
};
