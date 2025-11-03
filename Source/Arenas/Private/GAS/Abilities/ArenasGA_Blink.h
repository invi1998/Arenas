// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_Blink.generated.h"

class ATargetActor_GroundPick;
/**
 * 闪现能力
 */
UCLASS()
class ARENAS_API UArenasGA_Blink : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_Blink();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Blink")
	float BlinkDistance = 1000.f;		// 闪现距离

	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TargetingMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Anim")
	UAnimMontage* TeleportMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float BlinkLandTargetPushSpeed = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 1000;

	UFUNCTION()
	void GroundPickTargetReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void GroundPickCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION()
	void Teleport(FGameplayEventData Payload);

	FGameplayAbilityTargetDataHandle BlinkTargetDataHandle;
	
};
