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

	for (const FArenasAbilitySet& AbilitySet : Abilities)
	{
		if (AbilitySet.IsValid())
		{
			FGameplayAbilitySpec NewAbilitySpec(AbilitySet.AbilityToGrantClass);
			NewAbilitySpec.SourceObject = GetAvatarActor();
			NewAbilitySpec.Level = 0;
			NewAbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);	// 技能输入标签

			GiveAbility(NewAbilitySpec);
		}
	}

	for (const FArenasAbilitySet& AbilitySet : BasicAbilities)
	{
		if (AbilitySet.IsValid())
		{
			FGameplayAbilitySpec NewAbilitySpec(AbilitySet.AbilityToGrantClass);
			NewAbilitySpec.SourceObject = GetAvatarActor();
			NewAbilitySpec.Level = 1;
			NewAbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);	// 技能输入标签

			GiveAbility(NewAbilitySpec);
		}
	}
	
}
