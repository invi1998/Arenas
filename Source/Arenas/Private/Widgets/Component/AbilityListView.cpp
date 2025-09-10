// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityListView.h"

#include "AbilityGaugeWidget.h"
#include "Abilities/GameplayAbility.h"
#include "Types/ArenaStructTypes.h"

void UAbilityListView::NativeConstruct()
{
}

void UAbilityListView::OnAbilityWidgetGenerated(UUserWidget& AbilityUserWidget)
{
	if (UAbilityGaugeWidget* AbilityGaugeWidget = Cast<UAbilityGaugeWidget>(&AbilityUserWidget))
	{
		if (const FAbilityWidgetData* AbilityWidgetData = FindWidgetDataByAbilityClass(AbilityGaugeWidget->GetListItem<UGameplayAbility>()->GetClass()))
		{
			AbilityGaugeWidget->ConfigureWidgetData(AbilityWidgetData);
		}
	}
}

void UAbilityListView::ConfigureAbilities(const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& InAbilities)
{
	ClearListItems();
	OnEntryWidgetGenerated().AddUObject(this, &UAbilityListView::OnAbilityWidgetGenerated);
	for (const TPair<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : InAbilities)
	{
		if (AbilityPair.Value)
		{
			AddItem(AbilityPair.Value.GetDefaultObject());
		}
	}
}

const FAbilityWidgetData* UAbilityListView::FindWidgetDataByAbilityClass(const TSubclassOf<UGameplayAbility>& InAbilityClass) const
{
	if (!AbilityDataTable) return nullptr;

	for (auto& AbilityWidgetDataPair : AbilityDataTable->GetRowMap())
	{
		const FAbilityWidgetData* AbilityWidgetData = AbilityDataTable->FindRow<FAbilityWidgetData>(AbilityWidgetDataPair.Key, TEXT(""));
		if (AbilityWidgetData && AbilityWidgetData->AbilityClass == InAbilityClass)
		{
			return AbilityWidgetData;
		}
	}
	return nullptr;
}



