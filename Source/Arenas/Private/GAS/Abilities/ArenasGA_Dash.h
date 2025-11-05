// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Dash.generated.h"

class UAbilityTask_WaitInputRelease;
class ATargetActor_ChargeIndicator;
class ATargetActor_Around;
class UCharacterMovementComponent;
/**
 * 技能：冲刺
 */
UCLASS()
class ARENAS_API UArenasGA_Dash : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* DashChargeMontage;
		
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* DashMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetDetectionRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag LocalGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName TargetActorAttachSocketName = "TargetDashCenter";

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_ChargeIndicator> ChargeIndicatorTargetActorClass;

	UPROPERTY()
	ATargetActor_ChargeIndicator* ChargeIndicatorTargetActor;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_Around> DashTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float TargetHitPushSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DashEffect;
	
	FActiveGameplayEffectHandle DashEffectHandle;

	FTimerHandle PushForwardInputTimerHandle;

	void PushForward();

	UPROPERTY()
	UCharacterMovementComponent* OwnerCharacterMovementComponent;

	void UpdateCurrentDashDistance();

	UPROPERTY()
	UAbilityTask_WaitInputRelease* WaitInputReleaseTask;

	void ChargeFinished();
	
	// 蓄力开始（玩家按下技能按钮后触发）
	UFUNCTION()
	void StartCharge(float TimeWaited);

	UFUNCTION()
	void ChargeCompleted(float TimeHeld);
	
	void StartDash();

	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
	float DashMontagePlayDuration = 0.f;		// 冲刺动画播放时长

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float MaxChargeTime = 2.2f;		// 最大蓄力时间

	float CurrentCharacterMaxMoveSpeed = 0.f;	// 角色当前最大移动速度

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	float MaxDashDistance = 2000.f;	// 最大冲刺距离

	FTimerHandle ChargeTimer;			// 蓄力计时器
	float ChargeTimeInterval = 0.1f;	// 蓄力时间间隔

	float CurrentChargeTime = 0.f;		// 当前蓄力时间
};
