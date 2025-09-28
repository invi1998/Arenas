// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_UpperCut.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasGameplayTags.h"
#include "ArenasGA_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UArenasGA_UpperCut::UArenasGA_UpperCut()
{
	BlockAbilitiesWithTag.AddTag(ArenasGameplayTags::Ability_BasicAttack);	// 阻止基础攻击技能
}

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

	NextComboName = NAME_None;
	
}

const FGenericDamageEffectData* UArenasGA_UpperCut::GetComboDamageEffectDataByName() const
{
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage);
		if (const FGenericDamageEffectData* FoundData = ComboDamageEffectDataMap.Find(CurrentSectionName))
		{
			return FoundData;
		}
	}
	return nullptr;
}

void UArenasGA_UpperCut::OnUpperCutLaunch(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(
			Payload.TargetData,
			ETeamAttitude::Hostile,
			TargetSweepSphereRadius,
			bShowSweepDebug,
			true
		);

		// 自己向上击飞
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * LaunchStrength_Self);

		for (FHitResult Hit : HitResults)
		{
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DefaultDamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));

			// 添加SetByCaller参数
			float LaunchedDamage = LaunchDamage.GetValueAtLevel(GetAbilityLevel());
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_BaseDamage, LaunchedDamage);

			FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
			EffectContext.AddHitResult(Hit);
			EffectSpecHandle.Data->SetContext(EffectContext);
		
			ApplyGameplayEffectSpecToTarget(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActorInfo(),
				GetCurrentActivationInfo(),
				EffectSpecHandle,
				UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Hit.GetActor()));

			// 向上击飞敌人
			PushTarget(Hit.GetActor(), FVector::UpVector * LaunchStrength_Target);
		
		}
	}
	
	UAbilityTask_WaitGameplayEvent* WaitComboChangeEndEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		UArenasGA_Combo::GetComboChangeEventTag(),
		nullptr,
		false,
		false
	);
	WaitComboChangeEndEventTask->EventReceived.AddDynamic(this, &UArenasGA_UpperCut::HandleComboChange);
	WaitComboChangeEndEventTask->ReadyForActivation();

	// 然后监听基础攻击
	UAbilityTask_WaitGameplayEvent* WaitComboCommitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		ArenasGameplayTags::Ability_BasicAttack_Pressed,
		nullptr
	);
	WaitComboCommitEventTask->EventReceived.AddDynamic(this, &UArenasGA_UpperCut::HandleComboCommitEvent);
	WaitComboCommitEventTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboDamageEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		ArenasGameplayTags::Event_Ability_Combo_Damage
	);
	WaitComboDamageEventTask->EventReceived.AddDynamic(this, &UArenasGA_UpperCut::HandleComboDamageEvent);
	WaitComboDamageEventTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboFinalBlowEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		ArenasGameplayTags::Event_Ability_Uppercut_FinalBlow,
		nullptr,
		false,
		false
	);
	WaitComboFinalBlowEventTask->EventReceived.AddDynamic(this, &UArenasGA_UpperCut::HandleUpperCutFinalBlow);
	WaitComboFinalBlowEventTask->ReadyForActivation();
	
}

void UArenasGA_UpperCut::HandleComboCommitEvent(FGameplayEventData Payload)
{
	if (NextComboName == NAME_None)
	{
		return;
	}

	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage), NextComboName, UpperCutMontage);
	}
}

void UArenasGA_UpperCut::HandleComboChange(FGameplayEventData Payload)
{
	FGameplayTag EventTag = Payload.EventTag;
	if (EventTag.MatchesTagExact(ArenasGameplayTags::Event_Ability_Combo_Change_End))
	{
		NextComboName = NAME_None;
		return;
	}

	TArray<FName> ComboNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, ComboNames);
	NextComboName = ComboNames.Num() > 0 ? ComboNames.Last() : NAME_None;
	
}

int32 UArenasGA_UpperCut::GetCurrentComboIndex() const
{
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage);
		return FCString::Atoi(*CurrentSectionName.ToString());
	}
	return 0; // 默认返回0，表示无效索引
}

void UArenasGA_UpperCut::HandleComboDamageEvent(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(
			Payload.TargetData,
			ETeamAttitude::Hostile,
			TargetSweepSphereRadius,
			bShowSweepDebug,
			true
		);

		// 自己向上击飞
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * LaunchStrength_Air);
		int32 ComboIndex = GetCurrentComboIndex();
		
		for (FHitResult Hit : HitResults)
		{
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DefaultDamageEffect, GetAbilityLevel(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()));

			// 添加SetByCaller参数
			float ComboDamageTemp = ComboDamage.GetValueAtLevel(GetAbilityLevel());
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_BaseDamage, ComboDamageTemp);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_ComboIndex, ComboIndex);

			FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
			EffectContext.AddHitResult(Hit);
			EffectSpecHandle.Data->SetContext(EffectContext);
		
			ApplyGameplayEffectSpecToTarget(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActorInfo(),
				GetCurrentActivationInfo(),
				EffectSpecHandle,
				UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Hit.GetActor()));

			PushTarget(Hit.GetActor(), FVector::UpVector * LaunchStrength_Air);
		
		}
	}
}

void UArenasGA_UpperCut::HandleUpperCutFinalBlow(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
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
			float FinalBlowDamageTemp = FinalBlowDamage.GetValueAtLevel(GetAbilityLevel());
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_BaseDamage, FinalBlowDamageTemp);
			
			FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
			EffectContext.AddHitResult(Hit);
			EffectSpecHandle.Data->SetContext(EffectContext);
		
			ApplyGameplayEffectSpecToTarget(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActorInfo(),
				GetCurrentActivationInfo(),
				EffectSpecHandle,
				UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Hit.GetActor()));

			FVector PushVelocity = (Hit.GetActor()->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).GetSafeNormal();

			PushTarget(Hit.GetActor(), PushVelocity * LaunchStrength_Target);
		
		}
	}
}

