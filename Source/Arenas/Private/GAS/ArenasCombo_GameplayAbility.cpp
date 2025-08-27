// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasCombo_GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UArenasCombo_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	// 等价于
	if (!K2_CommitAbility())
	{
		// EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		// 等价于
		K2_EndAbility();
		return;
	}

	// 在多人游戏中，我们需要确保蒙太奇在服务器上播放，然后再通过复制将其同步到客户端
	// HasAuthorityOrPredictionKey 会检查我们是否在服务器上，或者我们是否拥有预测密钥（客户端预测即本地客户端立即执行）
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageAndWaitTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage, 1.f, NAME_None, false);
		// 订阅蒙太奇淡出混合事件
		PlayComboMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		// 订阅蒙太奇完成事件
		PlayComboMontageAndWaitTask->OnCompleted.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		// 订阅蒙太奇中断事件
		PlayComboMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		// 订阅蒙太奇取消事件
		PlayComboMontageAndWaitTask->OnCancelled.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		
		PlayComboMontageAndWaitTask->ReadyForActivation();	// 准备好被激活（实际并未被激活）
	}
}
