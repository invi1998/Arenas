// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenasGA_GroundBlast.generated.h"

class ATargetActor_GroundPick;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGA_GroundBlast : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenasGA_GroundBlast();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* GroundBlastMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<ATargetActor_GroundPick> GroundPickTargetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceDistance = 2000.f;	// 目标追踪距离

	// 技能施法目标区域半径
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;

	// 是否应该选择敌人
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	bool bShouldTargetEnemies = true;

	// 是否应该选择友军
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	bool bShouldTargetAllies = false;
	
	UFUNCTION()
	void OnTargetConfirmed(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);

	UFUNCTION()
	void OnTargetCancelled(const FGameplayAbilityTargetDataHandle& InTargetDataHandle);
	
};
