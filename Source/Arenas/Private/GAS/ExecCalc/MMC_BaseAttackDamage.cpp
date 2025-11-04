// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "GAS/ExecCalc/MMC_BaseAttackDamage.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "GAS/ArenasAbilitySystemComponent.h"

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
	float FinalArmor = 1 - (1 - Armor / 100.f) * (1 - ArmorEx / 100.f);
	AttackDamage += BaseDamage;
	AttackDamage += AttackDamage * 0.1f * ComboIndex;
	
	// 计算最终伤害值，考虑护甲的减伤效果
	float FinalDamage = AttackDamage * (100.f / (100.f + FinalArmor));

	// 显示伤害数字
	FVector HitLocation = Spec.GetContext().GetHitResult() ? Spec.GetContext().GetHitResult()->ImpactPoint : FVector::ZeroVector;

	// 获取攻击者的AbilitySystemComponent
	if (UArenasAbilitySystemComponent* ArenasASC = Cast<UArenasAbilitySystemComponent>(Spec.GetContext().GetInstigatorAbilitySystemComponent()))
	{
		
		// 统计英雄角色的伤害
		if (ArenasASC->IsHeroCharacter())
		{
			ArenasASC->AddDamageMatchStatNumber(FinalDamage);
			ArenasASC->ShowComboText(FinalDamage, EArenasComboTextType::Normal, HitLocation);

		}
		
	}

	// 获取受击者的AbilitySystemComponent
	if (Spec.GetContext().GetHitResult() && Spec.GetContext().GetHitResult()->GetActor())
	{
		if (UArenasAbilitySystemComponent* TargetASC = UArenasBlueprintFunctionLibrary::NativeGetArenasASCFromActor(Spec.GetContext().GetHitResult()->GetActor()))
		{
			if (TargetASC->IsHeroCharacter())
			{
				// 这里可以统计目标英雄承受伤害
				TargetASC->ShowComboText(FinalDamage, EArenasComboTextType::Damaged, HitLocation);
			}
		}
	}
	

	return -FinalDamage;
}
