// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
 * 库存组件，管理角色的物品和装备
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem* /* NewItem */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FInventoryItemHandle& /* ItemHandle */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStackCountChangeDelegate, const FInventoryItemHandle& /* ItemInventoryHandle */, int /* NewCount */);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemNeedShowInShopDelegate, const FInventoryItemHandle& /* ItemHandle */);

class UPA_ShopItem;
class UArenasAbilitySystemComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARENAS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	FOnItemAddedDelegate OnItemAdded;	// 当有新物品添加时触发的委托
	FOnItemRemovedDelegate OnItemRemoved;	// 当有物品被移除时触发的委托
	FOnItemStackCountChangeDelegate OnItemStackCountChanged;	// 当物品堆叠数量变化时触发的委托
	FOnItemNeedShowInShopDelegate OnItemNeedShowInShop;	// 当有物品需要在商店中显示时触发的委托

	void TryPurchase(const UPA_ShopItem* ItemToPurchase);
	float GetGold() const;
	FORCEINLINE int GetCapacity() const { return Capacity; }
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int InSlotNumber);
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

	bool IsAllSlotsOccupied() const;	// 检查所有槽位是否已被占用
	UInventoryItem* GetAvailableStackForItem(const UPA_ShopItem* InShopItem) const;	// 获取一个可用的堆叠物品（如果有的话）

	bool IsFullForItem(const UPA_ShopItem* InShopItem) const;	// 检查库存是否已满，无法再添加指定的物品

	void TryActivateItemAbility(const FInventoryItemHandle& Handle);	// 尝试激活指定物品的能力
	void SellItem(const FInventoryItemHandle& Handle);	// 出售指定的库存物品

	bool TryItemCombination(const UPA_ShopItem* Item);	// 尝试处理物品合成
	bool FindIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutFoundIngredients, const TArray<const UPA_ShopItem*>& IngredientToIgnore = TArray<const UPA_ShopItem*>{}) const;	// 查找物品的合成材料
	UInventoryItem* GetItemByShopItem(const UPA_ShopItem* InShopItem, TArray<FInventoryItemHandle>& HasFoundHandle) const;	// 通过商店物品查找库存物品
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int Capacity = 6;	// 最大库存容量
	
	UPROPERTY()
	UArenasAbilitySystemComponent* OwnerArenasASC;
	
	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryItemsMap;	// 库存物品映射表，Key为库存物品句柄，Value为库存物品对象

	/**********************************************************************************/
	/*									Server										 */
	/**********************************************************************************/
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);	// 服务器端处理购买请求

	void GrantItem(const UPA_ShopItem* ItemToPurchase);
	void RemoveItem(UInventoryItem* Item);	// 移除指定的库存物品
	void ConsumeItem(UInventoryItem* Item);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateItemAbility(FInventoryItemHandle Handle);	// 服务器端处理激活物品能力请求（被动物品不需要调用此函数）

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(FInventoryItemHandle Handle);	// 服务器端处理出售物品请求

	/**********************************************************************************/
	/*									Client										 */
	/**********************************************************************************/
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* ItemAdded);	// 客户端添加物品

	UFUNCTION(Client, Reliable)
	void Client_ItemStackCountChanged(FInventoryItemHandle Handle, int NewStackCount);	// 客户端更新物品堆叠数量

	UFUNCTION(Client, Reliable)
	void Client_RemoveItem(FInventoryItemHandle Handle);	// 客户端移除物品
	
};
