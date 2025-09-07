// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "Types/ArenaStructTypes.h"
#include "ArenasGA_UpperCut.generated.h"

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_UpperCut : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_UpperCut();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	const FGenericDamageEffectData* GetComboDamageEffectDataByName() const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TMap<FName, FGenericDamageEffectData> ComboDamageEffectDataMap; // 连招伤害数据映射表，Key为连招名称，Value为对应的伤害数据
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UpperCutMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float LaunchStrength_Self = 600.f; // 击飞力度

	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float LaunchStrength_Target = 1000.f; // 击飞力度

	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float LaunchStrength_Air = 100.f; // 击飞力度（空中目标）
	
	UFUNCTION()
	void OnUpperCutLaunch(FGameplayEventData Payload);

	UFUNCTION()
	void HandleComboCommitEvent(FGameplayEventData Payload);

	UFUNCTION()
	void HandleComboChange(FGameplayEventData Payload);

	int32 GetCurrentComboIndex() const;

	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData Payload);

	UFUNCTION()
	void HandleUpperCutFinalBlow(FGameplayEventData Payload);

	FName NextComboName;
	
};
