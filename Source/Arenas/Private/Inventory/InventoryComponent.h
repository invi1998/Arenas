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

	void TryPurchase(const UPA_ShopItem* ItemToPurchase);
	float GetGold() const;
	FORCEINLINE int GetCapacity() const { return Capacity; }
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int InSlotNumber);
	UInventoryItem* GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const;

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
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);

	void GrantItem(const UPA_ShopItem* ItemToPurchase);

	/**********************************************************************************/
	/*									Client										 */
	/**********************************************************************************/
	UFUNCTION(Client, Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* ItemAdded);
	
};
