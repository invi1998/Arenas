// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Types/ArenaStructTypes.h"
#include "ArenasAbilitySystemComponent.generated.h"

class UPA_AbilitySystemGenerics;
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

	UPROPERTY(EditDefaultsOnly, Category = "PA_Generics")
	UPA_AbilitySystemGenerics* AbilitySystemGenerics;		// 赋予初始属性和效果
	
	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;
	
};
