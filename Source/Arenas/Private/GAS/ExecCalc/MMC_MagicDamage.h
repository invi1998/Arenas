// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GAS/ArenasAttributeSet.h"
#include "MMC_MagicDamage.generated.h"

/**
 * 魔法伤害计算（通常是技能伤害）
 */

struct FArenasMagicDamageCaptureStatics_MMC
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResist)
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistEx)

	FArenasMagicDamageCaptureStatics_MMC()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, MagicResist, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, MagicResistEx, Target, true)
	}
};

static const FArenasMagicDamageCaptureStatics_MMC& GetMMCMagicDamageCaptureStatics()
{
	static FArenasMagicDamageCaptureStatics_MMC DmgStatics;
	return DmgStatics;
}


UCLASS()
class ARENAS_API UMMC_MagicDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MagicDamage();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
	
	
};
