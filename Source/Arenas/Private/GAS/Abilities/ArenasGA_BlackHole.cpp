// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_BlackHole.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor/TargetActor_GroundPick.h"

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
	
}

void UArenasGA_BlackHole::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	RemoveAimEffect();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UArenasGA_BlackHole::PlaceBlackHole(const FGameplayAbilityTargetDataHandle& Data)
{
}

void UArenasGA_BlackHole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& Data)
{
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
