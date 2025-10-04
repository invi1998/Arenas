// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasAssetManager.h"

#include "Inventory/PA_ShopItem.h"

UArenasAssetManager& UArenasAssetManager::Get()
{
	if (UArenasAssetManager* Singleton = Cast<UArenasAssetManager>(GEngine->AssetManager.Get()))
	{
		return *Singleton;
	}

	return *NewObject<UArenasAssetManager>();
}

void UArenasAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	// 这里传入一个空的TArray<FName>，表示加载该类型的所有资源 （如果想支持一些高级功能，比如只加载某些特定的资源（如只加载图标等），可以传入一个包含这些资源名称的数组）
	LoadPrimaryAssetsWithType(UPA_ShopItem::GetShopItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UArenasAssetManager::ShopItemLoadCompleted, LoadFinishedCallback));
}

bool UArenasAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutShopItems) const
{
	TArray<UObject*> LoadedObjects;
	bool Loaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedObjects);

	if (Loaded)
	{
		for (UObject* Item : LoadedObjects)
		{
			OutShopItems.Add(Cast<UPA_ShopItem>(Item));
		}
	}

	return Loaded;
}

const FItemCollectionEntry* UArenasAssetManager::GetCombinationEntry(const UPA_ShopItem* CombinationItem) const
{
	return CombinationMap.Find(CombinationItem);
}

const FItemCollectionEntry* UArenasAssetManager::GetIngredientEntry(const UPA_ShopItem* IngredientItem) const
{
	return IngredientMap.Find(IngredientItem);
}

void UArenasAssetManager::ShopItemLoadCompleted(FStreamableDelegate CallBack)
{
	CallBack.ExecuteIfBound();
	BuildItemMaps();
}

void UArenasAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
	if (FItemCollectionEntry* Combinations = CombinationMap.Find(Ingredient))
	{
		if (!Combinations->Contains(CombinationItem))
		{
			Combinations->AddItem(CombinationItem);
		}
	}
	else
	{
		CombinationMap.Add(Ingredient, FItemCollectionEntry{TArray<const UPA_ShopItem*>{CombinationItem}});
	}
}

void UArenasAssetManager::BuildItemMaps()
{
	// 遍历所有加载的UPA_ShopItem资源，并将它们填充在 CombinationMap （组合映射） 和 IngredientMap （成分映射） 中
	TArray<const UPA_ShopItem*> LoadedShopItems;
	if (GetLoadedShopItems(LoadedShopItems))
	{
		for (const UPA_ShopItem* ShopItem : LoadedShopItems)
		{
			if (ShopItem->GetIngredientItems().Num() == 0) continue;

			TArray<const UPA_ShopItem*> ShopItems;
			for (const TSoftObjectPtr<UPA_ShopItem>& IngredientItem : ShopItem->GetIngredientItems())
			{
				UPA_ShopItem* IngredientShopItem = IngredientItem.LoadSynchronous();
				ShopItems.Add(IngredientShopItem);
				AddToCombinationMap(IngredientShopItem, ShopItem);	// 将该物品添加到组合映射中
			}

			IngredientMap.Add(ShopItem, FItemCollectionEntry{ShopItems});
			
		}
	}
}

