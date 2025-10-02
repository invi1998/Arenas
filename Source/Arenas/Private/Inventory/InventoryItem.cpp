// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryItem.h"

#include "PA_ShopItem.h"
#include "GAS/ArenasAbilitySystemComponent.h"

FInventoryItemHandle::FInventoryItemHandle()
	: HandleID(GetInvalidID())
{
	// 对于默认构造的库存句柄，我们将其初始化为无效ID
	// 这样可以确保未初始化的句柄不会意外地与任何有效的句柄匹配
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	return FInventoryItemHandle(GenerateNextHandleID());
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	return FInventoryItemHandle();
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleID != GetInvalidID();
}

FInventoryItemHandle::FInventoryItemHandle(uint32 InHandleID)
	: HandleID(InHandleID)
{
}

uint32 FInventoryItemHandle::GenerateNextHandleID()
{
	static uint32 nextHandleID = 1;
	return nextHandleID++;	// 这会给我它之前的值，然后递增它（即首次调用返回1，下次调用返回2，以此类推，而非首次调用返回2）
}

uint32 FInventoryItemHandle::GetInvalidID()
{
	// 返回一个无效ID，这里选择0作为无效ID
	return 0;
}

bool operator==(const FInventoryItemHandle& A, const FInventoryItemHandle& B)
{
	return A.GetHandleId() == B.GetHandleId();	
}

bool operator!=(const FInventoryItemHandle& A, const FInventoryItemHandle& B)
{
	return !(A == B);
}

uint32 GetTypeHash(const FInventoryItemHandle& Handle)
{
	return Handle.GetHandleId();
}

UInventoryItem::UInventoryItem()
	: ShopItem(nullptr)
	, StackCount(1)
{
}

void UInventoryItem::InitializeItem(const UPA_ShopItem* InShopItem, const FInventoryItemHandle& InHandle)
{
	Handle = InHandle;
	ShopItem = InShopItem;
}

void UInventoryItem::ApplyGASModifications(UArenasAbilitySystemComponent* OwningArenasASC)
{
	if (!GetShopItem() || !OwningArenasASC) return;

	// 确保我们只在服务器上应用GAS修改
	if (!OwningArenasASC->GetOwner() || !OwningArenasASC->GetOwner()->HasAuthority()) return;

	if (TSubclassOf<UGameplayEffect> EquippedEquippedEffect = GetShopItem()->GetEquippedEffect())
	{
		AppliedEquippedEffectHandle = OwningArenasASC->BP_ApplyGameplayEffectToSelf(EquippedEquippedEffect, 1, OwningArenasASC->MakeEffectContext());
		
	}

	if (TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility())
	{
		// 授予物品对应的能力，但是首先需要检查是否在GAS里已经拥有了该技能
		const FGameplayAbilitySpec* FoundAbilitySpec = OwningArenasASC->FindAbilitySpecFromClass(GrantedAbility);
		if (!FoundAbilitySpec)
		{
			// 如果没有找到该能力，则授予它
			GrantedAbilitySpecHandle = OwningArenasASC->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
		}
		else
		{
			GrantedAbilitySpecHandle = FoundAbilitySpec->Handle;
		}
		
	}
	
}

bool UInventoryItem::IsValid() const
{
	return ShopItem != nullptr && Handle.IsValid();
}

void UInventoryItem::SetSlot(int InSlotNumber)
{
	Slot = InSlotNumber;
}

bool UInventoryItem::IsStackFull() const
{
	return StackCount >= GetShopItem()->GetMaxStackCount();
}

bool UInventoryItem::IsForItem(const UPA_ShopItem* InShopItem) const
{
	return InShopItem && GetShopItem() == InShopItem;
}

bool UInventoryItem::AddStackCount()
{
	if (IsStackFull()) return false;

	++StackCount;
	return true;
}

bool UInventoryItem::ReduceStackCount()
{
	--StackCount;
	return StackCount > 0;
}

bool UInventoryItem::SetStackCount(int InStackCount)
{
	if (InStackCount <= 0 || InStackCount > GetShopItem()->GetMaxStackCount()) return false;
	StackCount = InStackCount;
	return true;
}
