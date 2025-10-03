// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_ShopItem.generated.h"

class UPA_ShopItem;
class UGameplayAbility;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FItemCollectionEntry
{
	GENERATED_BODY()

public:
	FItemCollectionEntry();
	FItemCollectionEntry(const TArray<const UPA_ShopItem*>& InItems);
	void AddItem(const UPA_ShopItem* InItem, bool bAddUnique = false);
	bool Contains(const UPA_ShopItem* InItem) const;
	const TArray<const UPA_ShopItem*>& GetItems() const { return Items; }

private:
	UPROPERTY()
	TArray<const UPA_ShopItem*> Items;
	
};

/**
 * 
 */
UCLASS()
class ARENAS_API UPA_ShopItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetShopItemAssetType();

	UTexture2D* GetIcon() const;
	FText GetItemName() const { return ItemName; }
	FText GetDescription() const { return ItemDescription; }
	FText GetEffectRichText() const { return EffectRichText; }
	FText GetActiveAbilityName() const { return ActiveAbilityName; }
	FText GetActiveAbilityCostRichText() const { return ActiveAbilityCostRichText; }
	FText GetActiveAbilityRichText() const { return ActiveAbilityRichText; }
	FText GetPassiveAbilityName() const { return PassiveAbilityName; }
	FText GetPassiveAbilityRichText() const { return PassiveAbilityRichText; }
	float GetPrice() const { return Price; }
	float GetSellPrice() const { return Price * 0.5f; } // 出售价格为购买价格的一半
	bool IsConsumable() const { return bIsConsumable; }
	TSubclassOf<UGameplayEffect> GetEquippedEffect() const { return EquippedEffect; }
	TSubclassOf<UGameplayEffect> GetConsumedEffect() const { return ConsumedEffect; }
	TSubclassOf<UGameplayAbility> GetGrantedAbility() const { return GrantedAbility; }
	bool IsStackable() const { return bIsStackable; }
	int GetMaxStackCount() const { return MaxStackCount; }
	const TArray<TSoftObjectPtr<UPA_ShopItem>>& GetIngredientItems() const { return IngredientItems; }
	bool CanCastActiveAbility() const;


private:
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	float Price;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemDescription;

	// 物品效果的富文本描述，支持多行文本，meta = (MultiLine = true)
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (MultiLine = true))
	FText EffectRichText; // 物品效果的富文本描述

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ActiveAbilityName; // 主动技能名称

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ActiveAbilityCostRichText; // 主动技能的消耗富文本描述

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (MultiLine = true))
	FText ActiveAbilityRichText; // 主动技能的富文本描述

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText PassiveAbilityName; // 被动技能名称

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem", meta = (MultiLine = true))
	FText PassiveAbilityRichText; // 被动技能的富文本描述

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsConsumable = false;		// 是否是消耗品

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> EquippedEffect;	// 装备后赋予的GameplayEffect

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayEffect> ConsumedEffect;	// 使用后赋予的GameplayEffect

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<UGameplayAbility> GrantedAbility;	// 购买后赋予的技能

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsStackable = false;	// 是否可叠加

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	int MaxStackCount = 1;		// 最大叠加数量

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TArray<TSoftObjectPtr<UPA_ShopItem>> IngredientItems; // 合成该物品所需的材料
	
};
