// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Launched.h"

#include "ArenasGameplayTags.h"

UArenasGA_Launched::UArenasGA_Launched()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;		// 该能力只能在服务器上执行
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;	// 通过GameplayEvent触发
	TriggerData.TriggerTag = ArenasGameplayTags::Ability_Passive_Launch_Activated;	// TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.Passive.Launch.Activated"));
	// TriggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Event.Ability.Uppercut.Launch")); // 也可以用这个
	AbilityTriggers.Add(TriggerData);

	ActivationBlockedTags.RemoveTag(ArenasGameplayTags::Status_Stun); // 眩晕状态下也能触发该能力
}

void UArenasGA_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility()) // 提交能力
	{
		K2_EndAbility(); // 结束能力
		return;
	}

	if (K2_HasAuthority())
	{
		// 获取推力方向（与冲击法线相反）
		FVector PushVelocity = TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal;
		PushSelf(PushVelocity);
		K2_EndAbility();
	}
	
}
