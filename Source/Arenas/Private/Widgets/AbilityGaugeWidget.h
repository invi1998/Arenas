// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "ArenasUserWidget.h"
#include "AbilityGaugeWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 技能能量条Widget
 */
UCLASS()
class ARENAS_API UAbilityGaugeWidget : public UArenasUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownCountdownText;	// 冷却倒计时文本
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownDurationText;	// 冷却总时长文本

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CostText;				// 消耗文本
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* InputKeyText;			// 施法按键
	
};
