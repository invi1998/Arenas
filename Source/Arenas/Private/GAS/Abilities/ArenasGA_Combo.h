// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Combo.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_Combo : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_Combo();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	static FGameplayTag GetComboChangeEventTag();
	
	void SetupWaitComboInputPressTask();
	void TryCommitCombo();

	int32 GetCurrentComboIndex() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ComboMontage;

	UFUNCTION()
	void OnComboChangeEventReceived(FGameplayEventData PayloadData);

	UFUNCTION()
	void OnComboInputPressed(float TimeWaited);

	FName NextComboName;

	UFUNCTION()
	void DoDamage(FGameplayEventData Payload);
};
