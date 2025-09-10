// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_GroundBlast.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UArenasGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &ActivationInfo))
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GroundBlastMontage, 1.f, NAME_None, false);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->ReadyForActivation();
}
