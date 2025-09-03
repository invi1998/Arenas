// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_UpperCut.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UArenasGA_UpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayUpperCutMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			UpperCutMontage,
			1.0f,
			NAME_None,
			false,
			0.0f
		);

		PlayUpperCutMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_UpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_UpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_UpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_UpperCut::K2_EndAbility);
		PlayUpperCutMontageTask->ReadyForActivation();
		
	}
	
}
