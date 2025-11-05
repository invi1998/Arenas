// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Tornado.generated.h"

/**
 * 技能：龙卷风
 */
UCLASS()
class ARENAS_API UArenasGA_Tornado : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float HitPushSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TornadoMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FScalableFloat TornadoDuration;

	UFUNCTION()
	void TornadoDamageEventReceived(FGameplayEventData Payload);
	
};
