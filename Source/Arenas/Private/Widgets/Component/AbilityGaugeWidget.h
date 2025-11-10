// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "Types/ArenaStructTypes.h"
#include "AbilityGaugeWidget.generated.h"

class UAbilityToolTip;
class UArenasAbilitySystemComponent;
struct FGameplayAbilitySpec;
class UTextBlock;
class UImage;
/**
 * 技能能量条Widget
 */
UCLASS()
class ARENAS_API UAbilityGaugeWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void ConfigureWidgetData(const FAbilityWidgetData* AbilityWidgetData);

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "AbilityIcon";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CooldownMaterialParamName = "Percent";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CanCastAbilityMaterialParamName = "CanCast";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName UpgradePointAvailableMaterialParamName = "UpgradePointAvailable";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName HasEnoughManaMaterialParamName = "HasEnoughMana";	// 是否有足够的法力值施放技能

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float CooldownUpdateInterval = 0.1f; // 冷却倒计时文本更新间隔
	
	UPROPERTY(meta=(BindWidget))
	UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	UImage* LevelImage;		// 等级图标

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName LevelMaterialParamName = "Level";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName MaxLevelMaterialParamName = "MaxLevel";

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownCountdownText;	// 冷却倒计时文本
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* CooldownDurationText;	// 冷却总时长文本

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CostText;				// 消耗文本
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* InputKeyText;			// 施法按键

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UAbilityToolTip> AbilityToolTipClass;

	UPROPERTY()
	UGameplayAbility* AbilityCDO;		// 该技能的CDO(Class Default Object)

	void CreateToolTip(const FAbilityWidgetData* AbilityWidgetData);

	void OnAbilityCommited(UGameplayAbility* InAbility);

	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);
	
	void CooldownFinished();
	void UpdateCooldown();

	// 在倒计时文本上显示剩余时间，并根据剩余时间的不同选择不同的格式化选项，在时间大于1时显示整数，小于1时显示1位小数
	FNumberFormattingOptions WholeNumberFormatOptions;		// 整数格式化选项
	FNumberFormattingOptions OneDigitNumberFormatOptions;	// 1位小数格式化选项

	float CachedCooldownDuration = 0.f;
	float CachedCooldownTimeRemaining = 0.f;
	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownUpdateTimerHandle;	// 用于更新倒计时文本(每0.1秒更新一次)

	UPROPERTY()
	const UArenasAbilitySystemComponent* OwnerAbilitySystemComponent;

	FGameplayAbilitySpecHandle CachedAbilitySpecHandle;

	const FGameplayAbilitySpec* GetAbilitySpec();

	bool bIsAbilityLearned = false;
	bool bIsCanUpgrade = true;
	bool bCanAffordAbilityCost = true;

	void OnAbilitySpecDirtied(const FGameplayAbilitySpec& GameplayAbilitySpec);
	void UpgradePointUpdated(const FOnAttributeChangeData& OnAttributeChangeData);
	void ManaUpdated(const FOnAttributeChangeData& OnAttributeChangeData);

	void UpdateCanCastAbilityVisual();

	
};
