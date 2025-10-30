// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_FallingStar.h"

#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor/TargetActor_FallingStar.h"

UArenasGA_FallingStar::UArenasGA_FallingStar()
{
	ActivationOwnedTags.AddTag(ArenasGameplayTags::Status_Focus);
}

void UArenasGA_FallingStar::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayFallingStarMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastingMontage);
	PlayFallingStarMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitFallingStarTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, FallingStarTargetClass);
	WaitFallingStarTargetDataTask->ValidData.AddDynamic(this, &UArenasGA_FallingStar::OnFallingStarTargetDataReceived);
	WaitFallingStarTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitFallingStarTargetDataTask->BeginSpawningActor(this, FallingStarTargetClass, TargetActor);
	ATargetActor_FallingStar* FallingStarTargetActor = Cast<ATargetActor_FallingStar>(TargetActor);
	if (FallingStarTargetActor)
	{
		FallingStarTargetActor->ConfigureFallingStarTargetActor(AOERadius, LoopDuration.GetValueAtLevel(GetAbilityLevel(Handle, ActorInfo)), SpawnRate.GetValueAtLevel(GetAbilityLevel(Handle, ActorInfo)), SpawnHeight, FallingSpeed, GetOwningTeamId());
	}
	WaitFallingStarTargetDataTask->FinishSpawningActor(this, TargetActor);
	if (FallingStarTargetActor)
	{
		FallingStarTargetActor->SetActorLocation(GetAvatarActorFromActorInfo()->GetActorLocation());
	}

}

void UArenasGA_FallingStar::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UArenasGA_FallingStar::OnFallingStarTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(Data, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

		// 给目标施加灼烧特效

	}
}


