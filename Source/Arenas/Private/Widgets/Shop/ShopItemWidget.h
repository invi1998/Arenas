// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;
/**
 * 
 */
UCLASS()
class ARENAS_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }

private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
};
