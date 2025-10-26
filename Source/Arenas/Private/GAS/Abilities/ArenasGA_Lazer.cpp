// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Lazer.h"

#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GAS/ArenasAttributeSet.h"
#include "GAS/TargetActor/TargetActor_Beam.h"

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

void UArenasGA_Lazer::TargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
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

	// 创建等待目标数据任务
	// 首先，进入这段代码的前提是 HasAuthorityOrPredictionKey
	// 因此我们可以确保这是在服务器或拥有预测密钥的客户端（即玩家本身客户端）上执行的，
	// 如果是在其他客户端上执行的，那么这段代码将不会被执行。
	// 如果是服务端，那么它将负责生成目标数据并将其发送给其他客户端。
	// 如果是拥有预测密钥的客户端，那么它将负责生成本地目标数据（因为AGameplayAbilityTargetActor只会将生成结果发给其他客户端，而不会发给自己），

	// EGameplayTargetingConfirmation::CustomMulti 表明数据生成是由自定义逻辑控制的，可以多次生成数据。
	// GameplayTargeting能力负责决定何时目标数据已准备好。并非所有TargetingActors都支持此功能。数据生成时不应被销毁。
	UAbilityTask_WaitTargetData* WaitDamageTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, LazerTargetActorClass);
	WaitDamageTargetDataTask->ValidData.AddDynamic(this, &UArenasGA_Lazer::TargetDataReceived);
	WaitDamageTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitDamageTargetDataTask->BeginSpawningActor(this, LazerTargetActorClass, TargetActor);
	WaitDamageTargetDataTask->FinishSpawningActor(this, TargetActor);

	if (ATargetActor_Beam* TargetActor_Beam = Cast<ATargetActor_Beam>(TargetActor))
	{
		// 将TargetActor附加到技能拥有者的指定Socket上，以确保其位置和旋转与角色同步，SnapToTargetNotIncludingScale用于确保位置和旋转对齐但不影响缩放
		TargetActor_Beam->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LazerTargetActorAttachSocketName);
	}
}
