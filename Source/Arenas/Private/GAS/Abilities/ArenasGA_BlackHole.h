// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_BlackHole.generated.h"

class UAbilityTask_PlayMontageAndWait;
class ATargetActor_GroundPick;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_BlackHole : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 500.f;		// 目标区域半径

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> AimEffectClass;		// 瞄准特效GameplayEffect类

	FActiveGameplayEffectHandle ActiveAimGameplayEffectHandle;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayCastBlackHoleMontageTask;

	UFUNCTION()
	void PlaceBlackHole(const FGameplayAbilityTargetDataHandle& Data);		// 放置黑洞

	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& Data);	// 放置取消

	void AddAimEffect();
	void RemoveAimEffect();
	
};
