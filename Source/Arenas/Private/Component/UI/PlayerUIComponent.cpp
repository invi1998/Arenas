// Ace of Arenas. (invi1998 All Rights Reserved)


#include "PlayerUIComponent.h"

#include "ArenasGameplayTags.h"
#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "GAS/ArenasHeroAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"


UPlayerUIComponent::UPlayerUIComponent()
{
	FormattingOptions.MaximumFractionalDigits = 0;	// 默认不显示小数位
}

void UPlayerUIComponent::OnExpChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Exp = OnAttributeChangeData.NewValue;
	if (MaxExp <= 0.f)
	{
		return;
	}
	if (Exp > MaxExp) return;	// 防止升级时，下一等级的经验计算还未完成，导致当前经验大于最大经验值，从而出现负数进度条
	float ExpPercent = UKismetMathLibrary::SafeDivide(Exp - StartExp, MaxExp - StartExp);
	
	OnExperiencePercentChanged.Broadcast(ExpPercent, FText::AsNumber(Exp, &FormattingOptions), FText::AsNumber(MaxExp, &FormattingOptions));
}

void UPlayerUIComponent::OnMaxExpChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxExp = OnAttributeChangeData.NewValue;
	if (MaxExp <= 0.f)
	{
		return;
	}
	float ExpPercent = UKismetMathLibrary::SafeDivide(Exp - StartExp, MaxExp - StartExp);
	OnExperiencePercentChanged.Broadcast(ExpPercent, FText::AsNumber(Exp, &FormattingOptions), FText::AsNumber(MaxExp, &FormattingOptions));
}

void UPlayerUIComponent::OnPrevExpChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	StartExp = OnAttributeChangeData.NewValue;
	if (MaxExp <= 0.f)
	{
		return;
	}
	float ExpPercent = UKismetMathLibrary::SafeDivide(Exp - StartExp, MaxExp - StartExp);
	OnExperiencePercentChanged.Broadcast(ExpPercent, FText::AsNumber(Exp, &FormattingOptions), FText::AsNumber(MaxExp, &FormattingOptions));
}

void UPlayerUIComponent::OnStrengthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	BoardCastHealthAndManaRegenState();
}

void UPlayerUIComponent::OnIntelligenceChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	BoardCastHealthAndManaRegenState();
}

void UPlayerUIComponent::SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC)
{
	Super::SetAndBoundAttributeDelegate(InArenasASC);

	bool bFound = false;
	float CurrentExp = CachedArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetExperienceAttribute(), bFound);
	float CurrentStartExp = CachedArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	float CurrentMaxExp = CachedArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);

	// 初始化当前属性值
	if (bFound)
	{
		Exp = CurrentExp;
		StartExp = CurrentStartExp;
		MaxExp = CurrentMaxExp;
			
		// 广播当前属性值
		OnExperiencePercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(Exp - StartExp, MaxExp - StartExp), FText::AsNumber(Exp, &FormattingOptions), FText::AsNumber(MaxExp, &FormattingOptions));
	}
	else
	{
		// 如果没有找到属性值，则将其设置为0
		Exp = 0.f;
		MaxExp = 0.f;
	}

	/*
	// 广播当前属性值
	if (const UArenasAttributeSet* AttributeSet = Cast<UArenasAttributeSet>(InArenasASC->GetAttributeSet(UArenasAttributeSet::StaticClass())))
	{
		AttributeSet->BroadcastAttributeInitialValue(this);
	}
	 */
		
	// 绑定属性变化的委托
	CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::OnExpChanged);
	CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ThisClass::OnMaxExpChanged);
	CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(this, &ThisClass::OnPrevExpChanged);
	
	// 监听 Status_Dead，Status_Stun，Status_Health_Full，Status_Mana_Full，Status_Damaged 状态，来计算并广播当前英雄回血回蓝数值状态
	CachedArenasASC->RegisterGameplayTagEvent(ArenasGameplayTags::Status_Dead, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnStatusTagChanged);
	CachedArenasASC->RegisterGameplayTagEvent(ArenasGameplayTags::Status_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnStatusTagChanged);
	// 同时还需要监听 力量和智力属性的变化
	CachedArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetStrengthAttribute()).AddUObject(this, &ThisClass::OnStrengthChanged);
	
}

void UPlayerUIComponent::BoardCastHealthAndManaRegenState()
{
	Super::BoardCastHealthAndManaRegenState();
	
	bool bInHealthRegenState = !bIsDead && !bIsStunned && !bIsHealthFull;
	bool bInManaRegenState = !bIsDead && !bIsStunned && !bIsManaFull;
	float CurrentHealthRegen = bInHealthRegenState ? GetCurrentHealthRegenNumber() : 0.f;
	float CurrentManaRegen = bInManaRegenState ? GetCurrentManaRegenNumber() : 0.f;
	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.MaximumFractionalDigits = 1;
	// 格式： +number / s
	FText HealthRegenText = FText::Format(FText::FromString("+{0} / s"), FText::AsNumber(CurrentHealthRegen, &NumberFormattingOptions));
	FText ManaRegenText = FText::Format(FText::FromString("+{0} / s"), FText::AsNumber(CurrentManaRegen, &NumberFormattingOptions));

	OnHealthRegenStateChanged.Broadcast(bInHealthRegenState, HealthRegenText);
	OnManaRegenStateChanged.Broadcast(bInManaRegenState, ManaRegenText);
	
}

float UPlayerUIComponent::GetCurrentHealthRegenNumber() const
{
	if (CachedArenasASC)
	{
		bool bFound = false;
		float Strength = CachedArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetStrengthAttribute(), bFound);
		float HealthRegen = CachedArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetHealthRegenAttribute(), bFound);
		if (bFound)
		{
			// 回血公式 0.1 * 力量值 + 基础回血（装备等提供）
			return 0.1f * Strength + HealthRegen;
		}
	}
	return 0.f;
}

float UPlayerUIComponent::GetCurrentManaRegenNumber() const
{
	if (CachedArenasASC)
	{
		bool bFound = false;
		float Intelligence = CachedArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetIntelligenceAttribute(), bFound);
		float ManaRegen = CachedArenasASC->GetGameplayAttributeValue(UArenasAttributeSet::GetManaRegenAttribute(), bFound);
		if (bFound)
		{
			// 回蓝公式 0.05 * 智力值 + 基础回蓝（装备等提供）
			return 0.05f * Intelligence + ManaRegen;
		}
	}
	return 0.f;
}

void UPlayerUIComponent::OnStatusTagChanged(FGameplayTag StatTag, int32 Count)
{
	if (!StatTag.IsValid()) return;

	if (StatTag == ArenasGameplayTags::Status_Dead)
	{
		bIsDead = Count > 0;
	}
	else if (StatTag == ArenasGameplayTags::Status_Stun)
	{
		bIsStunned = Count > 0;
	}
	
	BoardCastHealthAndManaRegenState();
}
