// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Interface/TreeNodeInterface.h"
#include "ShopItemWidget.generated.h"

class UListView;
class UShopItemWidget;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 * 
 */
UCLASS()
class ARENAS_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry, public ITreeNodeInterface
{
	GENERATED_BODY()

public:
	virtual UUserWidget* GetWidget() const override;
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override;
	virtual TArray<const ITreeNodeInterface*> GetOutputs() const override;
	virtual const UObject* GetItemObject() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnItemSelected(bool bSelected);	// 当物品被选中时触发的事件
	
	FOnItemPurchaseIssued OnItemPurchaseIssued;		// 当已发出商品购买
	FOnShopItemSelected OnShopItemSelected;			// 当商店物品被选中
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	FORCEINLINE bool IsItemSelected() const { return bIsSelected; }

private:
	void CopyFromOther(const UShopItemWidget* Other);
	void InitWithShopItem(const UPA_ShopItem* NewShopItem);
	TArray<const ITreeNodeInterface*> ItemToInterface(const TArray<const UPA_ShopItem*>& Items) const;
	
	
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;

	UPROPERTY()
	const UListView* ParentListView;

	bool bIsSelected = false;
};

