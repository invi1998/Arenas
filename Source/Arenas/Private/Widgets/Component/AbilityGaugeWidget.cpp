// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityGaugeWidget.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAbilityGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CooldownCountdownText->SetVisibility(ESlateVisibility::Hidden);
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

	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);
	
	if (AbilityCDO && CostText && CooldownDurationText)
	{
		float StaticCooldown = UArenasBlueprintFunctionLibrary::GetStaticCooldownDurationFromAbility(AbilityCDO);
		float StaticCost = UArenasBlueprintFunctionLibrary::GetStaticCostFromAbility(AbilityCDO);

		CooldownCountdownText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), StaticCooldown)));
		CostText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), StaticCost)));
	}
	
}
