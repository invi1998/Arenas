// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "PA_ShopItem.h"

#include "Abilities/GameplayAbility.h"

FItemCollectionEntry::FItemCollectionEntry()
	: Items{}
{
}

FItemCollectionEntry::FItemCollectionEntry(const TArray<const UPA_ShopItem*>& InItems)
	: Items{InItems}
{
}

void FItemCollectionEntry::AddItem(const UPA_ShopItem* InItem, bool bAddUnique)
{
	if (bAddUnique && Items.Contains(InItem))
	{
		return;
	}

	Items.Add(InItem);
}

bool FItemCollectionEntry::Contains(const UPA_ShopItem* InItem) const
{
	return Items.Contains(InItem);
}

FPrimaryAssetId UPA_ShopItem::GetPrimaryAssetId() const
{
	// 返回该商店物品的主资产ID，类型为"ShopItem"，名称为该对象的名称
	return FPrimaryAssetId(GetShopItemAssetType(), GetFName());
}

FPrimaryAssetType UPA_ShopItem::GetShopItemAssetType()
{
	return FPrimaryAssetType("ShopItem");
}

UTexture2D* UPA_ShopItem::GetIcon() const
{
	return Icon.LoadSynchronous();
}

UGameplayAbility* UPA_ShopItem::GetActiveAbilityCDO() const
{
	return ActiveAbility ? ActiveAbility->GetDefaultObject<UGameplayAbility>() : nullptr;
}
