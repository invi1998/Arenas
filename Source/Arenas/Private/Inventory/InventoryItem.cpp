// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "InventoryItem.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "PA_ShopItem.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"

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

void UInventoryItem::InitializeItem(const UPA_ShopItem* InShopItem, const FInventoryItemHandle& InHandle, UArenasAbilitySystemComponent* InAbilitySystemComponent)
{
	Handle = InHandle;
	ShopItem = InShopItem;
	OwningArenasASC = InAbilitySystemComponent;
	if (OwningArenasASC)
	{
		OwningArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &UInventoryItem::OnManaChanged);
	}
	ApplyGASModifications();
}

bool UInventoryItem::TryActivatePassiveAbility()
{
	if (!OwningArenasASC || !ItemPassiveAbilitySpecHandle.IsValid()) return false;
	return OwningArenasASC->TryActivateAbility(ItemPassiveAbilitySpecHandle);
}

bool UInventoryItem::TryActivateActiveItemAbility()
{
	if (!OwningArenasASC || !ItemActiveAbilitySpecHandle.IsValid()) return false;
	return OwningArenasASC->TryActivateAbility(ItemActiveAbilitySpecHandle);
}

void UInventoryItem::RemoveGASModifications()
{
	if (!GetShopItem() || !OwningArenasASC) return;
	OwningArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).RemoveAll(this);

	// 确保我们只在服务器上进行GAS修改
	if (!OwningArenasASC->GetOwner() || !OwningArenasASC->GetOwner()->HasAuthority()) return;
	if (AppliedEquippedEffectHandle.IsValid())
	{
		OwningArenasASC->RemoveActiveGameplayEffect(AppliedEquippedEffectHandle);
		AppliedEquippedEffectHandle.Invalidate();
	}
	if (ItemPassiveAbilitySpecHandle.IsValid())
	{
		// 对于授予的能力，我们不立即移除它们，而是在技能结束后才移除
		OwningArenasASC->SetRemoveAbilityOnEnd(ItemPassiveAbilitySpecHandle);
	}
	if (ItemActiveAbilitySpecHandle.IsValid())
	{
		OwningArenasASC->SetRemoveAbilityOnEnd(ItemActiveAbilitySpecHandle);
	}
}

void UInventoryItem::ApplyConsumableGASModifications()
{
	if (!GetShopItem() || !OwningArenasASC) return;
	if (!OwningArenasASC->GetOwner() || !OwningArenasASC->GetOwner()->HasAuthority()) return;
	if (TSubclassOf<UGameplayEffect> ConsumableEffect = GetShopItem()->GetConsumedEffect())
	{
		OwningArenasASC->BP_ApplyGameplayEffectToSelf(ConsumableEffect, 1, OwningArenasASC->MakeEffectContext());
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

bool UInventoryItem::IsGrantedActiveAbility(TSubclassOf<UGameplayAbility> AbilityClass) const
{
	if (!ShopItem) return false;
	TSubclassOf<UGameplayAbility> GrantedAbility = ShopItem->GetActiveAbility();
	return GrantedAbility && GrantedAbility == AbilityClass;
}

bool UInventoryItem::IsGrantedAnyActiveAbility() const
{
	if (!ShopItem) return false;
	return ShopItem->GetActiveAbility() != nullptr;
}

float UInventoryItem::GetAbilityCooldownTimeRemaining() const
{
	if (!OwningArenasASC || !IsGrantedAnyActiveAbility()) return 0.f;

	return UArenasBlueprintFunctionLibrary::GetAbilityCooldownRemainingTime(GetShopItem()->GetActiveAbilityCDO(), OwningArenasASC);
}

float UInventoryItem::GetAbilityCooldownDuration() const
{
	if (!OwningArenasASC || !IsGrantedAnyActiveAbility()) return 0.f;
	return UArenasBlueprintFunctionLibrary::GetAbilityCooldownDuration(GetShopItem()->GetActiveAbilityCDO(), OwningArenasASC, 1);
}

float UInventoryItem::GetAbilityManaCost() const
{
	if (!OwningArenasASC || !IsGrantedAnyActiveAbility()) return 0.f;
	return UArenasBlueprintFunctionLibrary::GetAbilityManaCost(GetShopItem()->GetActiveAbilityCDO(), OwningArenasASC, 1);
}

bool UInventoryItem::CanCastItemAbility() const
{
	if (!OwningArenasASC || !IsGrantedAnyActiveAbility()) return false;

	if (FGameplayAbilitySpec* AbilitySpec = OwningArenasASC->FindAbilitySpecFromClass(GetShopItem()->GetActiveAbility()))
	{
		return UArenasBlueprintFunctionLibrary::CheckAbilityCanCost(*AbilitySpec, OwningArenasASC);
	}
	return UArenasBlueprintFunctionLibrary::CheckAbilityCanCost_Static(GetShopItem()->GetActiveAbilityCDO(), OwningArenasASC);
	
}

void UInventoryItem::ApplyGASModifications()
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
		ItemPassiveAbilitySpecHandle = OwningArenasASC->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
		// 对于被动技能，我们可以直接激活它
		OwningArenasASC->TryActivateAbility(ItemPassiveAbilitySpecHandle);
	}

	if (TSubclassOf<UGameplayAbility> ActiveItemAbility = GetShopItem()->GetActiveAbility())
	{
		ItemActiveAbilitySpecHandle = OwningArenasASC->GiveAbility(FGameplayAbilitySpec(ActiveItemAbility));
	}
	
}

void UInventoryItem::OnManaChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	OnAbilityCanCastUpdated.Broadcast(CanCastItemAbility());
}
