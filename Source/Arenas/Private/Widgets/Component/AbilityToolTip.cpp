// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "AbilityToolTip.h"

#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UAbilityToolTip::SetAbilityInfo(UTexture2D* InAbilityIcon, const FString& InAbilityName,
                                     const FString& InAbilityDescription, const FString& InAbilityCooldown, const FString& InAbilityManaCost,
                                     const FString& InAbilityEffects, const FString& InAbilitySubDescription)
{

	AbilityIconImage->SetBrushFromTexture(InAbilityIcon);
	AbilityNameText->SetText(FText::FromString(InAbilityName));
	AbilityDescriptionText->SetText(FText::FromString(InAbilityDescription));
	AbilityCooldownText->SetText(FText::FromString(InAbilityCooldown));
	AbilityManaCostText->SetText(FText::FromString(InAbilityManaCost));
	AbilityEffectsRichText->SetText(FText::FromString(InAbilityEffects));
	AbilitySubDescriptionText->SetText(FText::FromString(InAbilitySubDescription));
	
}
