// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenaStructTypes.h"

#include "GAS/Abilities/ArenasGameplayAbility.h"

bool FArenasAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrantClass;
}

FGenericDamageEffectData::FGenericDamageEffectData() : PushVelocity{0.f}
{
}

FAbilityWidgetData::FAbilityWidgetData() : InputAction(nullptr)
{
}

FHeroBaseStats::FHeroBaseStats() : BaseMaxHealth(0), BaseMaxMana(0), BaseAttackDamage(0), BaseArmor(0),
                                   BaseMoveSpeed(0), Strength(0),
                                   Intelligence(0),
                                   StrengthGrowthRate(0),
                                   IntelligenceGrowthRate(0),
                                   ExpGrowthRate(0),
                                   StartingGold(0)
{
}
