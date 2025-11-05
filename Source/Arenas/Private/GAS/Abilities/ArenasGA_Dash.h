// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Dash.generated.h"

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
	UAnimMontage* DashMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetDetectionRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "GameplayCue")
	FGameplayTag LocalGameplayCueTag;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FName TargetActorAttachSocketName = "TargetDashCenter";

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

	UFUNCTION()
	void StartDash(FGameplayEventData Payload);

	UFUNCTION()
	void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	
	
	
};
