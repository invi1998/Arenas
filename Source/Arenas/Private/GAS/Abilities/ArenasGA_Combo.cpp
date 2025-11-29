// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasGA_Combo.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

UArenasGA_Combo::UArenasGA_Combo() : ComboMontage(nullptr)
{
	FGameplayTagContainer TempAbilityTags;
	TempAbilityTags.AddTag(ArenasGameplayTags::Ability_BasicAttack);
	SetAssetTags(TempAbilityTags);
	BlockAbilitiesWithTag.AddTag(ArenasGameplayTags::Ability_BasicAttack);
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

FGameplayTag UArenasGA_Combo::GetComboChangeEventTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Event.Ability.Combo.Change"));
}

void UArenasGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		PlayComboMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UArenasGA_Combo::K2_EndAbility);
		// 订阅蒙太奇完成事件
		PlayComboMontageAndWaitTask->OnCompleted.AddDynamic(this, &UArenasGA_Combo::K2_EndAbility);
		// 订阅蒙太奇中断事件
		PlayComboMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UArenasGA_Combo::K2_EndAbility);
		// 订阅蒙太奇取消事件
		PlayComboMontageAndWaitTask->OnCancelled.AddDynamic(this, &UArenasGA_Combo::K2_EndAbility);
		
		PlayComboMontageAndWaitTask->ReadyForActivation();	// 准备好被激活（实际并未被激活）

		// 等待动画通知事件（AnimNotify Event）
		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetComboChangeEventTag(),
			nullptr,
			false,	// 因为连招会被多次触发，所以这里不需要只触发一次
			false);	// 同时，我们这里监听的是Combo.Change，它下面有多个具体的连招段Tag，所以这里不需要只匹配精确标签

		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UArenasGA_Combo::OnComboChangeEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	// 伤害事件
	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitComboDamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetDamageEventTag(),
			nullptr,
			false,
			true);

		WaitComboDamageEventTask->EventReceived.AddDynamic(this, &UArenasGA_Combo::DoDamage);
		WaitComboDamageEventTask->ReadyForActivation();
	}
	
	NextComboName = NAME_None;
	SetupWaitComboInputPressTask();
}

FGameplayTag UArenasGA_Combo::GetDamageEventTag() const
{
	return ArenasGameplayTags::Event_Ability_Combo_Damage;
}

void UArenasGA_Combo::SetupWaitComboInputPressTask()
{
	UAbilityTask_WaitInputPress* WaitComboInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitComboInputPressTask->OnPress.AddDynamic(this, &UArenasGA_Combo::OnComboInputPressed);
	WaitComboInputPressTask->ReadyForActivation();
	
}

void UArenasGA_Combo::TryCommitCombo()
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
		
	}
	
}

int32 UArenasGA_Combo::GetCurrentComboIndex() const
{
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage);
		return FCString::Atoi(*CurrentSectionName.ToString());
	}
	return 0; // 默认返回0，表示无效索引
}

void UArenasGA_Combo::OnComboChangeEventReceived(FGameplayEventData PayloadData)
{
	FGameplayTag EventTag = PayloadData.EventTag;

	if (EventTag == ArenasGameplayTags::Event_Ability_Combo_Change_End)
	{
		NextComboName = NAME_None;
		return;
	}

	NextComboName = UArenasBlueprintFunctionLibrary::NativeGetGameplayTagLastName(EventTag);
	
}

void UArenasGA_Combo::OnComboInputPressed(float TimeWaited)
{
	SetupWaitComboInputPressTask();
	TryCommitCombo();
}

void UArenasGA_Combo::DoDamage(FGameplayEventData Payload)
{
	int32 HitResultsCount = UAbilitySystemBlueprintLibrary::GetDataCountFromTargetData(Payload.TargetData);
	
	
	float CurrentComboIndex = GetCurrentComboIndex();
	
	for (int32 i = 0; i < HitResultsCount; i++)
	{

		FHitResult Hit = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Payload.TargetData, i);
		
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DefaultDamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));

		// 设置发起者（Instigator）为当前Ability的拥有者

		// 添加SetByCaller参数
		EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_BaseDamage, BaseDamage);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_ComboIndex, CurrentComboIndex);

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
