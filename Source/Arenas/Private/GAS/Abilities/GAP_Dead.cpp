// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GAP_Dead.h"

#include "ArenasGameplayTags.h"

UGAP_Dead::UGAP_Dead()
{

	ActivationBlockedTags.RemoveTag(ArenasGameplayTags::Status_Stun);
	ActivationBlockedTags.RemoveTag(ArenasGameplayTags::Status_Dead);
	
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;		// 仅在服务器上执行
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;		// 每个角色实例化
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = ArenasGameplayTags::Status_Dead;
	AbilityTriggers.Add(TriggerData);

}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority())
	{
		if (AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser())
		{
			UE_LOG(LogTemp, Warning, TEXT("****** UGAP_Dead::ActivateAbility Killer: %s"), *Killer->GetName());
		}
	}
	
	
}
