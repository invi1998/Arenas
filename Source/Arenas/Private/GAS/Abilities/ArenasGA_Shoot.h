// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Shoot.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_Shoot : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_Shoot();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	                             const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Shooting")
	UAnimMontage* ShootMontage;
	
	UFUNCTION()
	void OnStartShoot(FGameplayEventData Payload);

	UFUNCTION()
	void OnStopShoot(FGameplayEventData Payload);

	UFUNCTION()
	void ShootProjectile(FGameplayEventData Payload);

	FGameplayTag GetShootEventTag() const;
	
	
};

