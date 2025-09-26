// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_AbilitySystemGenerics.generated.h"

class UGameplayAbility;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class ARENAS_API UPA_AbilitySystemGenerics : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffectClass() const { return DeathEffectClass; }
	FORCEINLINE const TArray<TSubclassOf<UGameplayEffect>>& GetInitialGameplayEffects() const { return InitialGameplayEffects; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFullStateEffectClass() const { return FullStateEffectClass; }
	FORCEINLINE const TArray<TSubclassOf<UGameplayAbility>>& GetPassiveAbilities() const { return PassiveAbilities; }
	FORCEINLINE const UDataTable* GetBaseStatsDataTable() const { return BaseStatsDataTable; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialGameplayEffects;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStateEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	UDataTable* BaseStatsDataTable;

};
