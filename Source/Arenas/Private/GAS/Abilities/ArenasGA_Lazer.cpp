// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Lazer.h"

#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GAS/ArenasAttributeSet.h"

void UArenasGA_Lazer::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !LazerMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayLazerMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LazerMontage);
		PlayLazerMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_Lazer::K2_EndAbility);
		PlayLazerMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitLazerEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Event_Ability_Lazer_Fire);
		WaitLazerEventTask->EventReceived.AddDynamic(this, &UArenasGA_Lazer::OnLazerFireEventReceived);
		WaitLazerEventTask->ReadyForActivation();

		UAbilityTask_WaitCancel* WaitCancelTask = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancelTask->OnCancel.AddDynamic(this, &UArenasGA_Lazer::K2_EndAbility);
		WaitCancelTask->ReadyForActivation();
		
	}
	
}

void UArenasGA_Lazer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC && ActiveOngoingConsumeEffectHandle.IsValid())
	{
		OwnerASC->RemoveActiveGameplayEffect(ActiveOngoingConsumeEffectHandle);
		ActiveOngoingConsumeEffectHandle.Invalidate();		// 使句柄无效

		OwnerASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).RemoveAll(this);
	}
	
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UArenasGA_Lazer::ManaUpdate(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC && !OwnerASC->CanApplyAttributeModifiers(OnGoingConsumeEffect.GetDefaultObject(), GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo), MakeEffectContext(CurrentSpecHandle, CurrentActorInfo)))
	{
		// 如果存在ASC，但无法再应用持续消耗效果，则结束技能
		K2_EndAbility();
		return;
	}
}

void UArenasGA_Lazer::OnLazerFireEventReceived(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		ActiveOngoingConsumeEffectHandle = BP_ApplyGameplayEffectToOwner(OnGoingConsumeEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

		if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
		{
			OwnerASC->GetGameplayAttributeValueChangeDelegate(UArenasAttributeSet::GetManaAttribute()).AddUObject(this, &UArenasGA_Lazer::ManaUpdate);
		}
		
	}
}
