// Ace of Arenas. (invi1998 All Rights Reserved)


#include "GAS/ArenasAbilitySystemComponent.h"

void UArenasAbilitySystemComponent::ApplyInitialEffects()
{
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialGameplayEffects)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}
