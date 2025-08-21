// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenasCombo_GameplayAbility.h"

void UArenasCombo_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Active Ability Combo Ability: %s"), *GetNameSafe(this));
	
}
