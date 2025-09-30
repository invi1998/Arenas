// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToolTip.generated.h"

class UOverlay;
class UPA_ShopItem;
class URichTextBlock;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ARENAS_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItem(const UPA_ShopItem* Item);
	void SetPrice(const float NewPrice);

private:
	UPROPERTY(meta=(BindWidget))
	UImage* IconImage;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* DescriptionText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* PriceText;

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* EffectsRichText;

	UPROPERTY(meta=(BindWidget))
	UOverlay* ActiveAbilityOverlay;	// 主动技能面板

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* ActiveAbilityNameText;	// 主动技能名称

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* ActiveAbilityCostRichText;	// 主动技能消耗

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* ActiveAbilityRichText;	// 主动技能描述

	UPROPERTY(meta=(BindWidget))
	UOverlay* PassiveAbilityOverlay;	// 被动技能面板

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* PassiveAbilityNameText;	// 被动技能名称

	UPROPERTY(meta=(BindWidget))
	URichTextBlock* PassiveAbilityRichText;	// 被动技能描述
	
};
