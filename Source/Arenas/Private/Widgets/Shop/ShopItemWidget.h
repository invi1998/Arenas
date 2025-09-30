// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ShopItemWidget.generated.h"

class UShopItemWidget;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 * 
 */
UCLASS()
class ARENAS_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	FOnItemPurchaseIssued OnItemPurchaseIssued;		// 当已发出商品购买
	FOnShopItemSelected OnShopItemSelected;			// 当商店物品被选中
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }

private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;
};
