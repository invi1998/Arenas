// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "Abilities/GameplayAbility.h"
#include "UObject/Object.h"
#include "InventoryItem.generated.h"

/**
 * 库存物品基类
 */

/*
在这个类中我们需要存储相当多的东西。
这基本上是我们在物品栏组件中存储的物品之一。
以及账户，如果我们有的话。
以及如果他们有一些，如果他们授予了一些能力，那就是能力句柄；以及如果它们正在拥有
一些效果，则为效果句柄。
我们需要处理所有这些，或者将所有这些存放在这个库存中。
这就意味着这个类可能会非常庞大，嗯，包含许多成员。
并且，通过网络通信来传输这些信息实际上会非常低效。
所以，我们这里将尝试的另一种方法是不通过发送来进行通信。

我们使用一种特定类型，称为库存原子句柄（inventory atom handle），它将是一个
用于识别特定物品的唯一标识符。在客户端和服务器端，它们各自用那个特定的 ID 创建自己的特定的句柄副本。
然后用它来沟通我们在说的是哪一个。
我们在网络通信中就只需要提供那个句柄。

其实不难发现，这种做法实际上类似于GAS里的游戏能力规范句柄（GameplayAbilitySpecHandle）。
基本上内部存储的是一个整数，那个句柄变量。
而且那个句柄每次都会是唯一的。
然后你只需要通过网络通信传递它。
然后你传输的基本上只是一个整数。
就大小而言，这将更高效。
所以我们将采用类似的方法。

*/

struct FActiveGameplayEffectHandle;
class UArenasAbilitySystemComponent;
class UPA_ShopItem;
class UGameplayAbility;

USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()

public:
	FInventoryItemHandle();

	// 静态工厂函数，是唯一用来正确创建句柄应该调用的静态方法
	static FInventoryItemHandle CreateHandle();

	// 返回一个无效的句柄
	static FInventoryItemHandle InvalidHandle();

	bool IsValid() const;
	uint32 GetHandleId() const { return HandleID; }

private:
	// 我们提供一个私有的带参数的构造函数，以确保句柄只能通过静态创建函数来生成 CreateHandle
	// 同时，该构造函数被声明为 explicit，以防止隐式转换
	// 例如：FInventoryItemHandle MyHandle = 5; // 错误
	// 这样做可以防止意外地将整数转换为句柄，从而引发错误
	// 现在只能通过 FInventoryItemHandle::CreateHandle() 来创建句柄
	explicit FInventoryItemHandle(uint32 InHandleID);
	
	UPROPERTY()
	uint32 HandleID;

	// 我们的句柄ID需要一个唯一的无符号整形数
	static uint32 GenerateNextHandleID();

	// 返回一个无效ID
	static uint32 GetInvalidID();
	
};

// 重载比较运算符，以便我们可以直接比较两个 FInventoryItemHandle 实例
bool operator==(const FInventoryItemHandle& A, const FInventoryItemHandle& B);
bool operator!=(const FInventoryItemHandle& A, const FInventoryItemHandle& B);

// 为了让 FInventoryItemHandle 能够作为 TMap 或 TSet 的键，我们需要为其定义一个哈希函数
uint32 GetTypeHash(const FInventoryItemHandle& Handle);



UCLASS()
class ARENAS_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItem();
	
	const FInventoryItemHandle& GetHandle() const { return Handle; }
	const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	void InitializeItem(const UPA_ShopItem* InShopItem, const FInventoryItemHandle& InHandle, UArenasAbilitySystemComponent* InAbilitySystemComponent);
	
	bool TryActivateGrantedAbility();		// 尝试激活物品授予的被动技能
	bool TryActivateActiveItemAbility();	// 尝试激活物品的主动技能
	void RemoveGASModifications();
	void ApplyConsumableGASModifications();
	
	bool IsValid() const;
	FORCEINLINE int GetStackCount() const { return StackCount; }
	void SetSlot(int InSlotNumber);

	bool IsStackFull() const;	// 检查该物品堆叠是否已满
	bool IsForItem(const UPA_ShopItem* InShopItem) const;	// 检查该库存物品是否属于指定的商店物品

	bool AddStackCount();	// 增加堆叠数量，返回是否成功增加
	bool ReduceStackCount();	// 减少堆叠数量，返回是否成功（如果减少后堆叠数量为0则返回false）

	bool SetStackCount(int InStackCount);
	bool IsGrantedActiveAbility(TSubclassOf<UGameplayAbility> AbilityClass) const;
	bool IsGrantedAnyActiveAbility() const;

	float GetAbilityCooldownTimeRemaining() const;	// 获取物品的主动技能的剩余冷却时间
	float GetAbilityCooldownDuration() const;		// 获取物品的主动技能的冷却持续时间
	float GetAbilityManaCost() const;				// 获取物品的主动技能的法力消耗

	bool CanCastItemAbility() const;	// 检查物品的主动技能是否可以施放（即是否有足够的法力值）

private:
	void ApplyGASModifications();
	
	UPROPERTY()
	UArenasAbilitySystemComponent* OwningArenasASC;
	
	FInventoryItemHandle Handle;
	
	UPROPERTY()
	const UPA_ShopItem* ShopItem;

	int StackCount;
	int Slot;		// 物品栏中的槽位
	
	FActiveGameplayEffectHandle AppliedEquippedEffectHandle;			// 应用的装备效果句柄
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;				// 授予的能力规范句柄（被动技能）
	FGameplayAbilitySpecHandle ActiveItemAbilitySpecHandle;				// 物品的主动技能能力规范句柄
	
};




















