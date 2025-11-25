// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MMC_AttackDamagePRBase.h"

UMMC_AttackDamagePRBase::UMMC_AttackDamagePRBase()
{
	RelevantAttributesToCapture.Add(GetArenasAttackDamagePRBaseCalculation().StrengthDef);
	RelevantAttributesToCapture.Add(GetArenasAttackDamagePRBaseCalculation().IntelligenceDef);
	RelevantAttributesToCapture.Add(GetArenasAttackDamagePRBaseCalculation().AgilityDef);
	RelevantAttributesToCapture.Add(GetArenasAttackDamagePRBaseCalculation().HeroTypeDef);
}

float UMMC_AttackDamagePRBase::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float HeroType = 0;
	float Strength = 0.f;
	float Intelligence = 0.f;
	float Agility = 0.f;

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	GetCapturedAttributeMagnitude(GetArenasAttackDamagePRBaseCalculation().HeroTypeDef, Spec, EvaluationParameters, HeroType);
	GetCapturedAttributeMagnitude(GetArenasAttackDamagePRBaseCalculation().StrengthDef, Spec, EvaluationParameters, Strength);
	GetCapturedAttributeMagnitude(GetArenasAttackDamagePRBaseCalculation().IntelligenceDef, Spec, EvaluationParameters, Intelligence);
	GetCapturedAttributeMagnitude(GetArenasAttackDamagePRBaseCalculation().AgilityDef, Spec, EvaluationParameters, Agility);

	/**
	* 如果是敏捷英雄：每一点敏捷额外增加1点攻击力
	* 如果是力量英雄：每一点力量额外增加1点攻击力
	* 如果是智力英雄：每一点智力额外增加1点攻击力
	* 如果全才英雄：每一点敏捷、力量、智力额外增加0.45点攻击力
	 */
	if (HeroType == 1)	// 力量英雄
	{
		return Strength;
	}
	else if (HeroType == 2)	// 敏捷英雄
	{
		return Agility;
	}
	else if (HeroType == 3)	// 智力英雄
	{
		return Intelligence;
	}
	else if (HeroType == 4)	// 全才英雄
	{
		return (Agility + Strength + Intelligence) * 0.45f;
	}

	return 0.f;
	
}
