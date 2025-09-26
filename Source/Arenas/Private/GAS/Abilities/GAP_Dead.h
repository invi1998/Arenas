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

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float BaseExperienceReward = 200.f; // 基础经验奖励

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float BaseGoldReward = 200.f; // 基础金币奖励

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float ExperienceRewardExperience = 0.1f; // 每点经验奖励的经验值(依据你击杀对象所拥有的经验值，奖励你额外的经验，即：击杀等级越高的对象，获得的额外经验就会越多）

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float GoldRewardPerExperience = 0.05f;	// 每点经验奖励的金币值 （依据你击杀对象所拥有的经验值，奖励你额外的金币，即：击杀等级越高的对象，获得的额外金币就会越多）
	
	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	float KillerRewardPercent = 0.5f; // 击杀者奖励百分比（击杀者直接获得的总奖励的百分比，剩下的均分给范围内的其他英雄，包括击杀者）

	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	TSubclassOf<UGameplayEffect> RewardEffectClass; // 奖励效果类
	
	TArray<AActor*> GetRewardTargets() const;
	
	
};
