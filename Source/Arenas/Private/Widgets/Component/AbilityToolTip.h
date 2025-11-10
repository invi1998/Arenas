// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilityToolTip.generated.h"

class URichTextBlock;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ARENAS_API UAbilityToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetAbilityInfo(UTexture2D* InAbilityIcon, const FString& InAbilityName, const FString& InAbilityDescription, const FString& InAbilityCooldown, const FString& InAbilityManaCost, const FString& InAbilityEffects, const FString& InAbilitySubDescription);

private:
	UPROPERTY(meta=(BindWidget))
	UImage* AbilityIconImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilityNameText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilityDescriptionText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilityCooldownText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilityManaCostText;

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* AbilityEffectsRichText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AbilitySubDescriptionText;
	
};
