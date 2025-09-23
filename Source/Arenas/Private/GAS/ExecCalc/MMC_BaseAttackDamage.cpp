// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GAS/ExecCalc/MMC_BaseAttackDamage.h"

#include "GAS/ArenasAttributeSet.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	AttackDamageCaptureDef.AttributeToCapture = UArenasAttributeSet::GetAttackDamageAttribute();
	AttackDamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	ArmorDamageCaptureDef.AttributeToCapture = UArenasAttributeSet::GetArmorAttribute();
	ArmorDamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;

	RelevantAttributesToCapture.Add(AttackDamageCaptureDef);
	RelevantAttributesToCapture.Add(ArmorDamageCaptureDef);
	
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float AttackDamage = 0.f;
	float Armor = 0.f;

	// 获取攻击者的攻击力属性值
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	GetCapturedAttributeMagnitude(AttackDamageCaptureDef, Spec, EvaluationParameters, AttackDamage);
	GetCapturedAttributeMagnitude(ArmorDamageCaptureDef, Spec, EvaluationParameters, Armor);
	
	// 计算最终伤害值，考虑护甲的减伤效果
	float FinalDamage = AttackDamage * (100.f / (100.f + Armor));

	return -FinalDamage;
}
