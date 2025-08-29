// Ace of Arenas. (invi1998 All Rights Reserved)


#include "GAS/ExecCalc/ExecCalc_Damage.h"

#include "ArenasGameplayTags.h"

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(GetDamageCaptureStatics().DamageTakenDef);
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
		if (SetByCallerData.Key.MatchesTagExact(ArenasGameplayTags::SetByCaller_ComboIndex))
		{
			ComboIndex = FMath::RoundToInt(SetByCallerData.Value);
		}
	}

	if (ComboIndex > 0)
	{
		BaseDamage *= ComboIndex;	// 连招伤害翻倍
	}

	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			GetDamageCaptureStatics().DamageTakenProperty,
			EGameplayModOp::Override,
			BaseDamage
		)
	);
	
	
}
