// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasCombo_GameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

UArenasCombo_GameplayAbility::UArenasCombo_GameplayAbility() : ComboMontage(nullptr)
{
	FGameplayTagContainer TempAbilityTags;
	TempAbilityTags.AddTag(ArenasGameplayTags::Ability_BasicAttack);
	SetAssetTags(TempAbilityTags);
	BlockAbilitiesWithTag.AddTag(ArenasGameplayTags::Ability_BasicAttack);
}

void UArenasCombo_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
		PlayComboMontageAndWaitTask->OnBlendOut.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		// 订阅蒙太奇完成事件
		PlayComboMontageAndWaitTask->OnCompleted.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		// 订阅蒙太奇中断事件
		PlayComboMontageAndWaitTask->OnInterrupted.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		// 订阅蒙太奇取消事件
		PlayComboMontageAndWaitTask->OnCancelled.AddDynamic(this, &UArenasCombo_GameplayAbility::K2_EndAbility);
		
		PlayComboMontageAndWaitTask->ReadyForActivation();	// 准备好被激活（实际并未被激活）

		// 等待动画通知事件（AnimNotify Event）
		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			GetComboChangeEventTag(),
			nullptr,
			false,	// 因为连招会被多次触发，所以这里不需要只触发一次
			false);	// 同时，我们这里监听的是Combo.Change，它下面有多个具体的连招段Tag，所以这里不需要只匹配精确标签

		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UArenasCombo_GameplayAbility::OnComboChangeEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}

	// 伤害事件
	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitComboDamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			ArenasGameplayTags::Event_Ability_Combo_Damage,
			nullptr,
			false,
			true);

		WaitComboDamageEventTask->EventReceived.AddDynamic(this, &UArenasCombo_GameplayAbility::DoDamage);
		WaitComboDamageEventTask->ReadyForActivation();
	}
	

	SetupWaitComboInputPressTask();
}

FGameplayTag UArenasCombo_GameplayAbility::GetComboChangeEventTag()
{
	return FGameplayTag::RequestGameplayTag(FName("Event.Ability.Combo.Change"));
}

void UArenasCombo_GameplayAbility::SetupWaitComboInputPressTask()
{
	UAbilityTask_WaitInputPress* WaitComboInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitComboInputPressTask->OnPress.AddDynamic(this, &UArenasCombo_GameplayAbility::OnComboInputPressed);
	WaitComboInputPressTask->ReadyForActivation();
	
}

void UArenasCombo_GameplayAbility::TryCommitCombo()
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

TSubclassOf<UGameplayEffect> UArenasCombo_GameplayAbility::GetCurrentComboDamageEffect() const
{
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage);
		if (DamageEffectsMap.Contains(CurrentSectionName))
		{
			return DamageEffectsMap[CurrentSectionName];
		}
	}

	return DefaultDamageEffect;
}

int32 UArenasCombo_GameplayAbility::GetCurrentComboIndex() const
{
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage);
		return FCString::Atoi(*CurrentSectionName.ToString());
	}
	return 0; // 默认返回0，表示无效索引
}

void UArenasCombo_GameplayAbility::OnComboChangeEventReceived(FGameplayEventData PayloadData)
{
	FGameplayTag EventTag = PayloadData.EventTag;

	if (EventTag == ArenasGameplayTags::Event_Ability_Combo_Change_End)
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("Combo Ended"));
		return;
	}

	NextComboName = UArenasBlueprintFunctionLibrary::NativeGetGameplayTagLastName(EventTag);
	UE_LOG(LogTemp, Warning, TEXT("Next Combo: %s"), *NextComboName.ToString());
	
}

void UArenasCombo_GameplayAbility::OnComboInputPressed(float TimeWaited)
{
	SetupWaitComboInputPressTask();
	TryCommitCombo();
}

void UArenasCombo_GameplayAbility::DoDamage(FGameplayEventData Payload)
{
	const TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Payload.TargetData, ETeamAttitude::Hostile, TargetSweepSphereRadius, false, true);

	float CurrentComboIndex = GetCurrentComboIndex();
	
	for (const FHitResult& Hit : HitResults)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GetCurrentComboDamageEffect(), GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));

		// 添加SetByCaller参数
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
