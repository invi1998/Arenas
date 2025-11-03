// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_FallingStar.generated.h"

class ATargetActor_GroundPick;
class UAbilityTask_WaitTargetData;
class ATargetActor_FallingStar;
/**
 * 星落术能力
 */
UCLASS()
class ARENAS_API UArenasGA_FallingStar : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_FallingStar();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetRange = 2000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float AOERadius = 1200.f;		// 范围伤害半径

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FScalableFloat LoopDuration;		// 循环持续时间

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FScalableFloat SpawnRate;			// 生成速率

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SpawnHeight = 1500.f;		// 生成高度

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float FallingSpeed = 3000.f;	// 下落速度

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_FallingStar> FallingStarTargetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* CastingMontage;

	UPROPERTY()
	UAbilityTask_WaitTargetData* WaitFallingStarTargetDataTask;
	
	UFUNCTION()
	void OnFallingStarTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnFallingStarTargetCancelled(const FGameplayAbilityTargetDataHandle& Data);
	
};
