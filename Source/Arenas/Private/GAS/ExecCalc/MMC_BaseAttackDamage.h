// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GAS/ArenasAttributeSet.h"
#include "MMC_BaseAttackDamage.generated.h"

struct FArenasDamageCaptureStatics_MMC
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorEx)
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamageEx)

	FArenasDamageCaptureStatics_MMC()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, Armor, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, ArmorEx, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, AttackDamage, Source, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, AttackDamageEx, Source, true)
	}
	
};

static const FArenasDamageCaptureStatics_MMC& GetMMCDamageCaptureStatics()
{
	static FArenasDamageCaptureStatics_MMC DmgStatics;
	return DmgStatics;
}

/**
 * 
 */
UCLASS()
class UMMC_BaseAttackDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_BaseAttackDamage();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	/*
	FGameplayEffectAttributeCaptureDefinition AttackDamageCaptureDef;
	FGameplayEffectAttributeCaptureDefinition ArmorDamageCaptureDef;
	*/
	
};
