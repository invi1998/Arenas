// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_GroundBlast.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor/TargetActor_GroundPick.h"

UArenasGA_GroundBlast::UArenasGA_GroundBlast()
{
	ActivationOwnedTags.AddTag(ArenasGameplayTags::Status_Aiming);
	BlockAbilitiesWithTag.AddTag(ArenasGameplayTags::Ability_BasicAttack);
}

void UArenasGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &ActivationInfo))
	{
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GroundBlastMontage, 1.f, NAME_None, false);
	PlayMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_GroundBlast::K2_EndAbility);
	PlayMontageTask->ReadyForActivation();

	// 使用AbilityTask_WaitTargetData来处理目标选择
	UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, GroundPickTargetClass);
	WaitTargetDataTask->ValidData.AddDynamic(this, &UArenasGA_GroundBlast::OnTargetConfirmed);	// 目标数据准备好时调用
	WaitTargetDataTask->Cancelled.AddDynamic(this, &UArenasGA_GroundBlast::OnTargetCancelled);	// 目标选择被取消时调用
	WaitTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor = nullptr;
	WaitTargetDataTask->BeginSpawningActor(this, GroundPickTargetClass, TargetActor);

	if (ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		GroundPickTargetActor->SetShouldDrawDebugSphere(bShowSweepDebug);
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickTargetActor->SetTargetTraceDistance(TargetTraceDistance);
		GroundPickTargetActor->SetTargetOptions(bShouldTargetEnemies, bShouldTargetAllies);
	}
	
	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);

}

void UArenasGA_GroundBlast::OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	if (K2_HasAuthority())
	{
		// BP_ApplyGameplayEffectToTarget(InTargetDataHandle, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(InTargetDataHandle);
	
		PushTargets(TargetActors, PushVelocity);
		
		for (AActor* TargetActor : TargetActors)
		{
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

			float DamageValue = BlastDamage.GetValueAtLevel(GetAbilityLevel());
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ArenasGameplayTags::SetByCaller_BaseDamage, DamageValue);

			ApplyGameplayEffectSpecToTarget(
				GetCurrentAbilitySpecHandle(),
				GetCurrentActorInfo(),
				GetCurrentActivationInfo(),
				EffectSpecHandle,
				UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetActor));
		}
	}
	
	FGameplayCueParameters BlastingCueParameters;
	// 这参数1是因为我们在InTargetDataHandle里传递了两组数据信息，第一组是技能作用目标，第二组是施法位置
	BlastingCueParameters.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(InTargetDataHandle, 1).ImpactPoint;
	// 我们希望在施法位置生成一个火焰升腾特效，同时，特效的规模与技能作用半径成正比，所以我们把技能作用半径也传递给特效
	BlastingCueParameters.RawMagnitude = TargetAreaRadius;

	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(BlastGameplayCueTag, BlastingCueParameters);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(UArenasBlueprintFunctionLibrary::GetCameraShakeGameplayCueTag(), BlastingCueParameters);
	
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (OwnerAnimInstance && CastBlastMontage)
	{
		OwnerAnimInstance->Montage_Play(CastBlastMontage);
	}
	
	K2_EndAbility();
}

void UArenasGA_GroundBlast::OnTargetCancelled(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		// 取消目标选择时停止播放施法动画
		OwnerAnimInstance->Montage_Stop(0.2f, GroundBlastMontage);
	}
	
	K2_EndAbility();
}
