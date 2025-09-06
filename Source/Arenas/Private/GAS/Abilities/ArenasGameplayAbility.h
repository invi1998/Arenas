// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "ArenasGameplayAbility.generated.h"

class AArenasCharacter;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;	// 默认伤害GameplayEffect

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 10.f;	// 基础伤害
	
	UAnimInstance* GetOwnerAnimInstance() const;

	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetData, ETeamAttitude::Type TargetTeamType = ETeamAttitude::Hostile, float SphereSweepRadius = 30.f, bool bDebug = false, bool bIgnoreSelf = true) const;

	UPROPERTY(EditDefaultsOnly, Category = "Hit Sweep")
	float TargetSweepSphereRadius = 30.f;	// 目标检测球体扫描半径

	UPROPERTY(EditDefaultsOnly, Category = "Hit Sweep")
	bool bShowSweepDebug = false;
	
	// 推开角色
	void PushSelf(const FVector& PushVelocity);
	void PushTarget(AActor* TargetActor, const FVector& PushVelocity);

	AArenasCharacter* GetOwningArenasCharacter();

private:
	UPROPERTY()
	AArenasCharacter* OwningArenasCharacter;
	
};
