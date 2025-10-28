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

public:
	UArenasGameplayAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DefaultDamageEffect;	// 默认伤害GameplayEffect

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage = 10.f;	// 基础伤害
	
	UAnimInstance* GetOwnerAnimInstance() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Hit Sweep")
	bool bShowDebugLine = false;
	
	// 推开角色
	void PushSelf(const FVector& PushVelocity);
	void PushTarget(AActor* TargetActor, const FVector& PushVelocity);
	void PushTargets(const TArray<AActor*>& TargetActors, const FVector& PushVelocity);
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVelocity);
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& CenterPoint, float PushSpeed);

	void PlayMontageLocally(UAnimMontage* MontageToPlay);
	void StopMontageAfterCurrentSection(UAnimMontage* MontageToStop);

	AArenasCharacter* GetOwningArenasCharacter();

	AActor* GetAimActorTarget(float AimDistance, ETeamAttitude::Type AimTeamAttitude) const;

	bool IsActorTeamAttitude(AActor* OtherActor, ETeamAttitude::Type DesiredAttitude) const;

	void SendLocalGameplayEvent(const FGameplayTag EventTag, const FGameplayEventData& Payload);

	FGenericTeamId GetOwningTeamId() const;

private:
	UPROPERTY()
	AArenasCharacter* OwningArenasCharacter;
	
};
