// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_GroundBlast.h"

#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_GroundPick.h"

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
	WaitTargetDataTask->FinishSpawningActor(this, TargetActor);

}

void UArenasGA_GroundBlast::OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("OnTargetConfirmed +++++++++"));
	K2_EndAbility();
}

void UArenasGA_GroundBlast::OnTargetCancelled(const FGameplayAbilityTargetDataHandle& InTargetDataHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("OnTargetCancelled -------------"));
	K2_EndAbility();
}
