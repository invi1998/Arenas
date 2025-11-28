// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GAS/ArenasAttributeSet.h"
#include "GAS/ArenasHeroAttributeSet.h"
#include "MMC_HealthRegen.generated.h"

struct FArenasHealthRegenCalculationData
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength)
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealthRegen)

	FArenasHealthRegenCalculationData()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, Strength, Source, false);		// true表示拍摄快照（即：计算时使用的值是效果应用时的值，而不是当前值）， false表示实时获取
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, HealthRegen, Source, false);
	}
	
};

static const FArenasHealthRegenCalculationData& GetArenasHealthRegenCalculationData()
{
	static FArenasHealthRegenCalculationData Data;
	return Data;
}

/**
 * 
 */
UCLASS()
class ARENAS_API UMMC_HealthRegen : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_HealthRegen();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
};
