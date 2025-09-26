// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "GAP_Dead.generated.h"

/**
 * 死亡被动： 当角色死亡时触发，通常用于处理角色的死亡状态和相关逻辑，奖励经验等
 */
UCLASS()
class ARENAS_API UGAP_Dead : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UGAP_Dead();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float RewardRange = 1000.f; // 奖励范围

	TArray<AActor*> GetRewardTargets() const;
	
	
};
