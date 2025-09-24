// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Types/ArenaStructTypes.h"
#include "ArenasAbilitySystemComponent.generated.h"

class IPawnUIInterface;
/**
 * 
 */
UCLASS()
class UArenasAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UArenasAbilitySystemComponent();

	void InitializeBaseAttributes();
	void ServerSideInit();
	void ApplyFullStateEffect();

	const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }

	void AddGameplayTagToActorIfNotHas(FGameplayTag InTag);
	void RemoveGameplayTagFromActorIfHas(FGameplayTag InTag);

private:
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	
	void AuthApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 EffectLevel = 1);
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleManaChanged(const FOnAttributeChangeData& Data);
	void HandleIncomingDamage(const FOnAttributeChangeData& Data);

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialGameplayEffects;

	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStateEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	UDataTable* BaseStatsDataTable;

	FTimerHandle DamageStateTimerHandle;	// 伤害状态定时器

	UPROPERTY(EditDefaultsOnly, Category = "Damage State")
	bool bNeedHandleDamageState = false;	// 是否需要处理伤害状态

	UPROPERTY(EditDefaultsOnly, Category = "Damage State")
	float DamageStateDuration = 5.f;	// 伤害状态持续时间
	
};
