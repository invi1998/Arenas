// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MMC_HealthRegen.h"

UMMC_HealthRegen::UMMC_HealthRegen()
{
	RelevantAttributesToCapture.Add(GetArenasHealthRegenCalculationData().StrengthDef);
	RelevantAttributesToCapture.Add(GetArenasHealthRegenCalculationData().HealthRegenDef);
}

float UMMC_HealthRegen::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float Strength = 0;
	float HealthRegen = 0;
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	GetCapturedAttributeMagnitude(GetArenasHealthRegenCalculationData().StrengthDef, Spec, EvaluationParameters, Strength);
	GetCapturedAttributeMagnitude(GetArenasHealthRegenCalculationData().HealthRegenDef, Spec, EvaluationParameters, HealthRegen);

	float FinalHealthRegen = HealthRegen + Strength * 0.1f;
	return FinalHealthRegen;
	
}
