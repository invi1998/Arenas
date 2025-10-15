// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GAS/ExecCalc/MMC_BaseAttackDamage.h"

#include "ArenasGameplayTags.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	/*
	AttackDamageCaptureDef.AttributeToCapture = UArenasAttributeSet::GetAttackDamageAttribute();
	AttackDamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;

	ArmorDamageCaptureDef.AttributeToCapture = UArenasAttributeSet::GetArmorAttribute();
	ArmorDamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	*/

	RelevantAttributesToCapture.Add(GetMMCDamageCaptureStatics().ArmorDef);
	RelevantAttributesToCapture.Add(GetMMCDamageCaptureStatics().ArmorExDef);
	RelevantAttributesToCapture.Add(GetMMCDamageCaptureStatics().AttackDamageDef);
	RelevantAttributesToCapture.Add(GetMMCDamageCaptureStatics().AttackDamageExDef);
	
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float AttackDamage = 0.f;
	float AttackDamageEx = 0.f;
	float Armor = 0.f;
	float ArmorEx = 0.f;

	// 获取攻击者的攻击力属性值
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	GetCapturedAttributeMagnitude(GetMMCDamageCaptureStatics().AttackDamageDef, Spec, EvaluationParameters, AttackDamage);
	GetCapturedAttributeMagnitude(GetMMCDamageCaptureStatics().AttackDamageExDef, Spec, EvaluationParameters, AttackDamageEx);
	GetCapturedAttributeMagnitude(GetMMCDamageCaptureStatics().ArmorDef, Spec, EvaluationParameters, Armor);
	GetCapturedAttributeMagnitude(GetMMCDamageCaptureStatics().ArmorExDef, Spec, EvaluationParameters, ArmorEx);

	float BaseDamage = 10.f;
	
	int32 ComboIndex = 0;
	for (const TPair<FGameplayTag, float>& SetByCallerData : Spec.SetByCallerTagMagnitudes)
	{
		if (SetByCallerData.Key.MatchesTagExact(ArenasGameplayTags::SetByCaller_BaseDamage))
		{
			BaseDamage = SetByCallerData.Value;
		}
		else if (SetByCallerData.Key.MatchesTagExact(ArenasGameplayTags::SetByCaller_ComboIndex))
		{
			ComboIndex = FMath::RoundToInt(SetByCallerData.Value);
		}
	}
	AttackDamage += AttackDamageEx;
	Armor += ArmorEx;
	AttackDamage += BaseDamage;
	AttackDamage += AttackDamage * 0.1f * ComboIndex;
	
	// 计算最终伤害值，考虑护甲的减伤效果
	float FinalDamage = AttackDamage * (100.f / (100.f + Armor));

	return -FinalDamage;
}
