// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGA_Shoot.h"

#include "ArenasGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UArenasGA_Shoot::UArenasGA_Shoot()
{
	ActivationOwnedTags.AddTag(ArenasGameplayTags::Status_Aiming);
}

void UArenasGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      const FGameplayEventData* TriggerEventData)
{
	// 射击技能，当按住右键时，激活该技能，松开右键时，结束该技能
	// 技能激活期间，左键可以发射子弹
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		// 服务器端逻辑
		UAbilityTask_WaitGameplayEvent* WaitStartShootEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Ability_BasicAttack_Pressed);
		WaitStartShootEventTask->EventReceived.AddDynamic(this, &UArenasGA_Shoot::OnStartShoot);
		WaitStartShootEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitStopShootEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Ability_BasicAttack_Released);
		WaitStopShootEventTask->EventReceived.AddDynamic(this, &UArenasGA_Shoot::OnStopShoot);
		WaitStopShootEventTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ArenasGameplayTags::Ability_BasicAttack);
		WaitShootProjectileEventTask->EventReceived.AddDynamic(this, &UArenasGA_Shoot::ShootProjectile);
		WaitShootProjectileEventTask->ReadyForActivation();
		
	}
	
}

void UArenasGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot Ability Input Released"));
	K2_EndAbility();
}

void UArenasGA_Shoot::OnStartShoot(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("UArenasGA_Shoot::OnStartShoot called"));
}

void UArenasGA_Shoot::OnStopShoot(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("UArenasGA_Shoot::OnStopShoot called"));
}

void UArenasGA_Shoot::ShootProjectile(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("UArenasGA_Shoot::ShootProjectile called"));
}
