// Ace of Arenas. (invi1998 All Rights Reserved)


#include "GAS/ExecCalc/ExecCalc_Damage.h"

#include "ArenasGameplayTags.h"

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().DamageTakenDef);
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().AttackDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

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

	float AttackDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().AttackDamageDef, EvaluateParams, AttackDamage);
	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetDamageCaptureStatics().ArmorDef, EvaluateParams, Armor);

	// 每段连招加百分之10的伤害
	AttackDamage += BaseDamage;
	AttackDamage += AttackDamage * 0.1f * ComboIndex;

	// 最终伤害计算公式
	float Damage = AttackDamage * (100.f / (100.f + Armor));

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			GetDamageCaptureStatics().DamageTakenProperty,
			EGameplayModOp::Override,
			Damage
		)
	);
	
	
}
