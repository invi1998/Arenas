// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "HeroStatsGauge.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/ArenasAbilitySystemComponent.h"

void UHeroStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	Icon->SetBrushFromTexture(IconTexture);
}

void UHeroStatsGauge::NativeConstruct()
{
	Super::NativeConstruct();

	NumberFormattingOptions.MaximumFractionalDigits = 0;

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
		}
	}
	
}

void UHeroStatsGauge::SetValue(float NewValue)
{
	AttributeText->SetText(FText::AsNumber(NewValue, &NumberFormattingOptions));
}

void UHeroStatsGauge::AttributeChanged(const FOnAttributeChangeData& Data)
{
	SetValue(Data.NewValue);
}
