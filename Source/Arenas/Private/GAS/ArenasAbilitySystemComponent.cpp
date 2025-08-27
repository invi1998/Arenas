// Ace of Arenas. (invi1998 All Rights Reserved)


#include "GAS/ArenasAbilitySystemComponent.h"

#include "ArenasAttributeSet.h"
#include "GAS/Abilities/ArenasGameplayAbility.h"

void UArenasAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialGameplayEffects)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UArenasAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TPair<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		if (AbilityPair.Value)
		{
			FGameplayAbilitySpec AbilitySpec(AbilityPair.Value, 0, static_cast<int32>(AbilityPair.Key));
			GiveAbility(AbilitySpec);
		}
	}

	for (const TPair<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		if (AbilityPair.Value)
		{
			FGameplayAbilitySpec AbilitySpec(AbilityPair.Value, 1, static_cast<int32>(AbilityPair.Key));
			GiveAbility(AbilitySpec);
		}
	}
	
}
