// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_GroundBlast.generated.h"

class ATargetActor_GroundPick;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_GroundBlast : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_GroundBlast();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* GroundBlastMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetClass;
	
	UFUNCTION()
	void OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);

	UFUNCTION()
	void OnTargetCancelled(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);
	
};
