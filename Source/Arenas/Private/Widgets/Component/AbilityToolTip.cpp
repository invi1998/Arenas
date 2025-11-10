// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityToolTip.h"

#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UAbilityToolTip::SetAbilityInfo(UTexture2D* InAbilityIcon, const FName& InAbilityName, const FText& InAbilityLevel,
                                     const FText& InAbilityDescription, const FText& InAbilityCooldown, const FText& InAbilityManaCost,
                                     const FText& InAbilityEffects, const FText& InAbilitySubDescription)
{

	AbilityIconImage->SetBrushFromTexture(InAbilityIcon);
	AbilityNameText->SetText(FText::FromName(InAbilityName));
	AbilityLevelText->SetText(InAbilityLevel);
	AbilityDescriptionText->SetText(InAbilityDescription);
	AbilityCooldownText->SetText(InAbilityCooldown);
	AbilityManaCostText->SetText(InAbilityManaCost);
	AbilityEffectsRichText->SetText(InAbilityEffects);
	AbilitySubDescriptionText->SetText(InAbilitySubDescription);
	
}
