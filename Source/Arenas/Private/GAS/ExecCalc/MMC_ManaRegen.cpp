// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MMC_ManaRegen.h"

#include "MMC_HealthRegen.h"

UMMC_ManaRegen::UMMC_ManaRegen()
{
	RelevantAttributesToCapture.Add(GetManaRegenCalculationData().IntelligenceDef);
	RelevantAttributesToCapture.Add(GetManaRegenCalculationData().ManaRegenDef);
}

float UMMC_ManaRegen::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float Intelligence = 0.f;
	float ManaRegen = 0.f;
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	GetCapturedAttributeMagnitude(GetManaRegenCalculationData().IntelligenceDef, Spec, EvaluationParameters, Intelligence);
	GetCapturedAttributeMagnitude(GetManaRegenCalculationData().ManaRegenDef, Spec, EvaluationParameters, ManaRegen);

	float FinalManaRegen = Intelligence * 0.05f + ManaRegen;
	return FinalManaRegen;
	
}
