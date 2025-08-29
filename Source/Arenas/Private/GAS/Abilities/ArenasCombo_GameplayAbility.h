// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasCombo_GameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasCombo_GameplayAbility : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasCombo_GameplayAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	static FGameplayTag GetComboChangeEventTag();
	
	void SetupWaitComboInputPressTask();
	void TryCommitCombo();
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ComboMontage;

	UFUNCTION()
	void OnComboChangeEventReceived(FGameplayEventData PayloadData);

	UFUNCTION()
	void OnComboInputPressed(float TimeWaited);

	UFUNCTION()
	void DoDamage(FGameplayEventData Payload);

	FName NextComboName;
	
	
};
