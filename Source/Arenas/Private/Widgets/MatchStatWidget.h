// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStatWidget.generated.h"

class AArenasPlayerController;
class UHeroStatsGauge;
/**
 * 
 */
UCLASS()
class ARENAS_API UMatchStatWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	void SetOwningArenasPlayerController(AArenasPlayerController* InArenasPC);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnOwningArenasPlayerControllerSet(AArenasPlayerController* InArenasPC);


private:
	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* KillsGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* DeathsGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* AssistsGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* GoldEarnedGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* DamageDealtGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* MinionKillsGauge;

	UPROPERTY()
	AArenasPlayerController* OwningArenasPlayerController;
	
};
