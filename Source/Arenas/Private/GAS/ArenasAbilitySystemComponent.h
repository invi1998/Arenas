// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Types/ArenaStructTypes.h"
#include "ArenasAbilitySystemComponent.generated.h"

class UPA_AbilitySystemGenerics;
class IPawnUIInterface;

// 伤害变化委托（伤害来源，伤害数值）
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAnyDamageTakenDelegate, AActor*, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorDeathDelegate, AActor*);

/**
 * 
 */
UCLASS()
class UArenasAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UArenasAbilitySystemComponent();

	FOnAnyDamageTakenDelegate OnAnyDamageTaken;
	FOnActorDeathDelegate OnActorDeath;

	void InitializeBaseAttributes();
	void ServerSideInit();
	void ApplyFullStateEffect();

	const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }

	void AddGameplayTagToActorIfNotHas(FGameplayTag InTag);
	void RemoveGameplayTagFromActorIfHas(FGameplayTag InTag);

	bool IsAtMaxLevel() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpgradeAbilityWithInputID(EArenasAbilityInputID AbilityID);

	UFUNCTION(Client, Reliable)
	void Client_AbilityUpgradeSuccess(FGameplayAbilitySpecHandle AbilitySpecHandle, int32 NewAbilityLevel);

private:
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	
	void AuthApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 EffectLevel = 1);
	void HandleHealthChanged(const FOnAttributeChangeData& Data);
	void HandleManaChanged(const FOnAttributeChangeData& Data);
	void HandleExperienceChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(EditDefaultsOnly, Category = "PA_Generics")
	UPA_AbilitySystemGenerics* AbilitySystemGenerics;		// 赋予初始属性和效果
	
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;
	
};
