// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GAS/ArenasAttributeSet.h"
#include "ExecCalc_Damage.generated.h"

struct FArenasDamageCaptureStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)

	FArenasDamageCaptureStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UArenasAttributeSet, DamageTaken, Target, true)
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
