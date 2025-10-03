// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MMC_LevelBased.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	RelevantAttributesToCapture.Add(GetMMCLevelBasedCaptureStatic().LevelDef);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	UAbilitySystemComponent* SourceASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!SourceASC)
	{
		return 0.f;
	}

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 获取等级属性值
	float Level = 0.f;
	GetCapturedAttributeMagnitude(GetMMCLevelBasedCaptureStatic().LevelDef, Spec, EvaluationParameters, Level);

	bool bFound = false;
	float RateAttributeValue = SourceASC->GetGameplayAttributeValue(RateAttribute, bFound);
	if (!bFound)
	{
		return 0.f;
	}
	
	return (Level - 1) * RateAttributeValue;
	
}
