// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_UpperCut.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

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

	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitUpperCutLaunchEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		ArenasGameplayTags::Event_Ability_Uppercut_Launch,
		nullptr,
		false,
		true
		);

		WaitUpperCutLaunchEventTask->EventReceived.AddDynamic(this, &UArenasGA_UpperCut::OnUpperCutLaunch);
		WaitUpperCutLaunchEventTask->ReadyForActivation();
		
	}
	
}

void UArenasGA_UpperCut::OnUpperCutLaunch(FGameplayEventData Payload)
{
	TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(
		Payload.TargetData,
		ETeamAttitude::Hostile,
		TargetSweepSphereRadius,
		bShowSweepDebug,
		true
	);

	for (FHitResult Hit : HitResults)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DefaultDamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));

		// 添加SetByCaller参数
		EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_BaseDamage, BaseDamage);

		FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
		EffectContext.AddHitResult(Hit);
		EffectSpecHandle.Data->SetContext(EffectContext);
		
		ApplyGameplayEffectSpecToTarget(
			GetCurrentAbilitySpecHandle(),
			GetCurrentActorInfo(),
			GetCurrentActivationInfo(),
			EffectSpecHandle,
			UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Hit.GetActor()));
	}
	
}

