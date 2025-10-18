// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "GAS/ArenasAttributeSet.h"
#include "HeroStatsGauge.generated.h"

class UTextBlock;
class UImage;
class UArenasAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ARENAS_API UHeroStatsGauge : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	// 增益属性数值颜色（正数为绿色，负数为红色）
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FSlateColor PositiveGrowthColor = FSlateColor(FLinearColor::Green);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FSlateColor NegativeGrowthColor = FSlateColor(FLinearColor::Red);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	bool bUseDiffColorForGrowthRate = false;	// 是否对属性成长率使用不同的颜色

private:
	UPROPERTY()
	UArenasAbilitySystemComponent* CachedArenasASC;

	UPROPERTY(meta=(BindWidgetOptional))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	UTextBlock* AttributeText;

	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* AttributeGrowthRateText;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute AttributeToDisplay;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute AttributeGrowthRate;

	UPROPERTY(EditAnywhere, Category = "Visual")
	UTexture2D* IconTexture;

	void SetValue(float NewValue);
	void SetGrowthRateValue(float NewValue);

	FNumberFormattingOptions NumberFormattingOptions;
	FNumberFormattingOptions GrowthRateNumberFormattingOptions;

	void AttributeChanged(const FOnAttributeChangeData& Data);
	void AttributeGrowthRateChanged(const FOnAttributeChangeData& Data);

};
