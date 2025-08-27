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
	void ApplyInitialEffects();
	void GiveInitialAbilities();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialGameplayEffects;

	TWeakInterfacePtr<IPawnUIInterface> CachedPawnUIInterface;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;
	
};
