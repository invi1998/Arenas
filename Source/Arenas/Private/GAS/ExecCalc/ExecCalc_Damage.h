// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS/ArenasAttributeSet.h"
#include "ExecCalc_Damage.generated.h"

struct FArenasDamageCaptureStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage)

	FArenasDamageCaptureStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, DamageTaken, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, Armor, Target, true)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, AttackDamage, Source, true)
	}
	
};

static const FArenasDamageCaptureStatics& GetDamageCaptureStatics()
{
	static FArenasDamageCaptureStatics DmgStatics;
	return DmgStatics;
}


/**
 * 
 */
UCLASS()
class UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
