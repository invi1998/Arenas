// Ace of Arenas. (invi1998 All Rights Reserved)


#include "ArenaStructTypes.h"

#include "GAS/Abilities/ArenasGameplayAbility.h"

bool FArenasAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrantClass;
}
