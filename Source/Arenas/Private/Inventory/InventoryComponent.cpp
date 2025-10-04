// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryComponent.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "PA_ShopItem.h"
#include "Framework/ArenasAssetManager.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasHeroAttributeSet.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerArenasASC) return;

	Server_Purchase(ItemToPurchase);
	
}

float UInventoryComponent::GetGold() const
{
	if (OwnerArenasASC)
	{
		bool bFound = false;
		float Gold = OwnerArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound)
		{
			return Gold;
		}
	}

	return 0.f;
}


void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int InSlotNumber)
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetSlot(InSlotNumber);
	}
}

UInventoryItem* UInventoryComponent::GetInventoryItemByHandle(const FInventoryItemHandle& Handle) const
{
	UInventoryItem* const* FoundItem = InventoryItemsMap.Find(Handle);
	return FoundItem ? *FoundItem : nullptr;
}

bool UInventoryComponent::IsAllSlotsOccupied() const
{
	return InventoryItemsMap.Num() >= GetCapacity();
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UPA_ShopItem* InShopItem) const
{
	if (!InShopItem) return nullptr;

	UInventoryItem* FoundItem = nullptr;
	for (const auto& ItemPair : InventoryItemsMap)
	{
		// 查找是否有相同类型且未满的堆叠物品
		// 注意这里还要检查ItemPair.Key是否有效，因为有可能存在无效的句柄（比如物品被移除后）
		// 以及ItemPair.Value是否有效，因为有可能存在无效的库存物品对象（比如物品被移除后）
		// 以及ItemPair.Value->IsForItem(InShopItem)来检查是否是同一种物品
		// 最后还要检查ItemPair.Value->IsStackFull()来确保该堆叠物品未满
		if (ItemPair.Key.IsValid() && ItemPair.Value && ItemPair.Value->IsForItem(InShopItem) && !ItemPair.Value->IsStackFull())
		{
			FoundItem = ItemPair.Value;
			break;
		}
	}
	return FoundItem;
}

bool UInventoryComponent::IsFullForItem(const UPA_ShopItem* InShopItem) const
{
	// 如果没有该物品，或者没有可用的堆叠槽位，并且所有槽位都已占用，则表示物品栏已满
	return InShopItem && !GetAvailableStackForItem(InShopItem) && IsAllSlotsOccupied();
}

void UInventoryComponent::TryActivateItemAbility(const FInventoryItemHandle& Handle)
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		if (FoundItem->IsValid())
		{
			Server_ActivateItemAbility(Handle);
		}
	}
}

void UInventoryComponent::SellItem(const FInventoryItemHandle& Handle)
{
	Server_SellItem(Handle);
}

void UInventoryComponent::CheckItemCombination(const UInventoryItem* Item)
{
	if (!Item || !Item->IsValid() || !GetOwner() || !GetOwner()->HasAuthority()) return;

	// 获取该物品的合成条目
	const FItemCollectionEntry* CombinationEntry = UArenasAssetManager::Get().GetCombinationEntry(Item->GetShopItem());
	if (!CombinationEntry) return;

	// 检查是否拥有所有合成所需的物品，如果有则移除这些物品（因为它们将被合成为新的物品）
	for (const UPA_ShopItem* CombinationItem : CombinationEntry->GetItems())
	{
		TArray<UInventoryItem*> FoundIngredients;
		if (FoundIngredientForItem(CombinationItem, FoundIngredients))
		{
			for (UInventoryItem* IngredientItem : FoundIngredients)
			{
				RemoveItem(IngredientItem);
			}
			// 给予合成后的物品
			GrantItem(CombinationItem);
			return;
		}
	}
	
}

bool UInventoryComponent::FoundIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutFoundIngredients) const
{
	const FItemCollectionEntry* Ingredients = UArenasAssetManager::Get().GetIngredientEntry(Item);
	if (!Ingredients) return false;

	bool bAllFound = true;
	for (const UPA_ShopItem* Ingredient : Ingredients->GetItems())
	{
		if (UInventoryItem* FoundItem = GetItemByShopItem(Ingredient))
		{
			OutFoundIngredients.Add(FoundItem);
		}
		else
		{
			bAllFound = false;
			break;
		}
	}

	return bAllFound;
	
}

