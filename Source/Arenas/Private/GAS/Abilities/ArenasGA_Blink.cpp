// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Blink.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor/TargetActor_GroundPick.h"

UArenasGA_Blink::UArenasGA_Blink()
{
	ActivationOwnedTags.AddTag(ArenasGameplayTags::Status_Aiming);
}

void UArenasGA_Blink::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		K2_EndAbility();
		return;
	}

	UAbilityTask_PlayMontageAndWait* PlayTargetingMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayTargetingMontage->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitBlinkLocationTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, GroundPickTargetActorClass);
	WaitBlinkLocationTargetData->ValidData.AddDynamic(this, &UArenasGA_Blink::GroundPickTargetReceived);
	WaitBlinkLocationTargetData->Cancelled.AddDynamic(this, &UArenasGA_Blink::GroundPickCancelled);
	WaitBlinkLocationTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitBlinkLocationTargetData->BeginSpawningActor(this, GroundPickTargetActorClass, TargetActor);
	if (ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor))
	{
		GroundPickTargetActor->SetShouldDrawDebugSphere(bShowDebugLine);
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickTargetActor->SetTargetTraceDistance(BlinkDistance);
	}

	WaitBlinkLocationTargetData->FinishSpawningActor(this, TargetActor);
}

void UArenasGA_Blink::GroundPickTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	BlinkTargetDataHandle = TargetDataHandle;
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayTeleportMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TeleportMontage);
		PlayTeleportMontage->OnBlendOut.AddDynamic(this, &UArenasGA_Blink::K2_EndAbility);
		PlayTeleportMontage->OnCancelled.AddDynamic(this, &UArenasGA_Blink::K2_EndAbility);
		PlayTeleportMontage->OnInterrupted.AddDynamic(this, &UArenasGA_Blink::K2_EndAbility);
		PlayTeleportMontage->OnCompleted.AddDynamic(this, &UArenasGA_Blink::K2_EndAbility);
		PlayTeleportMontage->ReadyForActivation();

		if (K2_HasAuthority())
		{
			UAbilityTask_WaitGameplayEvent* WaitTeleportTimepoint = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Event_Ability_Blink_Teleport);
			WaitTeleportTimepoint->EventReceived.AddDynamic(this, &UArenasGA_Blink::Teleport);
			WaitTeleportTimepoint->ReadyForActivation();
		}
	}
	
}

void UArenasGA_Blink::GroundPickCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UArenasGA_Blink::Teleport(FGameplayEventData Payload)
{
	if (K2_HasAuthority())
	{
		FHitResult PickedLocationHit = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(BlinkTargetDataHandle, 1);
		FVector PickedTeleportLocation = PickedLocationHit.ImpactPoint;

		GetAvatarActorFromActorInfo()->SetActorLocation(PickedTeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);
		// BP_ApplyGameplayEffectToTarget(BlinkTargetDataHandle, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		ApplyAbilityMagicGameplayEffectToTarget(BlinkTargetDataHandle, DefaultDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		PushTargets(BlinkTargetDataHandle, PickedTeleportLocation, BlinkLandTargetPushSpeed);
		
	}
}
