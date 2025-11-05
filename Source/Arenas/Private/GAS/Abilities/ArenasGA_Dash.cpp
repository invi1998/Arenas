// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Dash.h"

#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/TargetActor/TargetActor_Around.h"

void UArenasGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility() || !DashMontage)
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayDashMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DashMontage);
		PlayDashMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		PlayDashMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		PlayDashMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		PlayDashMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		PlayDashMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitSashStartEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Event_Ability_Dash_Start);
		WaitSashStartEventTask->EventReceived.AddDynamic(this, &UArenasGA_Dash::StartDash);
		WaitSashStartEventTask->ReadyForActivation();

	}

	
}

void UArenasGA_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (DashEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(DashEffectHandle);
		}
	}

	if (PushForwardInputTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PushForwardInputTimerHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UArenasGA_Dash::PushForward()
{
	if (OwnerCharacterMovementComponent)
	{
		FVector ForwardVector = GetAvatarActorFromActorInfo()->GetActorForwardVector();
		OwnerCharacterMovementComponent->AddInputVector(ForwardVector);
		// 下一帧继续推，直到能力结束，以达到持续冲刺的效果
		PushForwardInputTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UArenasGA_Dash::PushForward);
	}
}

void UArenasGA_Dash::StartDash(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		if (DashEffect)
		{
			DashEffectHandle = BP_ApplyGameplayEffectToOwner(DashEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			
		}
	}

	if (IsLocallyControlled())
	{
		PushForwardInputTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UArenasGA_Dash::PushForward);
		OwnerCharacterMovementComponent = GetAvatarActorFromActorInfo()->GetComponentByClass<UCharacterMovementComponent>();
	}

	UAbilityTask_WaitTargetData* WaitDashTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, DashTargetActorClass);
	WaitDashTargetDataTask->ValidData.AddDynamic(this, &UArenasGA_Dash::TargetReceived);
	WaitDashTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitDashTargetDataTask->BeginSpawningActor(this, DashTargetActorClass, TargetActor);
	ATargetActor_Around* DashTargetActor = Cast<ATargetActor_Around>(TargetActor);
	if (DashTargetActor)
	{
		DashTargetActor->ConfigureDetection(TargetDetectionRadius, GetOwningTeamId(), LocalGameplayCueTag);
	}
	WaitDashTargetDataTask->FinishSpawningActor(this, TargetActor);
	if (DashTargetActor)
	{
		// 附加到角色组件上
		DashTargetActor->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetActorAttachSocketName);
	}

	
}

void UArenasGA_Dash::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority())
	{
		ApplyAbilityMagicGameplayEffectToTarget(TargetDataHandle, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		PushTargets(TargetDataHandle, TargetHitPushSpeed);
	}
}
