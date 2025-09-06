// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_UpperCut.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_UpperCut : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UpperCutMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float LaunchStrength = 600.f; // 击飞力度

	UFUNCTION()
	void OnUpperCutLaunch(FGameplayEventData Payload);

	
};
