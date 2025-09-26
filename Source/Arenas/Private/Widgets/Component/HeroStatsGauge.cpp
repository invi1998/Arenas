// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "HeroStatsGauge.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/ArenasAbilitySystemComponent.h"

void UHeroStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Icon && IconTexture)
	{
		Icon->SetBrushFromTexture(IconTexture);
	}
}

void UHeroStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();

	NumberFormattingOptions.MaximumFractionalDigits = 0;
	GrowthRateNumberFormattingOptions.MaximumFractionalDigits = 1;
	GrowthRateNumberFormattingOptions.AlwaysSign = true;	// 总是显示正负号

	if (APawn* OwnerPlayerPawn = GetOwningPlayerPawn())
	{
		CachedArenasASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(OwnerPlayerPawn);
		if (CachedArenasASC)
		{
			bool bFound = false;
			float AttributeVal = CachedArenasASC->GetGameplayAttributeValue(AttributeToDisplay, bFound);
			if (bFound)
			{
				SetValue(AttributeVal);
			}
			CachedArenasASC->GetGameplayAttributeValueChangeDelegate(AttributeToDisplay).AddUObject(this, &UHeroStatsGauge::AttributeChanged);

			bFound = false;
			float GrowthRateVal = CachedArenasASC->GetGameplayAttributeValue(AttributeGrowthRate, bFound);
			if (bFound && AttributeGrowthRateText)
			{
				SetGrowthRateValue(GrowthRateVal);
			}
			CachedArenasASC->GetGameplayAttributeValueChangeDelegate(AttributeGrowthRate).AddUObject(this, &UHeroStatsGauge::AttributeGrowthRateChanged);
		}
	}
	
}

void UHeroStatsGauge::SetValue(float NewValue)
{
	AttributeText->SetText(FText::AsNumber(NewValue, &NumberFormattingOptions));
}

void UHeroStatsGauge::SetGrowthRateValue(float NewValue)
{
	// + number
	AttributeGrowthRateText->SetText(FText::AsNumber(NewValue, &GrowthRateNumberFormattingOptions));
}

void UHeroStatsGauge::AttributeChanged(const FOnAttributeChangeData& Data)
{
	SetValue(Data.NewValue);
}

void UHeroStatsGauge::AttributeGrowthRateChanged(const FOnAttributeChangeData& Data)
{
	SetGrowthRateValue(Data.NewValue);
}
