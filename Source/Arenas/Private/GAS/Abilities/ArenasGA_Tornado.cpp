// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Tornado.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

void UArenasGA_Tornado::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		UAbilityTask_PlayMontageAndWait* PlayTornadoMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TornadoMontage);
		PlayTornadoMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_Tornado::K2_EndAbility);
		PlayTornadoMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_Tornado::K2_EndAbility);
		PlayTornadoMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_Tornado::K2_EndAbility);
		PlayTornadoMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_Tornado::K2_EndAbility);
		PlayTornadoMontageTask->ReadyForActivation();

		if (K2_HasAuthority())
		{
			UAbilityTask_WaitGameplayEvent* WaitTornadoDamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
				this,
				ArenasGameplayTags::Event_Generic_Damage,
				nullptr,
				false,
				true);

			WaitTornadoDamageEventTask->EventReceived.AddDynamic(this, &UArenasGA_Tornado::TornadoDamageEventReceived);
			WaitTornadoDamageEventTask->ReadyForActivation();
			
		}

		UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancel->OnCancel.AddDynamic(this, &UArenasGA_Tornado::K2_EndAbility);
		WaitCancel->ReadyForActivation();

		float TornadoDurationValue = TornadoDuration.GetValueAtLevel(GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		UAbilityTask_WaitDelay* WaitTornadoTimeout = UAbilityTask_WaitDelay::WaitDelay(this, TornadoDurationValue);
		WaitTornadoTimeout->OnFinish.AddDynamic(this, &UArenasGA_Tornado::K2_EndAbility);
		WaitTornadoTimeout->ReadyForActivation();
		
	}
	
}

void UArenasGA_Tornado::TornadoDamageEventReceived(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		ApplyAbilityMagicGameplayEffectToTarget(Payload.TargetData, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		PushTargets(Payload.TargetData, HitPushSpeed);
	}
}
