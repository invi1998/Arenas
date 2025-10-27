// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Lazer.generated.h"

class ATargetActor_Beam;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_Lazer : public UArenasGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* LazerMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> OnGoingConsumeEffect;	// 持续消耗效果

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float HitPushSpeed = 3000.f;		// 命中推开速度

	FActiveGameplayEffectHandle ActiveOngoingConsumeEffectHandle;	// 持续消耗效果句柄

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	TSubclassOf<ATargetActor_Beam> LazerTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	FName LazerTargetActorAttachSocketName = TEXT("Lazer");

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	float TargetRange = 4000.f;		// 激光束目标范围

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	float DetectionCylinderRadius = 30.f;	// 检测圆柱体半径

	UPROPERTY(EditDefaultsOnly, Category = "Target Actor")
	float TargetingInterval = 0.3f;		// 目标选择间隔时间

	void ManaUpdate(const FOnAttributeChangeData& OnAttributeChangeData);

	UFUNCTION()
	void TargetDataReceived(const FGameplayAbilityTargetDataHandle& Data);
	
	UFUNCTION()
	void OnLazerFireEventReceived(FGameplayEventData Payload);
	
};
