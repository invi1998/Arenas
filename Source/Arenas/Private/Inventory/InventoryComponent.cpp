// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryComponent.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "PA_ShopItem.h"
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

	if (UInventoryItem* NewInventoryItem = NewObject<UInventoryItem>(this))
	{
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

	OwnerArenasASC->ApplyModToAttribute(UArenasHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());

	// 购买物品金币扣除成功，发放物品
	GrantItem(ItemToPurchase);
}


