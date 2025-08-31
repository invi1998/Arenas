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

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	float BaseDamage = 10.f;	// 基础伤害

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectsMap;	// 连招伤害GameplayEffect映射表，Key为连招名称Combo01, Combo02, Combo03, Combo04...，Value为对应的GameplayEffect Class

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;	// 默认伤害GameplayEffect

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	float TargetSweepSphereRadius = 30.f;	// 目标检测球体扫描半径
	
	TSubclassOf<UGameplayEffect> GetCurrentComboDamageEffect() const;

	int32 GetCurrentComboIndex() const;
	
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
