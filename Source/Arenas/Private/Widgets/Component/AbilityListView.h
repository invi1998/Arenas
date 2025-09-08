// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "Types/ArenaStructTypes.h"
#include "AbilityListView.generated.h"

class UAbilityGaugeWidget;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class ARENAS_API UAbilityListView : public UListView
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct();
	
	void ConfigureAbilities(const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& InAbilities);

private:
	UPROPERTY()
	TMap<EArenasAbilityInputID, UAbilityGaugeWidget*> AbilityWidgetsMap;
	
	
};
