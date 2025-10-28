// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_BlackHole.generated.h"

class UAbilityTask_WaitTargetData;
class ATargetActor_BlackHole;
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
	float TargetAreaRadius = 1000.f;		// 目标区域半径

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	float PullSpeed = 600.f;					// 吸引速度

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	float BlackHoleDuration = 6.f;			// 黑洞持续时间

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	float HitPushSpeed = 1000.f;				// 被黑洞吸引到中心后推开速度

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TargetingMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HoldBlackHoleMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FinalBlowMontage;		// 终结一击动画

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> AimEffectClass;		// 瞄准状态GameplayEffect类

	FActiveGameplayEffectHandle ActiveAimGameplayEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	TSubclassOf<UGameplayEffect> FocusedEffectClass;	// 状态集中效果GameplayEffect类

	FActiveGameplayEffectHandle ActiveFocusedGameplayEffectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect")
	FGameplayTag BlackHoleExplodeCueTag;			// 黑洞爆炸GameplayCue标签
	
	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	TSubclassOf<ATargetActor_BlackHole> TargetActorClass;

	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* PlayCastBlackHoleMontageTask;

	UPROPERTY()
	UAbilityTask_WaitTargetData* BlackHoleTargetDataTask;

	UFUNCTION()
	void BlackHoleTargetDataReceived(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void BlackHoleTargetDataCancelled(const FGameplayAbilityTargetDataHandle& Data);
	
	UFUNCTION()
	void PlaceBlackHole(const FGameplayAbilityTargetDataHandle& Data);		// 放置黑洞

	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& Data);	// 放置取消

	void AddAimEffect();
	void RemoveAimEffect();
	void AddFocusedEffect();
	void RemoveFocusedEffect();
	
};
