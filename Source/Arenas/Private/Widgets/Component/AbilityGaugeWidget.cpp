// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityGaugeWidget.h"

#include "Components/Image.h"

void UAbilityGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UAbilityGaugeWidget::ConfigureWidgetData(const FAbilityWidgetData* AbilityWidgetData)
{
	if (Icon && AbilityWidgetData)
	{
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, AbilityWidgetData->AbilityIcon.LoadSynchronous());
	}
}

void UAbilityGaugeWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);


	
}
