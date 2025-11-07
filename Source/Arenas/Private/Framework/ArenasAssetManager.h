// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Inventory/PA_ShopItem.h"
#include "ArenasAssetManager.generated.h"

class UPA_ShopItem;
class UPA_CharacterDefinition;
/**
 * 基类是一个单例类，继承自UAssetManager，用于管理游戏中的各种资源。
 * 通过这个类，可以方便地加载、卸载和管理游戏中的各种资源，这就是我们的物品栏，商店等类型的资源管理。
 * 该类通常在游戏启动时初始化，并在整个游戏生命周期中保持存在。
 */
UCLASS()
class ARENAS_API UArenasAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UArenasAssetManager& Get();
	void LoadShopItems(const FStreamableDelegate& LoadFinishedCallback);
	bool GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutShopItems) const;

	void LoadCharacterDefinitions(const FStreamableDelegate& LoadFinishedCallback);
	bool GetLoadedCharacterDefinitions(TArray<UPA_CharacterDefinition*>& OutCharacterDefinitions) const;

	const FItemCollectionEntry* GetCombinationEntry(const UPA_ShopItem* CombinationItem) const;		// 获取合成物品所需的材料条目
	const FItemCollectionEntry* GetIngredientEntry(const UPA_ShopItem* IngredientItem) const;		// 获取材料物品可以合成的物品条目

private:
	void ShopItemLoadCompleted(FStreamableDelegate CallBack);
	void BuildItemMaps();
	void AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem);

	// 物品合成映射表，Key为合成后的物品，Value为合成该物品所需的材料和数量
	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollectionEntry> CombinationMap;

	// 物品合成映射表，Key为材料物品，Value为使用该材料可以合成的物品和数量
	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollectionEntry> IngredientMap;
	
};
