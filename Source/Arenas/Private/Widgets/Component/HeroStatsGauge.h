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

private:
	UPROPERTY()
	UArenasAbilitySystemComponent* CachedArenasASC;

	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AttributeText;

	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute AttributeToDisplay;

	UPROPERTY(EditAnywhere, Category = "Visual")
	UTexture2D* IconTexture;

	void SetValue(float NewValue);

	FNumberFormattingOptions NumberFormattingOptions;

	void AttributeChanged(const FOnAttributeChangeData& Data);

};
