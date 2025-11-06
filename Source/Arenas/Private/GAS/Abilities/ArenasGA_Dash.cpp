// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Dash.h"

#include "AbilitySystemComponent.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "GAS/TargetActor/TargetActor_Around.h"
#include "GAS/TargetActor/TargetActor_ChargeIndicator.h"

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
		CurrentChargeTime = 0.f;
		// 获取角色移动组件
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			bool bFound = false;
			float MoveSpeed = ASC->GetGameplayAttributeValue(UArenasAttributeSet::GetMoveSpeedAttribute(), bFound);
			float MoveSpeedEX = ASC->GetGameplayAttributeValue(UArenasAttributeSet::GetMoveSpeedExAttribute(), bFound);
			CurrentCharacterMaxMoveSpeed = MoveSpeed + MoveSpeedEX;
		}
		else
		{
			K2_EndAbility();
			return;
		}

		// 获取冲跑所需时间
		DashMontagePlayDuration = DashMontage->GetPlayLength() / DashMontage->RateScale;
		
		UAbilityTask_PlayMontageAndWait* PlayChargeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, DashChargeMontage);
		// PlayChargeMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		// PlayChargeMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		// PlayChargeMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		// PlayChargeMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
		PlayChargeMontageTask->ReadyForActivation();
		
		UAbilityTask_WaitTargetData* WaitChargeTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, ChargeIndicatorTargetActorClass);
		WaitChargeTargetDataTask->ReadyForActivation();

		AGameplayAbilityTargetActor* TargetActor;
		WaitChargeTargetDataTask->BeginSpawningActor(this, ChargeIndicatorTargetActorClass, TargetActor);
		if (ATargetActor_ChargeIndicator* TargetActor_ChargeIndicator = Cast<ATargetActor_ChargeIndicator>(TargetActor))
		{
			ChargeIndicatorTargetActor = TargetActor_ChargeIndicator;
			ChargeIndicatorTargetActor->SetTargetDistance(MinDashDistance);
		}
		WaitChargeTargetDataTask->FinishSpawningActor(this, TargetActor);

		UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this, true);
		WaitInputPressTask->OnPress.AddDynamic(this, &UArenasGA_Dash::StartCharge);
		WaitInputPressTask->ReadyForActivation();
		
		WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
		WaitInputReleaseTask->OnRelease.AddDynamic(this, &UArenasGA_Dash::ChargeCompleted);
		WaitInputReleaseTask->ReadyForActivation();

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
		if (PhasingEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(PhasingEffectHandle);
		}
		if (ChargeEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ChargeEffectHandle);
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

void UArenasGA_Dash::UpdateCurrentDashDistance()
{
	CurrentChargeTime += ChargeTimeInterval;
	UE_LOG(LogTemp, Warning, TEXT("Current Charge Time: %f"), CurrentChargeTime);
	float RealDashTime = CurrentChargeTime * DashMontagePlayDuration / MaxChargeTime;
	UE_LOG(LogTemp, Warning, TEXT("Real Dash Time: %f"), RealDashTime);
	// 根据蓄力时间计算当前冲刺距离
	float CurrentDashDistance = RealDashTime * (MaxDashDistance - MinDashDistance) / DashMontagePlayDuration + MinDashDistance;
	UE_LOG(LogTemp, Warning, TEXT("Current Dash Distance: %f"), CurrentDashDistance);
	if (ChargeIndicatorTargetActor)
	{
		ChargeIndicatorTargetActor->SetTargetDistance(CurrentDashDistance);
	}
	if (CurrentChargeTime >= MaxChargeTime)
	{
		ChargeFinished();
	}
}

void UArenasGA_Dash::ChargeFinished()
{
	if (ChargeIndicatorTargetActor)
	{
		ChargeIndicatorTargetActor->StopAttaching();
	}
	// 玩家达到最大蓄力时间或者主动提前结束蓄力会被调用
	if (UWorld* World = GetWorld())
	{
		if (ChargeTimer.IsValid())
		{
			World->GetTimerManager().ClearTimer(ChargeTimer);
		}
	}
	
	// 如果WaitInputReleaseTask还在等待输入释放，则取消它，防止重复调用
	if (WaitInputReleaseTask)
	{
		WaitInputReleaseTask->EndTask();
	}
	
	// 结束蓄力后，播放冲刺动画
	UAbilityTask_PlayMontageAndWait* PlayDashMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		DashMontage);
	PlayDashMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
	PlayDashMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
	PlayDashMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
	PlayDashMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_Dash::K2_EndAbility);
	PlayDashMontageTask->ReadyForActivation();

	StartDash();
	
	/*
	UAbilityTask_WaitGameplayEvent* WaitSashStartEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Event_Ability_Dash_Start);
	WaitSashStartEventTask->EventReceived.AddDynamic(this, &UArenasGA_Dash::StartDash);
	WaitSashStartEventTask->ReadyForActivation();
	*/
}

void UArenasGA_Dash::StartCharge(float TimeWaited)
{
	if (ChargeEffect)
	{
		ChargeEffectHandle = BP_ApplyGameplayEffectToOwner(ChargeEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}
	
	// 依据蓄力时间，修改角色的冲刺距离
	if (UWorld* World = GetWorld())
	{
		CurrentChargeTime = TimeWaited;
		
		World->GetTimerManager().ClearTimer(ChargeTimer);
		World->GetTimerManager().SetTimer(ChargeTimer, this, &UArenasGA_Dash::UpdateCurrentDashDistance, ChargeTimeInterval, true, TimeWaited);
	}
}

void UArenasGA_Dash::ChargeCompleted(float TimeHeld)
{
	CurrentChargeTime = TimeHeld;
	// 主动提前释放蓄力
	ChargeFinished();
}

void UArenasGA_Dash::StartDash()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ChargeEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ChargeEffectHandle);
		}
	}
	
	if (K2_HasAuthority())
	{
		if (DashEffect)
		{
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DashEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			float RealDashTime = CurrentChargeTime * DashMontagePlayDuration / MaxChargeTime;
			float CurrentDashDistance = RealDashTime * (MaxDashDistance - MinDashDistance) / DashMontagePlayDuration + MinDashDistance;
			float ExNeedSpeed = CurrentDashDistance / DashMontagePlayDuration - CurrentCharacterMaxMoveSpeed;
			UE_LOG(LogTemp, Warning, TEXT("Dash Need Speed: %f, CurrentDashDistance: %f, RealDashTime: %f"), ExNeedSpeed, CurrentDashDistance, RealDashTime);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_DashSpeed, ExNeedSpeed);
			// DashEffectHandle = BP_ApplyGameplayEffectToOwner(DashEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
			DashEffectHandle = ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
		}
	}

	if (PhasingEffect)
	{
		PhasingEffectHandle = BP_ApplyGameplayEffectToOwner(PhasingEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
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
