// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ArenasGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UAnimInstance* GetOwnerAnimInstance() const;

	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetData, float SphereSweepRadius = 30.f, bool bDebug = false, bool bIgnoreSelf = true) const;
};
