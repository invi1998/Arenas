// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_FallingStar.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor/TargetActor_GroundPick.h"
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
		UE_LOG(LogTemp, Warning, TEXT(" -- UArenasGA_FallingStar::ActivateAbility: IsServer = %s"), K2_HasAuthority() ? TEXT("True") : TEXT("False"));
	
		K2_EndAbility();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT(" ++ UArenasGA_FallingStar::ActivateAbility: IsServer = %s"), K2_HasAuthority() ? TEXT("True") : TEXT("False"));
	

	UAbilityTask_PlayMontageAndWait* PlayFallingStarMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, CastingMontage);
	PlayFallingStarMontageTask->OnCompleted.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->OnInterrupted.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->OnCancelled.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->OnBlendOut.AddDynamic(this, &UArenasGA_FallingStar::K2_EndAbility);
	PlayFallingStarMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, GroundPickTargetActorClass);
	WaitPlacementTask->ValidData.AddDynamic(this, &UArenasGA_FallingStar::PlaceFallingStar);
	WaitPlacementTask->Cancelled.AddDynamic(this, &UArenasGA_FallingStar::PlacementCancelled);
	WaitPlacementTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitPlacementTask->BeginSpawningActor(this, GroundPickTargetActorClass, TargetActor);

	if (ATargetActor_GroundPick* TargetActor_GroundPick = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		TargetActor_GroundPick->SetShouldDrawDebugSphere(bShowDebugLine);
		TargetActor_GroundPick->SetTargetAreaRadius(AOERadius);
		TargetActor_GroundPick->SetTargetTraceDistance(TargetRange);
	}
	WaitPlacementTask->FinishSpawningActor(this, TargetActor);

}

void UArenasGA_FallingStar::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UArenasGA_FallingStar::PlaceFallingStar(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	// 星落生成
	WaitFallingStarTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, FallingStarTargetClass);
	WaitFallingStarTargetDataTask->ValidData.AddDynamic(this, &UArenasGA_FallingStar::OnFallingStarTargetDataReceived);
	WaitFallingStarTargetDataTask->Cancelled.AddDynamic(this, &UArenasGA_FallingStar::OnFallingStarTargetCancelled);
	WaitFallingStarTargetDataTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitFallingStarTargetDataTask->BeginSpawningActor(this, FallingStarTargetClass, TargetActor);
	ATargetActor_FallingStar* FallingStarTargetActor = Cast<ATargetActor_FallingStar>(TargetActor);
	if (FallingStarTargetActor)
	{
		FallingStarTargetActor->ConfigureFallingStarTargetActor(
			AOERadius,
			LoopDuration.GetValueAtLevel(GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)),
			SpawnRate.GetValueAtLevel(GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)),
			SpawnHeight,
			FallingSpeed,
			GetOwningTeamId());
	}
	WaitFallingStarTargetDataTask->FinishSpawningActor(this, TargetActor);
	if (FallingStarTargetActor)
	{
		// 在角色位置绘制调试球体
		const FVector TargetPoint = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(Data, 1).ImpactPoint;
		DrawDebugSphere(GetWorld(), TargetPoint, AOERadius, 32, FColor::Red, false, -1.f, 0, 2.f);
		FallingStarTargetActor->SetActorLocation(TargetPoint);
	}
	
	
}

void UArenasGA_FallingStar::PlacementCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	K2_EndAbility();
}

void UArenasGA_FallingStar::OnFallingStarTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (K2_HasAuthority())
	{
		BP_ApplyGameplayEffectToTarget(Data, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));

		// 给目标施加灼烧特效

	}
}

void UArenasGA_FallingStar::OnFallingStarTargetCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
	OnFallingStarTargetDataReceived(Data);
}


