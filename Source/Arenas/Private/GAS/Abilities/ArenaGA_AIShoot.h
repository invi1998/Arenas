// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasGameplayAbility.h"
#include "ArenaGA_AIShoot.generated.h"

class AProjectileActor;
/**
 * 
 */
UCLASS()
class ARENAS_API UArenaGA_AIShoot : public UArenasGameplayAbility
{
	GENERATED_BODY()

public:
	UArenaGA_AIShoot();
	static FGameplayTag GetComboChangeEventTag();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


private:
	virtual FGameplayTag GetDamageEventTag() const;

	UFUNCTION()
	void ShootProjectile(FGameplayEventData Payload);

	UPROPERTY(EditDefaultsOnly, Category = "Shooting")
	TSubclassOf<AProjectileActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Shooting")
	float ProjectileSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shooting")
	float ShootProjectileRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shooting")
	TSubclassOf<UGameplayEffect> ProjectileHitEffect;

	FGenericTeamId GetOwnerTeamId() const;

	void SetupWaitComboInputPressTask();
	void TryCommitCombo();

	int32 GetCurrentComboIndex() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ComboMontage;

	UFUNCTION()
	void OnComboChangeEventReceived(FGameplayEventData PayloadData);

	UFUNCTION()
	void OnComboInputPressed(float TimeWaited);

	FName NextComboName;
	
};
