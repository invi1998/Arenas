// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeroUserWidget.generated.h"

class UHeroStatsGauge;
class UTextBlock;
class UImage;
class UPlayerUIComponent;
class UArenasAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ARENAS_API UHeroUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

protected:
	// NativeOnInitialized 先执行，NativeConstruct 后执行
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta=(DisplayName = "On Owning Player UI Component Initialized"))
	void BP_OnOwningPlayerUIComponentInitialized(UPlayerUIComponent* InPlayerUIComponent);

private:
	UPROPERTY()
	UArenasAbilitySystemComponent* CachedArenasASC;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* AttackDamageGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* ArmorGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* AttackRangeGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* AttackSpeedGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* MoveSpeedGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* StrengthGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* IntelligenceGauge;

	UPROPERTY(meta=(BindWidget))
	UHeroStatsGauge* AgilityGauge;

};
