// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GAS/ArenasHeroAttributeSet.h"
#include "MMC_ManaRegen.generated.h"

/**
 * 回蓝计算，每点智力增加 0.05 的每秒回蓝 + 基础回蓝（装备等提供的回蓝不受智力影响）
 */

struct FArenasManaRegenCalculationData
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ManaRegen)

	FArenasManaRegenCalculationData()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, Intelligence, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, ManaRegen, Source, true);
	}
	
};

static const FArenasManaRegenCalculationData& GetManaRegenCalculationData()
{
	static FArenasManaRegenCalculationData Data;
	return Data;
}


UCLASS()
class ARENAS_API UMMC_ManaRegen : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_ManaRegen();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
