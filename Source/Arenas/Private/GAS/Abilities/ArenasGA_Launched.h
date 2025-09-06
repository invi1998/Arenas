// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Launched.generated.h"

/**
 * 被动技能：被击飞或者击退
 */
UCLASS()
class ARENAS_API UArenasGA_Launched : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_Launched();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
