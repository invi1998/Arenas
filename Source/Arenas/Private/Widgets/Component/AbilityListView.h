// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "Types/ArenaStructTypes.h"
#include "AbilityListView.generated.h"

struct FAbilityWidgetData;
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

	void OnAbilityWidgetGenerated(UUserWidget& AbilityUserWidget);
	void ConfigureAbilities(const TMap<EArenasAbilityInputID, TSubclassOf<UGameplayAbility>>& InAbilities);

private:
	UPROPERTY(EditAnywhere, Category = "Data")
	UDataTable* AbilityDataTable;
	
	const FAbilityWidgetData* FindWidgetDataByAbilityClass(const TSubclassOf<UGameplayAbility>& InAbilityClass) const;
};