UInventoryItem* UInventoryComponent::GetItemByShopItem(const UPA_ShopItem* InShopItem) const
{
	if (!InShopItem) return nullptr;

	for (const auto& ItemPair : InventoryItemsMap)
	{
		if (ItemPair.Value && ItemPair.Value->IsForItem(InShopItem))
		{
			return ItemPair.Value;
		}
	}

	return nullptr;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerArenasASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(GetOwner());
	
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* ItemToPurchase)
{
	// 确保只在服务端进行
	if (!GetOwner() || !ItemToPurchase || !GetOwner()->HasAuthority()) return;

	// 首先检查是否有可用的堆叠槽位
	if (UInventoryItem* AvailableStackItem = GetAvailableStackForItem(ItemToPurchase))
	{
		// 如果有可用的堆叠槽位，则只需要增加堆叠数量，不需要创建新的库存物品对象
		// 增加堆叠数量，并广播堆叠数量变化
		if (AvailableStackItem->AddStackCount())
		{
			OnItemStackCountChanged.Broadcast(AvailableStackItem->GetHandle(), AvailableStackItem->GetStackCount());

			Client_ItemStackCountChanged(AvailableStackItem->GetHandle(), AvailableStackItem->GetStackCount());
		}
	}
	else if (UInventoryItem* NewInventoryItem = NewObject<UInventoryItem>(this))
	{
		// 创建一个新的库存物品对象，并初始化它
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
		NewInventoryItem->InitializeItem(ItemToPurchase, NewHandle);
		InventoryItemsMap.Add(NewHandle, NewInventoryItem);

		OnItemAdded.Broadcast(NewInventoryItem);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
			FString::Printf(TEXT("Server Added Item: %s, ID: %d"),
				*NewInventoryItem->GetShopItem()->GetItemName().ToString(),
				NewInventoryItem->GetHandle().GetHandleId()));

		Client_ItemAdded(NewHandle, ItemToPurchase);

		// 应用物品的GAS修改
		NewInventoryItem->ApplyGASModifications(OwnerArenasASC);

		CheckItemCombination(NewInventoryItem);
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
	if (!Item || !GetOwner() || !GetOwner()->HasAuthority()) return;
	// 首先移除物品的GAS修改
	Item->RemoveGASModifications(OwnerArenasASC);
	// 服务端广播
	OnItemRemoved.Broadcast(Item->GetHandle());
	// 从映射表中移除该物品
	InventoryItemsMap.Remove(Item->GetHandle());
	// 客户端移除
	Client_RemoveItem(Item->GetHandle());
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	if (!Item || !GetOwner() || !GetOwner()->HasAuthority()) return;

	// 首先应用消耗品的GAS修改
	Item->ApplyConsumableGASModifications(OwnerArenasASC);

	if (!Item->ReduceStackCount())
	{
		// 如果减少堆叠数量后为0，则移除该物品
		RemoveItem(Item);
	}
	else
	{
		// 否则广播堆叠数量变化
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
		Client_ItemStackCountChanged(Item->GetHandle(), Item->GetStackCount());
	}
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle Handle)
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		if (!FoundItem->IsValid()) return;

		const UPA_ShopItem* ShopItem = FoundItem->GetShopItem();
		if (!ShopItem) return;

		// 出售物品，获得金币
		float SellPrice = ShopItem->GetSellPrice();	// 假设出售价格为购买价格的一半
		OwnerArenasASC->ApplyModToAttribute(UArenasHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, SellPrice * FoundItem->GetStackCount());

		// 移除物品
		RemoveItem(FoundItem);
	}
}

bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle Handle)
{
	return true;
}

void UInventoryComponent::Server_ActivateItemAbility_Implementation(FInventoryItemHandle Handle)
{
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->TryActivateActiveItemAbility(OwnerArenasASC);
		const UPA_ShopItem* ShopItem = FoundItem->GetShopItem();
		if (ShopItem && ShopItem->IsConsumable())
		{
			ConsumeItem(FoundItem);
		}
	}
}

bool UInventoryComponent::Server_ActivateItemAbility_Validate(FInventoryItemHandle Handle)
{
	return true;
}

void UInventoryComponent::Client_RemoveItem_Implementation(FInventoryItemHandle Handle)
{
	if (!GetOwner() || GetOwner()->HasAuthority()) return;

	// 在客户端移除物品
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		OnItemRemoved.Broadcast(Handle);
		InventoryItemsMap.Remove(Handle);
	}
}

void UInventoryComponent::Client_ItemStackCountChanged_Implementation(FInventoryItemHandle Handle, int NewStackCount)
{
	if (!GetOwner() || GetOwner()->HasAuthority()) return;

	// 在客户端更新堆叠数量
	if (UInventoryItem* FoundItem = GetInventoryItemByHandle(Handle))
	{
		FoundItem->SetStackCount(NewStackCount);
		OnItemStackCountChanged.Broadcast(Handle, NewStackCount);
	}
	
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* ItemAdded)
{
	// 确保只在客户端进行 （接收到服务端创建的库存句柄，然后客户端依据该句柄创建对应的库存物品对象副本）
	if (!GetOwner() || GetOwner()->HasAuthority()) return;
	
	if (UInventoryItem* NewInventoryItem = NewObject<UInventoryItem>(this))
	{
		NewInventoryItem->InitializeItem(ItemAdded, AssignedHandle);
		InventoryItemsMap.Add(AssignedHandle, NewInventoryItem);

		OnItemAdded.Broadcast(NewInventoryItem);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange,
			FString::Printf(TEXT("Client Received Item: %s, ID: %d"),
				*NewInventoryItem->GetShopItem()->GetItemName().ToString(),
				NewInventoryItem->GetHandle().GetHandleId()));
	}
	
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;

	if (GetGold() < ItemToPurchase->GetPrice())
	{
		return;
	}

	if (IsFullForItem(ItemToPurchase))
	{
		return;
	}

	OwnerArenasASC->ApplyModToAttribute(UArenasHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());

	// 购买物品金币扣除成功，发放物品
	GrantItem(ItemToPurchase);
}


