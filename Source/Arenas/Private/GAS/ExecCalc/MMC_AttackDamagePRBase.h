// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GAS/ArenasHeroAttributeSet.h"
#include "MMC_AttackDamagePRBase.generated.h"

/**
* 如果是敏捷英雄：每一点敏捷额外增加1点攻击力
* 如果是力量英雄：每一点力量额外增加1点攻击力
* 如果是智力英雄：每一点智力额外增加1点攻击力
* 如果全才英雄：每一点敏捷、力量、智力额外增加0.45点攻击力
* 英雄每拥有1点智力，就会增加0.1%基础魔法抗性。
 */

struct FArenasAttackDamagePRBaseCalculation
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Intelligence)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Agility)
	DECLARE_ATTRIBUTE_CAPTUREDEF(HeroType)

	FArenasAttackDamagePRBaseCalculation()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, Strength, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, Intelligence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, Agility, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasHeroAttributeSet, HeroType, Target, false);
	}
	
};

static const FArenasAttackDamagePRBaseCalculation& GetArenasAttackDamagePRBaseCalculation()
{
	static FArenasAttackDamagePRBaseCalculation Calculation;
	return Calculation;
}


UCLASS()
class ARENAS_API UMMC_AttackDamagePRBase : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_AttackDamagePRBase();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
};
