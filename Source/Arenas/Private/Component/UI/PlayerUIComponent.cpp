// Ace of Arenas. (invi1998 All Rights Reserved)


#include "PlayerUIComponent.h"

#include "GAS/ArenasAbilitySystemComponent.h"
#include "GAS/ArenasAttributeSet.h"
#include "GAS/ArenasHeroAttributeSet.h"
#include "Kismet/KismetMathLibrary.h"


void UPlayerUIComponent::OnExpChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	
	Exp = OnAttributeChangeData.NewValue;
	if (MaxExp <= 0.f)
	{
		return;
	}
	float HealthPercent = UKismetMathLibrary::SafeDivide(Exp, MaxExp);
	
	OnExperiencePercentChanged.Broadcast(HealthPercent, Exp, MaxExp);
}

void UPlayerUIComponent::OnMaxExpChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	MaxExp = OnAttributeChangeData.NewValue;
	if (MaxExp <= 0.f)
	{
		return;
	}
	float HealthPercent = UKismetMathLibrary::SafeDivide(Exp, MaxExp);
	OnExperiencePercentChanged.Broadcast(HealthPercent, Exp, MaxExp);
}

void UPlayerUIComponent::SetAndBoundAttributeDelegate(UArenasAbilitySystemComponent* InArenasASC)
{
	Super::SetAndBoundAttributeDelegate(InArenasASC);

	if (InArenasASC)
	{
		bool bFound = false;
		float CurrentExp = InArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetExperienceAttribute(), bFound);
		float CurrentMaxExp = InArenasASC->GetGameplayAttributeValue(UArenasHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);

		// 初始化当前属性值
		if (bFound)
		{
			Exp = CurrentExp;
			MaxExp = CurrentMaxExp;
			
			// 广播当前属性值
			OnExperiencePercentChanged.Broadcast(UKismetMathLibrary::SafeDivide(Exp, MaxExp), Exp, MaxExp);
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
		InArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::OnExpChanged);
		InArenasASC->GetGameplayAttributeValueChangeDelegate(UArenasHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ThisClass::OnMaxExpChanged);
		
	}
	
}
