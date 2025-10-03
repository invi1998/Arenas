// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GAS/ArenasAttributeSet.h"
#include "GAS/ArenasHeroAttributeSet.h"
#include "MMC_LevelBased.generated.h"

struct FArenasLevelBasedCaptureStatics_MMC
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Level)

	FArenasLevelBasedCaptureStatics_MMC()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, Level, Target, false)
	}
	
};

static const FArenasLevelBasedCaptureStatics_MMC& GetMMCLevelBasedCaptureStatic()
{
	static FArenasLevelBasedCaptureStatics_MMC LevelStatics;
	return LevelStatics;
}

/**
 * 
 */
UCLASS()
class ARENAS_API UMMC_LevelBased : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_LevelBased();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute RateAttribute;
	
};
