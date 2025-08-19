// Ace of Arenas. (invi1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ArenasAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class UArenasAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void ApplyInitialEffects();

private:
	TArray<TSubclassOf<UGameplayEffect>> InitialGameplayEffects;
	
};
