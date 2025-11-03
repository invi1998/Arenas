// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_BlackHole.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ArenasBlueprintFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor/TargetActor_GroundPick.h"
#include "GAS/TargetActor/TargetActor_BlackHole.h"

void UArenasGA_BlackHole::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	PlayCastBlackHoleMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayCastBlackHoleMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
	PlayCastBlackHoleMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, GroundPickTargetActorClass);
	WaitPlacementTask->ValidData.AddDynamic(this, &UArenasGA_BlackHole::PlaceBlackHole);
	WaitPlacementTask->Cancelled.AddDynamic(this, &UArenasGA_BlackHole::PlacementCancelled);
	WaitPlacementTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitPlacementTask->BeginSpawningActor(this, GroundPickTargetActorClass, TargetActor);

	if (ATargetActor_GroundPick* TargetActor_GroundPick = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		TargetActor_GroundPick->SetShouldDrawDebugSphere(bShowDebugLine);
		TargetActor_GroundPick->SetTargetAreaRadius(TargetAreaRadius);
		TargetActor_GroundPick->SetTargetTraceDistance(TargetTraceRange);
	}
	WaitPlacementTask->FinishSpawningActor(this, TargetActor);
	AddAimEffect();
}

void UArenasGA_BlackHole::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	RemoveAimEffect();
	RemoveFocusedEffect();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UArenasGA_BlackHole::BlackHoleTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	// 伤害和推开效果
	if (K2_HasAuthority())
	{
		ApplyAbilityMagicGameplayEffectToTarget(Data, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		// 推开命中的目标，以黑洞中心为基准点
		const FVector HoleCenter = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 1).ImpactPoint;
		PushTargets(Data, HoleCenter, HitPushSpeed);

		UAbilityTask_PlayMontageAndWait* PlayFinalBlowMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalBlowMontage);
		PlayFinalBlowMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayFinalBlowMontageTask->ReadyForActivation();
		
		
	}
	else
	{
		PlayMontageLocally(FinalBlowMontage);
	}

	FGameplayCueParameters CueParams;
	CueParams.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 1).ImpactPoint;
	CueParams.RawMagnitude = TargetAreaRadius;
	if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
	{
		OwnerASC->ExecuteGameplayCue(BlackHoleExplodeCueTag, CueParams);
		OwnerASC->ExecuteGameplayCue(UArenasBlueprintFunctionLibrary::GetCameraShakeGameplayCueTag(), CueParams);
	}
	
}

void UArenasGA_BlackHole::BlackHoleTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	// 提前结束黑洞效果，引爆
	BlackHoleTargetDataReceived(Data);
}

void UArenasGA_BlackHole::PlaceBlackHole(const FGameplayAbilityTargetDataHandle& Data)
{

	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}
	
	RemoveAimEffect();
	AddFocusedEffect();
	
	if (PlayCastBlackHoleMontageTask)
	{
		PlayCastBlackHoleMontageTask->OnCompleted.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnInterrupted.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnCancelled.RemoveAll(this);
		PlayCastBlackHoleMontageTask->OnBlendOut.RemoveAll(this);
	}

	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		// 由于PlaceBlackHole是由玩家输入触发，而不是GameplayEvent，所以通常来说因该会有一个PredictionKey，如果没有的话，就需要手动指定同步键
		UAbilityTask_PlayMontageAndWait* PlayBlackHoleMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoldBlackHoleMontage);
		PlayBlackHoleMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayBlackHoleMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayBlackHoleMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayBlackHoleMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_BlackHole::K2_EndAbility);
		PlayBlackHoleMontageTask->ReadyForActivation();
	}

	BlackHoleTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	BlackHoleTargetDataTask->ValidData.AddDynamic(this, &UArenasGA_BlackHole::BlackHoleTargetDataReceived);
	// cancel 事件通常是玩家希望提前结束TargetActor，比如提前引爆黑洞照成爆炸伤害和推开单位
	BlackHoleTargetDataTask->Cancelled.AddDynamic(this, &UArenasGA_BlackHole::BlackHoleTargetDataCancelled);
	BlackHoleTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	BlackHoleTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_BlackHole* TargetActor_BlackHole = Cast<ATargetActor_BlackHole>(TargetActor);
	if (TargetActor_BlackHole)
	{
		TargetActor_BlackHole->ConfigureBlackHoleTargetActor(TargetAreaRadius, PullSpeed, BlackHoleDuration, GetOwningTeamId());
	}
	BlackHoleTargetDataTask->FinishSpawningActor(this, TargetActor);
	if (TargetActor_BlackHole)
	{
		// 设置黑洞位置，黑洞位置由玩家选择的地面位置决定
		const FVector TargetPoint = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 1).ImpactPoint;
		TargetActor_BlackHole->SetActorLocation(TargetPoint);
	}
	
	
}

void UArenasGA_BlackHole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	K2_EndAbility();
}

void UArenasGA_BlackHole::AddAimEffect()
{
	if (!AimEffectClass) return;
	ActiveAimGameplayEffectHandle = BP_ApplyGameplayEffectToOwner(AimEffectClass);
}

void UArenasGA_BlackHole::RemoveAimEffect()
{
	if (ActiveAimGameplayEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveAimGameplayEffectHandle);	
	}
}

void UArenasGA_BlackHole::AddFocusedEffect()
{
	if (!FocusedEffectClass) return;
	ActiveFocusedGameplayEffectHandle = BP_ApplyGameplayEffectToOwner(FocusedEffectClass);
}

void UArenasGA_BlackHole::RemoveFocusedEffect()
{
	if (ActiveFocusedGameplayEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(ActiveFocusedGameplayEffectHandle);	
	}
}
