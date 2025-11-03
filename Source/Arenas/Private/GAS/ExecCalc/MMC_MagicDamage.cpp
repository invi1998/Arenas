// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MMC_MagicDamage.h"

#include "ArenasBlueprintFunctionLibrary.h"
#include "ArenasGameplayTags.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "Types/ArenaStructTypes.h"

UMMC_MagicDamage::UMMC_MagicDamage()
{
	RelevantAttributesToCapture.Add(GetMMCMagicDamageCaptureStatics().MagicResistDef);
	RelevantAttributesToCapture.Add(GetMMCMagicDamageCaptureStatics().MagicResistExDef);
}

float UMMC_MagicDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float MagicDamage = 0.f;
	float MagicResist = 0.f;
	float MagicResistEx = 0.f;

	// 获取被攻击者的魔法抗性和额外魔法抗性
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	GetCapturedAttributeMagnitude(GetMMCMagicDamageCaptureStatics().MagicResistDef, Spec, EvaluationParameters, MagicResist);
	GetCapturedAttributeMagnitude(GetMMCMagicDamageCaptureStatics().MagicResistExDef, Spec, EvaluationParameters, MagicResistEx);
	
	for (const TPair<FGameplayTag, float>& SetByCallerData : Spec.SetByCallerTagMagnitudes)
	{
		if (SetByCallerData.Key.MatchesTagExact(ArenasGameplayTags::SetByCaller_MagicDamage))
		{
			MagicDamage = SetByCallerData.Value;
			break;
		}
	}

	// 最终魔法抗性计算 (总魔法抗性 = 1 - (1 - a) × (1 - b) × (1 - c) × ...）
	float FinalMagicResit = 1 - (1 - MagicResist / 100.f) * (1 - MagicResistEx / 100.f);
	
	// 计算最终魔法伤害
	float FinalMagicDamage = MagicDamage * (100.f / (100.f + FinalMagicResit));

	// 显示伤害数字
	FVector HitLocation = Spec.GetContext().GetHitResult() ? Spec.GetContext().GetHitResult()->ImpactPoint : FVector::ZeroVector;
	// 获取攻击者的AbilitySystemComponent
	if (UArenasAbilitySystemComponent* ArenasASC = Cast<UArenasAbilitySystemComponent>(Spec.GetContext().GetInstigatorAbilitySystemComponent()))
	{
		
		// 统计英雄角色的伤害
		if (ArenasASC->IsHeroCharacter())
		{
			ArenasASC->AddDamageMatchStatNumber(FinalMagicDamage);
			ArenasASC->ShowComboText(FinalMagicDamage, EArenasComboTextType::MagicDamaged, HitLocation);

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
				TargetASC->ShowComboText(FinalMagicDamage, EArenasComboTextType::Damaged, HitLocation);
			}
		}
	}

	return -FinalMagicDamage;
	
}
