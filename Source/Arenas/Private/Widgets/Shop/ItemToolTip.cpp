// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ItemToolTip.h"

#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Inventory/PA_ShopItem.h"

void UItemToolTip::SetItem(const UPA_ShopItem* Item)
{
	IconImage->SetBrushFromTexture(Item->GetIcon());
	ItemNameText->SetText(Item->GetItemName());
	DescriptionText->SetText(Item->GetDescription());
	PriceText->SetText(FText::AsNumber(static_cast<int>(Item->GetPrice())));

	if (FText EffectText = Item->GetEffectRichText(); !EffectText.IsEmptyOrWhitespace())
	{
		EffectsRichText->SetVisibility(ESlateVisibility::Visible);
		EffectsRichText->SetText(EffectText);
	}
	else
	{
		EffectsRichText->SetVisibility(ESlateVisibility::Collapsed);
		EffectsRichText->SetText(FText::GetEmpty());
	}

	if (FText ActiveAbilityName = Item->GetActiveAbilityName(); !ActiveAbilityName.IsEmptyOrWhitespace())
	{
		ActiveAbilityOverlay->SetVisibility(ESlateVisibility::Visible);
		ActiveAbilityNameText->SetText(ActiveAbilityName);
		ActiveAbilityCostRichText->SetText(Item->GetActiveAbilityCostRichText());
		ActiveAbilityRichText->SetText(Item->GetActiveAbilityRichText());
	}
	else
	{
		ActiveAbilityOverlay->SetVisibility(ESlateVisibility::Collapsed);
		ActiveAbilityNameText->SetText(FText::GetEmpty());
		ActiveAbilityCostRichText->SetText(FText::GetEmpty());
		ActiveAbilityRichText->SetText(FText::GetEmpty());
	}

	if (FText PassiveAbilityName = Item->GetPassiveAbilityName(); !PassiveAbilityName.IsEmptyOrWhitespace())
	{
		PassiveAbilityOverlay->SetVisibility(ESlateVisibility::Visible);
		PassiveAbilityNameText->SetText(PassiveAbilityName);
		PassiveAbilityRichText->SetText(Item->GetPassiveAbilityRichText());
	}
	else
	{
		PassiveAbilityOverlay->SetVisibility(ESlateVisibility::Collapsed);
		PassiveAbilityNameText->SetText(FText::GetEmpty());
		PassiveAbilityRichText->SetText(FText::GetEmpty());
	}
	
}

void UItemToolTip::SetPrice(const float NewPrice)
{
	PriceText->SetText(FText::AsNumber(static_cast<int>(NewPrice)));
}
