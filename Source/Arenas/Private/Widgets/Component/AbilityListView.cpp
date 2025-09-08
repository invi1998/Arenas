// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityListView.h"

#include "AbilityGaugeWidget.h"
#include "Abilities/GameplayAbility.h"

void UAbilityListView::NativeConstruct()
{
}

void UAbilityListView::ConfigureAbilities(const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& InAbilities)
{
	ClearListItems();
	for (const TPair<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : InAbilities)
	{
		if (AbilityPair.Value)
		{
			AddItem(AbilityPair.Value.GetDefaultObject());
		}
	}
}



